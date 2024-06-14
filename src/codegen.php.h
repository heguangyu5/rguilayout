/**********************************************************************************************
*
*   rGuiLayout - Code Generator
*
*   CODEGEN SUPPORTED VARIABLES:
*
*       > Layout general description variables
*           $(GUILAYOUT_NAME)
*               $(GUILAYOUT_NAME_PASCALCASE)
*               $(GUILAYOUT_NAME_UPPERCASE)
*               $(GUILAYOUT_NAME_LOWERCASE)
*           $(GUILAYOUT_VERSION)
*           $(GUILAYOUT_DESCRIPTION)
*           $(GUILAYOUT_COMPANY)
*           $(GUILAYOUT_WINDOW_WIDTH)
*           $(GUILAYOUT_WINDOW_HEIGHT)
*
*       > Layout C file (.c) data generation variables:
*           $(GUILAYOUT_FUNCTION_DEFINITION_C)
*           $(GUILAYOUT_INITIALIZATION_C)
*           $(GUILAYOUT_DRAWING_C)
*
*   NOTE: Code generated requires raygui 3.5-dev
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2018-2022 raylib technologies (@raylibtech) / Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "rguilayout.h"

#ifndef CODEGEN_H
#define CODEGEN_H

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
unsigned char *GenLayoutCode(const unsigned char *buffer, GuiLayout *layout, Vector2 offset, GuiLayoutConfig config);

#ifdef __cplusplus
}
#endif

#endif // CODEGEN_H

/***********************************************************************************
*
*   CODEGEN_IMPLEMENTATION
*
************************************************************************************/
#if defined(CODEGEN_IMPLEMENTATION)

#define TABAPPEND(x, y, z)          { for (int t = 0; t < z; t++) TextAppend(x, "    ", y); }
#define ENDLINEAPPEND(x, y)         TextAppend(x, "\n", y);

//----------------------------------------------------------------------------------
// Global variables definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------

// .C Writting code functions (.c)
static void WriteInitializationC(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs);
static void WriteDrawingC(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs);
static void WriteFunctionsDefinitionC(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs);

// Generic writting code functions (.c/.h)
static void WriteRectangleVariables(unsigned char *toolstr, int *pos, GuiLayoutControl control, bool exportAnchors, bool fullComments, int tabs);
static void WriteAnchors(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs);
static void WriteControlsVariables(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs);
static void WriteControlsDrawing(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs);
static void WriteControlDraw(unsigned char *toolstr, int *pos, int index, GuiLayoutControl control, GuiLayoutConfig config);

// Get controls specific texts functions
static char *GetControlRectangleText(int index, GuiLayoutControl control, bool defineRecs, bool exportAnchors);
static char *GetScrollPanelContainerRecText(int index, GuiLayoutControl control, bool defineRecs, bool exportAnchors);
static char *GetControlTextParam(GuiLayoutControl control);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Generate layout code string
// TODO: WARNING: layout is passed as value, probably not a good idea considering the size of the object
unsigned char *GenLayoutCode(const unsigned char *buffer, GuiLayout *layout, Vector2 offset, GuiLayoutConfig config)
{
    #define MAX_CODE_SIZE            1024*1024       // Max code size: 1MB
    #define MAX_VARIABLE_NAME_SIZE     64

    if (buffer == NULL) return NULL;

    unsigned char *toolstr = (unsigned char *)RL_CALLOC(MAX_CODE_SIZE, sizeof(unsigned char));
    unsigned const char *substr = NULL;

    int bufferPos = 0;
    int codePos = 0;
    int bufferLen = (int)strlen(buffer);

    // Offset all enabled anchors from reference window and offset
    for (int a = 1; a < MAX_ANCHOR_POINTS; a++)
    {
        if (layout->anchors[a].enabled)
        {
            layout->anchors[a].x -= (int)(layout->refWindow.x + offset.x);
            layout->anchors[a].y -= (int)(layout->refWindow.y + offset.y);
        }
    }

    // In case of controls with no anchor, offset must be applied to control position
    // NOTE: refWindow (aka anchor[0]) is already implicit!
    for (int i = 0; i < layout->controlCount; i++)
    {
        if (layout->controls[i].ap->id == 0)
        {
            layout->controls[i].rec.x -= offset.x;
            layout->controls[i].rec.y -= offset.y;
        }
    }

    int i = 0;
    for (i = 0; i < bufferLen; i++)
    {
        if ((buffer[i] == '$') && (buffer[i + 1] == '('))       // Detect variable to be replaced!
        {
            int spaceWidth = 0;
            for (int j = i-1; j >= 0; j--)
            {
                if (buffer[j] == ' ') spaceWidth++;
                else
                {
                    if (buffer[j] != '\n') spaceWidth = 0;
                    break;
                }
            }
            int tabs = (int)spaceWidth/4;

            substr = TextSubtext(buffer, bufferPos, i - bufferPos);

            strcpy(toolstr + codePos, substr);
            codePos = (int)strlen(toolstr);

            i += 2;
            bufferPos = i;

            for (int j = 0; j < MAX_VARIABLE_NAME_SIZE; j++)
            {
                if (buffer[i + j] == ')')
                {
                    substr = TextSubtext(buffer, i, j);

                    if (TextIsEqual(substr, "GUILAYOUT_NAME")) TextAppend(toolstr, config.name, &codePos);
                    else if (TextIsEqual(substr, "GUILAYOUT_NAME_UPPERCASE")) TextAppend(toolstr, TextToUpper(config.name), &codePos);
                    else if (TextIsEqual(substr, "GUILAYOUT_NAME_LOWERCASE")) TextAppend(toolstr, TextToLower(config.name), &codePos);
                    else if (TextIsEqual(substr, "GUILAYOUT_NAME_PASCALCASE")) TextAppend(toolstr, TextToPascal(config.name), &codePos);
                    else if (TextIsEqual(substr, "GUILAYOUT_VERSION")) TextAppend(toolstr, config.version, &codePos);
                    else if (TextIsEqual(substr, "GUILAYOUT_DESCRIPTION")) TextAppend(toolstr, config.description, &codePos);
                    else if (TextIsEqual(substr, "GUILAYOUT_COMPANY")) TextAppend(toolstr, config.company, &codePos);
                    else if (TextIsEqual(substr, "GUILAYOUT_WINDOW_WIDTH"))
                    {
                       if (layout->refWindow.width > 0) TextAppend(toolstr, TextFormat("%i", (int)layout->refWindow.width), &codePos);
                       else TextAppend(toolstr, TextFormat("%i", 800), &codePos);
                    }
                    else if (TextIsEqual(substr, "GUILAYOUT_WINDOW_HEIGHT"))
                    {
                       if (layout->refWindow.height > 0) TextAppend(toolstr, TextFormat("%i", (int)layout->refWindow.height), &codePos);
                       else TextAppend(toolstr, TextFormat("%i", 450), &codePos);
                    }

                    // C IMPLEMENTATION
                    else if (TextIsEqual(substr, "GUILAYOUT_INITIALIZATION_C")) WriteInitializationC(toolstr, &codePos, layout, config, tabs);
                    else if (TextIsEqual(substr, "GUILAYOUT_DRAWING_C") && layout->controlCount > 0) WriteDrawingC(toolstr, &codePos, layout, config, tabs);
                    else if (TextIsEqual(substr, "GUILAYOUT_FUNCTION_DEFINITION_C")) WriteFunctionsDefinitionC(toolstr, &codePos, layout, config, tabs);

                    bufferPos += (j + 1);

                    break;
                }
            }
        }
    }

    // Copy remaining text into toolstr
    strcpy(toolstr + codePos, buffer + bufferPos);

    // In case of controls with no anchor, offset must be applied to control position
    for (int i = 0; i < layout->controlCount; i++)
    {
        if (layout->controls[i].ap->id == 0)
        {
            layout->controls[i].rec.x += offset.y;
            layout->controls[i].rec.y += offset.y;
        }
    }

    // Restored all enabled anchors to reference window and offset
    for (int a = 1; a < MAX_ANCHOR_POINTS; a++)
    {
        if (layout->anchors[a].enabled)
        {
            layout->anchors[a].x += (int)(layout->refWindow.x + offset.x);
            layout->anchors[a].y += (int)(layout->refWindow.y + offset.y);
        }
    }

    return toolstr;
}

// Write variables initialization code (.c)
static void WriteInitializationC(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs)
{
    // Anchors
    if (config.exportAnchors && (layout->anchorCount > 0)) WriteAnchors(toolstr, pos, layout, config, tabs);

    // Control variables
    if (layout->controlCount > 0) WriteControlsVariables(toolstr, pos, layout, config, tabs);

    // Rectangles
    if (config.defineRecs)
    {
        ENDLINEAPPEND(toolstr, pos);
        ENDLINEAPPEND(toolstr, pos);
        TABAPPEND(toolstr, pos, tabs);
        // Define controls rectangles
        if (config.fullComments)
        {
            TextAppend(toolstr, "// Define controls rectangles", pos);
            ENDLINEAPPEND(toolstr, pos);
            TABAPPEND(toolstr, pos, tabs);
        }
        TextAppend(toolstr, TextFormat("$layoutRecs = array(", layout->controlCount), pos);
        ENDLINEAPPEND(toolstr, pos);

        for (int k = 0; k < layout->controlCount; k++)
        {
            TABAPPEND(toolstr, pos, tabs + 1);
            WriteRectangleVariables(toolstr, pos, layout->controls[k], config.exportAnchors, config.fullComments, tabs);
            ENDLINEAPPEND(toolstr, pos);
        }
        TABAPPEND(toolstr, pos, tabs);
        TextAppend(toolstr, ");", pos);
    }
}

// Write drawing code (.c)
static void WriteDrawingC(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs)
{
    if (layout->controlCount > 0) WriteControlsDrawing(toolstr, pos, layout, config, tabs);
}

// Write functions definition code (.c)
static void WriteFunctionsDefinitionC(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs)
{
    if(config.exportButtonFunctions)
    {
        for (int i = 0; i < layout->controlCount; i++)
        {
            int type = layout->controls[i].type;
            if (type == GUI_BUTTON || type == GUI_LABELBUTTON)
            {
                if (config.fullComments)
                {
                    TextAppend(toolstr, TextFormat("// %s: %s logic", controlTypeName[layout->controls[i].type], layout->controls[i].name), pos);
                    ENDLINEAPPEND(toolstr, pos);
                    TABAPPEND(toolstr, pos, tabs);
                }

                TextAppend(toolstr, TextFormat("function %s()", TextToPascal(layout->controls[i].name)), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
                TextAppend(toolstr, "{", pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs + 1);
                TextAppend(toolstr, TextFormat("echo \"%s clicked!\\n\";", layout->controls[i].name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
                TextAppend(toolstr, "}", pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
            }
        }
    }
}

//----------------------------------------------------------------------------------
// Generic writting code functions (.c/.h)
//----------------------------------------------------------------------------------

// Write rectangle variables code (.c/.h)
static void WriteRectangleVariables(unsigned char *toolstr, int *pos, GuiLayoutControl control, bool exportAnchors, bool fullComments, int tabs)
{
    if (exportAnchors && control.ap->id > 0)
    {
        TextAppend(toolstr, TextFormat("array($%s[0] + %i, $%s[1] + %i, %i, %i)", control.ap->name, (int)control.rec.x, control.ap->name, (int)control.rec.y, (int)control.rec.width, (int)control.rec.height), pos);
    }
    else
    {
        if (control.ap->id > 0) TextAppend(toolstr, TextFormat("array(%i, %i, %i, %i)", (int)control.rec.x + control.ap->x, (int)control.rec.y + control.ap->y, (int)control.rec.width, (int)control.rec.height), pos);
        else TextAppend(toolstr, TextFormat("array(%i, %i, %i, %i)", (int)control.rec.x - control.ap->x, (int)control.rec.y - control.ap->y, (int)control.rec.width, (int)control.rec.height), pos);
    }

    TextAppend(toolstr, ",", pos);

    if (fullComments)
    {
        TABAPPEND(toolstr, pos, tabs);
        TextAppend(toolstr, TextFormat("// %s: %s",controlTypeName[control.type], control.name), pos);
    }
}

// Write anchors code (.c/.h)
static void WriteAnchors(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs)
{
    if (config.fullComments)
    {
        TextAppend(toolstr, "// Define anchors", pos);
        ENDLINEAPPEND(toolstr, pos);
        TABAPPEND(toolstr, pos, tabs);
    }

    for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
    {
        GuiAnchorPoint anchor = layout->anchors[i];

        if (anchor.enabled)
        {
            TextAppend(toolstr, TextFormat("$%s = array(%i, %i);", anchor.name, (int)layout->anchors[i].x, (int)layout->anchors[i].y), pos);
            if (config.fullComments)
            {
                TABAPPEND(toolstr, pos, 3);
                TextAppend(toolstr, TextFormat("// ANCHOR ID:%i", anchor.id), pos);
            }

            ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
        }
    }
    ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
}

// Write controls variables code (.c/.h)
static void WriteControlsVariables(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs)
{
    if (config.fullComments)
    {
        TextAppend(toolstr, "// Define controls variables", pos);
        ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
    }

    for (int i = 0; i < layout->controlCount; i++)
    {
        bool drawVariables = true;
        GuiLayoutControl control = layout->controls[i];

        switch (control.type)
        {
            case GUI_WINDOWBOX:
            {
                TextAppend(toolstr, TextFormat("$%sActive = true;", control.name), pos);
            } break;
            case GUI_BUTTON:
            case GUI_LABELBUTTON:
            {
                if(!config.exportButtonFunctions)
                {
                    TextAppend(toolstr, TextFormat("$%sPressed = false;", control.name), pos);
                }
                else drawVariables = false;
            } break;
            case GUI_CHECKBOX:
            {
                TextAppend(toolstr, TextFormat("$%sChecked = false;", control.name), pos);
            } break;
            case GUI_TOGGLE:
            {
                TextAppend(toolstr, TextFormat("$%sActive = true;", control.name), pos);
            } break;
            case GUI_TOGGLEGROUP:
            case GUI_COMBOBOX:
            {
                TextAppend(toolstr, TextFormat("$%sActive = 0;", control.name), pos);
            } break;
            case GUI_LISTVIEW:
            {
                TextAppend(toolstr, TextFormat("$%sScrollIndex = 0;", control.name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);

                TextAppend(toolstr, TextFormat("$%sActive = 0;", control.name), pos);
            } break;
            case GUI_DROPDOWNBOX:
            {
                TextAppend(toolstr, TextFormat("$%sEditMode = false;", control.name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);

                TextAppend(toolstr, TextFormat("$%sActive = 0;", control.name), pos);
            } break;
            case GUI_TEXTBOX:
            case GUI_TEXTBOXMULTI:
            {
                TextAppend(toolstr, TextFormat("$%sEditMode = false;", control.name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);

                TextAppend(toolstr, TextFormat("$%sText = str_pad(\"%s\", %i, \"\\0\");", control.name, control.text, MAX_CONTROL_TEXT_LENGTH), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);

                TextAppend(toolstr, TextFormat("$%sFilledLen = 0;", control.name), pos);
            } break;
            case GUI_VALUEBOX:
            case GUI_SPINNER:
            {
                TextAppend(toolstr, TextFormat("$%sEditMode = false;", control.name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);

                TextAppend(toolstr, TextFormat("$%sValue = 0;", control.name), pos);
            } break;
            case GUI_SLIDER:
            case GUI_SLIDERBAR:
            case GUI_PROGRESSBAR:
            {
                TextAppend(toolstr, TextFormat("$%sValue = 0.0;", control.name), pos);
            } break;
            case GUI_COLORPICKER:
            {
                TextAppend(toolstr, TextFormat("$%sValue = array(0, 0, 0, 0);", control.name), pos);
            } break;
            case GUI_SCROLLPANEL:
            {
                TextAppend(toolstr, TextFormat("$%sScrollView = array(0, 0, 0, 0);", control.name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);

                TextAppend(toolstr, TextFormat("$%sScrollOffset = array(0, 0);", control.name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);

                TextAppend(toolstr, TextFormat("$%sBoundsOffset = array(0, 0);", control.name), pos);
            } break;
            case GUI_GROUPBOX:
            case GUI_LINE:
            case GUI_PANEL:
            case GUI_LABEL:
            case GUI_DUMMYREC:
            case GUI_STATUSBAR:
            default:
            {
                drawVariables = false;
            }break;
        }

        if (drawVariables)
        {
            if (config.fullComments)
            {
                TABAPPEND(toolstr, pos, 3);
                TextAppend(toolstr, TextFormat("// %s: %s", controlTypeName[layout->controls[i].type], layout->controls[i].name), pos);
            }
            ENDLINEAPPEND(toolstr, pos);
            TABAPPEND(toolstr, pos, tabs);
        }
    }
    *pos -= tabs*4 + 1; // Remove last \n\t
}

// Write controls drawing code (full block) (.c/.h)
static void WriteControlsDrawing(unsigned char *toolstr, int *pos, GuiLayout *layout, GuiLayoutConfig config, int tabs)
{
    if (config.fullComments)
    {
        TextAppend(toolstr, "// Draw controls", pos);
        ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
    }

    bool *draw = (bool *)RL_CALLOC(layout->controlCount, sizeof(bool));
    for (int i = 0; i < layout->controlCount; i++) draw[i] = false;

    // If dropdown control exist, draw GuiLock condition
    bool dropDownExist = false;
    for (int i = 0; i < layout->controlCount; i++)
    {
        if (layout->controls[i].type == GUI_DROPDOWNBOX)
        {
            if (!dropDownExist)
            {
                dropDownExist = true;
                TextAppend(toolstr, "if (", pos);
            }
            else
            {
                TextAppend(toolstr, " || ", pos);
            }
            TextAppend(toolstr, TextFormat("$%sEditMode", layout->controls[i].name), pos);
        }
    }

    if (dropDownExist)
    {
        TextAppend(toolstr, ") GuiLock();", pos);
        ENDLINEAPPEND(toolstr, pos); ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
    }

    // Draw GUI_WINDOWBOX
    for (int i = 0; i < layout->controlCount; i++)
    {
        if (!draw[i])
        {
            if (layout->controls[i].type == GUI_WINDOWBOX)
            {
                draw[i] = true;

                //char *rec = GetControlRectangleText(i, layout->controls[i], config.defineRecs, config.exportAnchors);

                TextAppend(toolstr, TextFormat("if ($%sActive)", layout->controls[i].name), pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
                TextAppend(toolstr, "{", pos);

                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs + 1);
                WriteControlDraw(toolstr, pos, i, layout->controls[i], config); // Draw GUI_WINDOWBOX
                ENDLINEAPPEND(toolstr, pos);

                int windowAnchorID = layout->controls[i].ap->id;

                // Draw controls inside window
                for (int j = 0; j < layout->controlCount; j++)
                {
                    if (!draw[j] && i != j && layout->controls[j].type != GUI_WINDOWBOX  && layout->controls[j].type != GUI_DROPDOWNBOX)
                    {
                        if (windowAnchorID == layout->controls[j].ap->id)
                        {
                            draw[j] = true;

                            TABAPPEND(toolstr, pos, tabs + 1);
                            WriteControlDraw(toolstr, pos, j, layout->controls[j], config);
                            ENDLINEAPPEND(toolstr, pos);
                        }
                    }
                }

                // Draw GUI_DROPDOWNBOX inside GUI_WINDOWBOX
                for (int j = 0; j < layout->controlCount; j++)
                {
                    if (!draw[j] && i != j && layout->controls[j].type == GUI_DROPDOWNBOX)
                    {
                        if (windowAnchorID == layout->controls[j].ap->id)
                        {
                            draw[j] = true;

                            TABAPPEND(toolstr, pos, tabs + 1);
                            WriteControlDraw(toolstr, pos, j, layout->controls[j], config);
                            ENDLINEAPPEND(toolstr, pos);
                        }
                    }
                }
                TABAPPEND(toolstr, pos, tabs);
                TextAppend(toolstr, "}", pos);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
            }
        }
    }

    // Draw the rest of controls except dropdownbox
    for (int i = 0; i < layout->controlCount; i++)
    {
        if (!draw[i])
        {
            if (layout->controls[i].type != GUI_DROPDOWNBOX)
            {
                draw[i] = true;
                WriteControlDraw(toolstr, pos, i, layout->controls[i], config);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
            }
        }
    }

    // Draw GUI_DROPDOWNBOX
    for (int i = 0; i < layout->controlCount; i++)
    {
        if (!draw[i])
        {
            if (layout->controls[i].type == GUI_DROPDOWNBOX)
            {
                draw[i] = true;
                WriteControlDraw(toolstr, pos, i, layout->controls[i], config);
                ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
            }
        }
    }

    if (dropDownExist)
    {
        ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
        TextAppend(toolstr, "GuiUnlock();", pos);
        ENDLINEAPPEND(toolstr, pos); TABAPPEND(toolstr, pos, tabs);
    }

    *pos -= (tabs)*4 + 1; // Delete last tabs and \n

    RL_FREE(draw);
}

// Write control drawing code (individual controls) (.c/.h)
static void WriteControlDraw(unsigned char *toolstr, int *pos, int index, GuiLayoutControl control, GuiLayoutConfig config)
{
    char *rec = GetControlRectangleText(index, control, config.defineRecs, config.exportAnchors);
    char *text = GetControlTextParam(control);
    char *name = control.name;

    // TODO: Define text for window, groupbox, buttons, toggles and dummyrecs
    switch (control.type)
    {
        case GUI_WINDOWBOX: TextAppend(toolstr, TextFormat("$%sActive = !GuiWindowBox(%s, %s);", name, rec, text), pos); break;
        case GUI_GROUPBOX: TextAppend(toolstr, TextFormat("GuiGroupBox(%s, %s);", rec, text), pos); break;
        case GUI_LINE: TextAppend(toolstr, TextFormat("GuiLine(%s, %s);", rec, text), pos); break;
        case GUI_PANEL: TextAppend(toolstr, TextFormat("GuiPanel(%s, %s);", rec, text), pos); break;
        case GUI_LABEL: TextAppend(toolstr, TextFormat("GuiLabel(%s, %s);", rec, text), pos); break;
        case GUI_BUTTON:
            if(!config.exportButtonFunctions) TextAppend(toolstr, TextFormat("$%sPressed = GuiButton(%s, %s); ", name, rec, text), pos);
            else TextAppend(toolstr, TextFormat("if (GuiButton(%s, %s)) %s(); ", rec, text, TextToPascal(control.name)), pos);
            break;
        case GUI_LABELBUTTON:
            if(!config.exportButtonFunctions) TextAppend(toolstr, TextFormat("$%sPressed = GuiLabelButton(%s, %s);", name, rec, text), pos);
            else TextAppend(toolstr, TextFormat("if (GuiLabelButton(%s, %s)) %s();", rec, text, TextToPascal(control.name)), pos);
            break;
        case GUI_CHECKBOX: TextAppend(toolstr, TextFormat("GuiCheckBox(%s, %s, $%sChecked);", rec, text, name), pos); break;
        case GUI_TOGGLE: TextAppend(toolstr, TextFormat("GuiToggle(%s, %s, $%sActive);", rec, text, name), pos); break;
        case GUI_TOGGLEGROUP:TextAppend(toolstr, TextFormat("GuiToggleGroup(%s, %s, $%sActive);", rec, text, name), pos); break;
        case GUI_COMBOBOX: TextAppend(toolstr, TextFormat("GuiComboBox(%s, %s, $%sActive);", rec, text, name), pos); break;
        case GUI_DROPDOWNBOX: TextAppend(toolstr, TextFormat("if (GuiDropdownBox(%s, %s, $%sActive, $%sEditMode)) $%sEditMode = !$%sEditMode;", rec, text, name, name, name, name), pos); break;
        case GUI_TEXTBOX: TextAppend(toolstr, TextFormat("if (GuiTextBox(%s, $%sText, $%sFilledLen, $%sEditMode)) $%sEditMode = !$%sEditMode;", rec, name, name, name, name, name), pos); break;
        case GUI_TEXTBOXMULTI: TextAppend(toolstr, TextFormat("if (GuiTextBoxMulti(%s, $%sText, $%sFilledLen, $%sEditMode)) $%sEditMode = !$%sEditMode;", rec, name, name, name, name, name), pos); break;
        case GUI_VALUEBOX: TextAppend(toolstr, TextFormat("if (GuiValueBox(%s, %s, $%sValue, 0, 100, $%sEditMode)) $%sEditMode = !$%sEditMode;", rec, text, name, name, name, name), pos); break;
        case GUI_SPINNER: TextAppend(toolstr, TextFormat("if (GuiSpinner(%s, %s, $%sValue, 0, 100, $%sEditMode)) $%sEditMode = !$%sEditMode;", rec, text, name, name, name, name), pos); break;
        case GUI_SLIDER: TextAppend(toolstr, TextFormat("GuiSlider(%s, %s, NULL, $%sValue, 0, 100);", rec, text, name), pos); break;
        case GUI_SLIDERBAR: TextAppend(toolstr, TextFormat("GuiSliderBar(%s, %s, NULL, $%sValue, 0, 100);", rec, text, name), pos); break;
        case GUI_PROGRESSBAR: TextAppend(toolstr, TextFormat("GuiProgressBar(%s, %s, NULL, $%sValue, 0, 1);", rec, text, name), pos); break;
        case GUI_STATUSBAR: TextAppend(toolstr, TextFormat("GuiStatusBar(%s, %s);", rec, text), pos); break;
        case GUI_SCROLLPANEL:
        {
            char *containerRec = GetScrollPanelContainerRecText(index, control, config.defineRecs, config.exportAnchors);
            TextAppend(toolstr, TextFormat("GuiScrollPanel(%s, %s, %s, $%sScrollOffset, $%sScrollView);", containerRec, text, rec, name, name), pos); break;
        }
        case GUI_LISTVIEW: TextAppend(toolstr, TextFormat("GuiListView(%s, %s, $%sScrollIndex, $%sActive);", rec, (text == NULL)? "null" : text, name, name), pos); break;
        case GUI_COLORPICKER: TextAppend(toolstr, TextFormat("GuiColorPicker(%s, $%sValue);", rec, name), pos); break;
        case GUI_DUMMYREC: TextAppend(toolstr, TextFormat("GuiDummyRec(%s, %s);", rec, text), pos); break;
        default: break;
    }
}

// Get controls rectangle text
static char *GetControlRectangleText(int index, GuiLayoutControl control, bool defineRecs, bool exportAnchors)
{
    static char text[512];
    memset(text, 0, 512);

    if (defineRecs)
    {
        strcpy(text, TextFormat("$layoutRecs[%i]", index));
    }
    else
    {
        if (exportAnchors && control.ap->id > 0)
        {
            strcpy(text, TextFormat("array($%s[0] + %i, $%s[1] + %i, %i, %i)", control.ap->name, (int)control.rec.x, control.ap->name, (int)control.rec.y, (int)control.rec.width, (int)control.rec.height));
        }
        else
        {
            if (control.ap->id > 0) strcpy(text, TextFormat("array(%i, %i, %i, %i)", (int)control.rec.x + control.ap->x, (int)control.rec.y + control.ap->y, (int)control.rec.width, (int)control.rec.height));
            else strcpy(text, TextFormat("array(%i, %i, %i, %i)", (int)control.rec.x - control.ap->x, (int)control.rec.y - control.ap->y, (int)control.rec.width, (int)control.rec.height));
        }
    }

    return text;
}

// Get scroll panel container rectangle text
static char *GetScrollPanelContainerRecText(int index, GuiLayoutControl control, bool defineRecs, bool exportAnchors)
{
    static char text[512];
    memset(text, 0, 512);

    if(defineRecs)
    {
        strcpy(text, TextFormat("array($layoutRecs[%i][0], $layoutRecs[%i][1], $layoutRecs[%i][2] - $%sBoundsOffset[0], $layoutRecs[%i][3] - $%sBoundsOffset[1])", index, index, index, control.name, index, control.name));
    }
    else
    {
        if (exportAnchors && control.ap->id > 0)
        {
            strcpy(text, TextFormat("array($%s[0] + %i, $%s[1] + %i, %i - $%sBoundsOffset[0], %i - $%sBoundsOffset[1])", control.ap->name, (int)control.rec.x, control.ap->name, (int)control.rec.y, (int)control.rec.width, control.name, (int)control.rec.height, control.name));
        }
        else
        {
            // DOING
            if (control.ap->id > 0) strcpy(text, TextFormat("array(%i, %i, %i - $%sBoundsOffset[0], %i - $%sBoundsOffset[1])", (int)control.rec.x + control.ap->x, (int)control.rec.y + control.ap->y, (int)control.rec.width, control.name, (int)control.rec.height, control.name));
            else strcpy(text, TextFormat("array(%i, %i, %i - $%sBoundsOffset[0], %i - $%sBoundsOffset[1])", (int)control.rec.x - control.ap->x, (int)control.rec.y - control.ap->y, (int)control.rec.width, control.name, (int)control.rec.height, control.name));
        }
    }

    return text;
}

// Get controls parameters text
static char *GetControlTextParam(GuiLayoutControl control)
{
    static char text[512];
    memset(text, 0, 512);

    // NOTE: control.text will never be NULL
    if (control.text[0] == '\0') strcpy(text, "NULL");
    else strcpy(text, TextFormat("\"%s\"", control.text));

    return text;
}

#endif // CODEGEN_IMPLEMENTATION
