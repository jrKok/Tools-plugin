#ifndef SHOW_FPS_TEST_H
/* future developments :
 * resize display
 * be able to select among multiple text files in one directory
 * autotune NAV1,NAV2,ADF1,ADF2 and COM1/COM2 automatically from frequencies on a line
 * */

#include "show_fps_test_global.h"
#include "XPLMDisplay.h"    // for window creation and manipulation
#include "XPLMGraphics.h"   // for window drawing
#include "XPLMDataAccess.h" // for the VR dataref
#include "XPLMPlugin.h"     // for XPLM_MSG_SCENERY_LOADED message
#include "XPLMUtilities.h"  // to be able to write in log.txt
#include "string.h"
#include <string>
#include <layout.h>

#if IBM
    #include <windows.h>
#endif
#if LIN
    #include <GL/gl.h>
#elif __GNUC__
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

#ifndef XPLM301
    #error This is made to be compiled against the XPLM301 SDK
#endif

#endif // SHOW_FPS_TEST_H
