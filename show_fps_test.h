#ifndef SHOW_FPS_TEST_H

#include "XPLMDisplay.h"   // for window creation and manipulation
#include "XPLMGraphics.h"  // for window drawing
#include "XPLMDataAccess.h" // for the VR dataref
#include "XPLMPlugin.h"     // for XPLM_MSG_SCENERY_LOADED message
#include "XPLMUtilities.h" // to be able to write in log.txt
#include "string.h"
#include <string>
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
