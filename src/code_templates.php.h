/*******************************************************************************************
*
*   rGuiLayout - Layout Code Generator Templates
*
*   DESCRIPTION:
*       Templates used to generate layout code files (.c/.h) replacing some variables
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

// Standard C file template
static const unsigned char *guiTemplateStandardCode = "<?php\n\
/*******************************************************************************************\n\
*\n\
*   $(GUILAYOUT_NAME_PASCALCASE) v$(GUILAYOUT_VERSION) - $(GUILAYOUT_DESCRIPTION)\n\
*\n\
*   LICENSE: Propietary License\n\
*\n\
*   Copyright (c) 2022 $(GUILAYOUT_COMPANY). All Rights Reserved.\n\
*\n\
*   Unauthorized copying of this file, via any medium is strictly prohibited\n\
*   This project is proprietary and confidential unless the owner allows\n\
*   usage in any other form by expresely written permission.\n\
*\n\
**********************************************************************************************/\n\
\n\
// Initialization\n\
//---------------------------------------------------------------------------------------\n\
$screenWidth  = $(GUILAYOUT_WINDOW_WIDTH);\n\
$screenHeight = $(GUILAYOUT_WINDOW_HEIGHT);\n\
\n\
InitWindow($screenWidth, $screenHeight, \"$(GUILAYOUT_NAME)\");\n\
\n\
// $(GUILAYOUT_NAME): controls initialization\n\
//----------------------------------------------------------------------------------\n\
$(GUILAYOUT_INITIALIZATION_C)\n\
//----------------------------------------------------------------------------------\n\
\n\
SetTargetFPS(60);\n\
//--------------------------------------------------------------------------------------\n\
\n\
// Main loop\n\
while (!WindowShouldClose())    // Detect window close button or ESC key\n\
{\n\
    // Update\n\
    //----------------------------------------------------------------------------------\n\
    // TODO: Implement required update logic\n\
    //----------------------------------------------------------------------------------\n\
\n\
    // Draw\n\
    //----------------------------------------------------------------------------------\n\
    BeginDrawing();\n\
\n\
        ClearBackground(GetColor(GuiGetStyle(GUI_CONTROL_DEFAULT, BACKGROUND_COLOR))); \n\
\n\
        // raygui: controls drawing\n\
        //----------------------------------------------------------------------------------\n\
        $(GUILAYOUT_DRAWING_C)\n\
        //----------------------------------------------------------------------------------\n\
\n\
    EndDrawing();\n\
    //----------------------------------------------------------------------------------\n\
}\n\
\n\
// De-Initialization\n\
//--------------------------------------------------------------------------------------\n\
CloseWindow();        // Close window and OpenGL context\n\
//--------------------------------------------------------------------------------------\n\
\n\
//------------------------------------------------------------------------------------\n\
// Controls Functions Definitions (local)\n\
//------------------------------------------------------------------------------------\n\
$(GUILAYOUT_FUNCTION_DEFINITION_C)\n\
";

// Header-only code file template
static const unsigned char *guiTemplateHeaderOnly = "";
