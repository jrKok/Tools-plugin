#include "show_fps_test.h"
#include "stringops.h"

/* Variables */

static XPLMWindowID	g_window;
int counter;
float currentFPS, currentmeasure;
bool commandBlock=false,filterEnabled=false;
std::vector<int> state;
std::vector<XPLMCommandRef> Commands;
std::vector<void*> refCons;
XPLMDataRef g_vr_dref,g_FPS;
static bool g_in_vr = false;
static float g_vr_button_lbrt[4]; // left, bottom, right, top
XPLMCommandRef CommandFPS = NULL,CommandFilter=NULL;

/*Forward declarations */

void				draw(XPLMWindowID in_window_id, void * in_refcon);
int					handle_mouse(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon);

int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }
int         MyFPSCommandHandler(XPLMCommandRef        inCommand, //Custom command handler for FPS window
                                XPLMCommandPhase      inPhase,
                                 void *               inRefcon);
int      MyFilterCommandHandler(XPLMCommandRef        inCommand, //This one receives the custom command
                                XPLMCommandPhase      inPhase,
                                 void *               inRefcon);
int MyBlockFilterCommandHandler(XPLMCommandRef        inCommand,//This one intercepts the commands of the registered stick commands
                                XPLMCommandPhase      inPhase,
                                 void *               inRefcon);
bool SetupFiltering();
void UndoFiltering();

/*End Forward Declarations*/

/* Helper Function */

static int	coord_in_rect(float x, float y, float * bounds_lbrt)  { return ((x >= bounds_lbrt[0]) && (x < bounds_lbrt[2]) && (y < bounds_lbrt[3]) && (y >= bounds_lbrt[1])); }

/*End HelperFunction*/

/* Start Implementations */


PLUGIN_API int XPluginStart(
                        char *		outName,
                        char *		outSig,
                        char *		outDesc)
{
    strcpy(outName, "FPS show in VR and Filter for VRStick commands");
    strcpy(outSig, "a plug-in by jrKok");
    strcpy(outDesc, "A plug-in that shows FPS in a small floating window in VR.");
    XPLMDebugString("Begin VR_Tools Plugin");

    currentFPS=0.0; counter=0;currentmeasure=0.0;
    g_vr_dref = XPLMFindDataRef("sim/graphics/VR/enabled");
    g_FPS = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
    CommandFPS = XPLMCreateCommand("VR/Custom/ShowFPS", "Show FPS");
    CommandFilter = XPLMCreateCommand("VR/Custom/Filter_Stick_Commands","Filter Stick");
    XPLMRegisterCommandHandler(CommandFPS,              // in Command name
                                 MyFPSCommandHandler,       // in Handler
                                 1,                      // Receive input before plugin windows.
                                 (void *) 0);
    XPLMRegisterCommandHandler(CommandFilter,MyFilterCommandHandler,1,(void *)0);

    return g_vr_dref != NULL;
    counter=0;

}

PLUGIN_API void	XPluginStop(void)
{//if the window hasn't been destroyed, we do it now
    XPLMDebugString("VR_Tool Plugin : am going to undo all filters\r\n");
    UndoFiltering();//I think I have to do this to delete the refCon pointers and avoid memory leaks
    XPLMDebugString("VR_Tool Plugin : am going to close open windows");
    if (g_window != NULL){
        XPLMDebugString("Unloading VR window\r\n");
        XPLMSetWindowPositioningMode(g_window,xplm_WindowPositionFree,0);
        XPLMDestroyWindow(g_window);
        g_window = NULL;}
}

PLUGIN_API void XPluginDisable(void) {UndoFiltering(); }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int msg, void * inParam){}

void	draw(XPLMWindowID in_window_id, void * in_refcon)
{
    XPLMSetGraphicsState(
            0 /* no fog */,
            0 /* 0 texture units */,
            0 /* no lighting */,
            0 /* no alpha testing */,
            1 /* do alpha blend */,
            1 /* do depth testing */,
            0 /* no depth writing */
    );

    int l, t, r, b;
    XPLMGetWindowGeometry(g_window, &l, &t, &r, &b);

    float cyan[] = {0.0, 1.0, 1.0};
    float gray[] = {0.378, 0.378, 0.370, 1.0};
            glColor4fv(gray);
            glBegin(GL_POLYGON);
            {
                glVertex2i(l+1, t-1);
                glVertex2i(r-1, t-1);
                glVertex2i(r-1, b+1);
                glVertex2i(l+1, b+1);
            }
            glEnd();

        // Draw the FPS
        std::string chFPS=std::to_string(currentFPS); //that's why I prefer C++ strings !
        chFPS="FPS: "+chFPS.substr(0,4);

        XPLMDrawString(cyan, l+10, t-16, (char *)chFPS.c_str(), NULL, xplmFont_Proportional);

}

int	handle_mouse(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon)
{
    return 1; //mouse events handled by caller
}

/* the very simple FPS displayer command
 * builds a VR window or even a regular small window if vr is not enabled
 * which will be triggered by a button choosen by the user
 * makes a mean of 15 values measured. The values get displayed when draw is called, which happens often enough - I hope
 */

int MyFPSCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    switch (inPhase)
    {
    case 0 : //Start Command

        if (g_window==NULL){//I prefer to recreate the window to keep a clean cockpit
            const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
            g_in_vr = vr_is_enabled;
            int global_desktop_bounds[4];
            XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

            XPLMCreateWindow_t params;
             params.structSize = sizeof(params);
             params.visible = 1;
             params.drawWindowFunc = draw;
             params.handleMouseClickFunc = handle_mouse;
             params.handleRightClickFunc = dummy_mouse_handler;
             params.handleMouseWheelFunc = dummy_wheel_handler;
             params.handleKeyFunc = dummy_key_handler;
             params.handleCursorFunc = dummy_cursor_status_handler;
             params.refcon = NULL;
             params.layer = xplm_WindowLayerFloatingWindows;
             params.decorateAsFloatingWindow = 2;
             params.left = global_desktop_bounds[0] + 20;
             params.bottom = global_desktop_bounds[1] + 50;
             params.right = global_desktop_bounds[0] + 160;
             params.top = global_desktop_bounds[1] + 100;

            g_window = XPLMCreateWindowEx(&params);
            XPLMSetWindowPositioningMode(g_window, g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
            XPLMSetWindowResizingLimits(g_window, 160, 20, 160, 20); // Limit resizing our window
            XPLMSetWindowTitle(g_window, "FPS");}
        else XPLMSetWindowIsVisible(g_window,1);

        break;
    case 1 : //Continue Command
    {float value;value=1/(XPLMGetDataf(g_FPS)); //instead of showing an instant of FPS, I will compute a mean of 15 values
        if (counter==0){ //begin counting
            counter=1;
            currentmeasure=value;
            currentFPS=value; //of course show first FPS immediatly, currentFPS is the value displayed
        }else{
            currentmeasure+=value; //sum of measured values, don't alter currentFPS which will still be displayed
            if(counter<15){
                counter++;
            }else{
                currentFPS=currentmeasure/15; //mean and change the value displayed
                currentmeasure=value; //reinitialize counter and buffer for sum
                counter=1;
            }
          }}
        break;
    case 2 : //End Command
        XPLMSetWindowIsVisible(g_window,0);//I'm going to remove the small Window to unclutter cockpit, otherwise handlers get displayed
        XPLMSetWindowPositioningMode(g_window,xplm_WindowPositionFree,0);
        XPLMDestroyWindow(g_window);
        g_window=NULL;
        counter=0;
        break;
    }
    return 0; //this one I've handled
}

/* NEXT TOOL : Filter VR stick commands with a button
 * button is choosen by user, best on a controller but not necessary
 * the button links to CommandFilter which sets up switches which are
 * used by the command blocker*/


bool SetupFiltering(){
    /* Reads output/settings/joystick preferences
     * Finds commands related to axis/commands
     * Builds a vector of these commands
     * assigns MyBlockFilterCommand to each of these commands
     * Builds a vector of states for each command (which will block or pass the command)
     */
    XPLMDebugString("VR_Tools plugin : setup Filtering commands");
    int nbcmd=0;
    std::ifstream jSettings("Output\\preferences\\X-Plane Joystick Settings.prf", std::ifstream::in);
    std::string lineRead;
    stringOps sto;
    while (getline(jSettings,lineRead)){
        if (sto.contains(lineRead,"AXIS_cmnd")){
            std::string eStr=sto.splitRight(lineRead," ");
            eStr=sto.cleanOut(eStr,"\r");
            eStr=sto.cleanOut(eStr,"\n");
            if (eStr!=""){
               XPLMCommandRef cRef=XPLMFindCommand((char*)eStr.c_str());
               if (cRef!=NULL){
                void * nb=new(int*);
                (*(int*)nb)=nbcmd;
                XPLMRegisterCommandHandler(cRef,MyBlockFilterCommandHandler,1,nb);
                Commands.push_back(cRef);
                state.push_back(0);
                refCons.push_back(nb);
                //Will delete the pointers later
               nbcmd++;}
            }
     jSettings.close();
       }
    }

    if (nbcmd>0) return true; else return false;
}

void UndoFiltering(){
    //will unassign MyBlockCommandHandler for each command in the vector
    std::size_t lg=Commands.size();
    if (lg>0){
    for (int I(0);I<lg;I++){
        XPLMUnregisterCommandHandler(Commands[I],MyBlockFilterCommandHandler,1,refCons[I]);
        delete refCons[I];//I hope that is enough to avoid memory leaks
    }
    Commands.clear();
    state.clear();
    refCons.clear();
    filterEnabled=false;
    commandBlock=false;}

}

int MyFilterCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){//activated when a button for custom command for filtering is pressed
    switch (inPhase)
    {
    case 0:
       {const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
        g_in_vr = vr_is_enabled;
        if (filterEnabled & g_in_vr)//I only can block commands if in VR and if CustomCommandHandlers are set
            commandBlock=false; //commands are processed
        else {if (g_in_vr)
                   filterEnabled=SetupFiltering();
              else filterEnabled=false;}
              }//If the filter button has never been pressed, commands are processed normally. I only begin filtering when button is pressed first
        break;
    case 1: //do nothing
        break;
    case 2:
        if (filterEnabled)
             commandBlock=true; //selected commands are blocked since filter button is released
        else commandBlock=false;
        break;
    }
    return 0;
}

int MyBlockFilterCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    switch (inPhase)
    {
    case 0: //at the start of the command I will evaluate if I have to block the concerned command
       { if (commandBlock){
            int refVal=*((int*)inRefcon);
            state[refVal]=0;//block command here
            return 0;}
        else{
            int refVal=*((int*)inRefcon);
            state[refVal]=1;//its up to X Plane or others to process command
            return 1;
        }}
        break;
    case 1:
     {int refVal=*((int*)inRefcon);
      return (state[refVal]);}
     break;
    case 2:
     {int refVal=*((int*)inRefcon);
      return (state[refVal]);}
     break;
    }
    return 1;
}
