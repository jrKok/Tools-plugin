#include "show_fps_test.h"
#include "stringops.h"
#include "textreader.h"


/* Variables */

static XPLMWindowID	g_window, g_textWindow;
int counter,pageHeight(60);
float currentFPS, currentmeasure;
bool commandBlock=false,filterEnabled=false;
Layout wLayout;

std::vector<int> state;
std::vector<XPLMCommandRef> Commands;
std::vector<void*> refCons;
XPLMDataRef g_vr_dref,g_FPS,dref_SunPitch;
XPLMDataRef nav1on,nav2on,com1on,com2on,adf1on,adf2on;
XPLMDataRef nav1freq,nav2freq,com1freq,com2freq,adf1freq,adf2freq,com1freqk,com2freqk;

static bool g_in_vr = false;
static float g_vr_button_lbrt[4]; // left, bottom, right, top
XPLMCommandRef CommandFPS = NULL,CommandFilter=NULL,CommandText=NULL;

float cyan[] = {0.0f, 1.0f, 1.0f};
float green[]={0.0f,0.95f,0.0f};
float gray[] = {0.375f, 0.375f, 0.368f};
float paperWhitesomber[] = {0.9529f,0.7686f,0.4706f};
//float red[]= {0.90f,0.0f,0.0f};
float darkgray[]={0.009f,0.009f,0.009f};
float blackpaper[]={0.015f,0.015f,0.015f};
float colorPaper[]={0.9922f,0.9608f,0.9098f}; //Duron paper white
float colorInk[]={0.005f,0.005f,0.005f};
float amberInk[]={1.0f,0.65f,0.0f};

/*Forward declarations */

void				draw(XPLMWindowID in_window_id, void * in_refcon);
void                drawText(XPLMWindowID in_window_id, void * in_refcon);
int					handle_mouse(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon);
int                 handle_mouse_for_TextW (XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon);

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
int  MyTextReaderCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);
bool SetupFiltering();
void UndoFiltering();
void drawRectnText(float cRect[], float cInk[], float cSelect[]);

/*End Forward Declarations*/

/* Helper Function */

static int	coord_in_rect(float x, float y, float * bounds_lbrt)  { return ((x >= bounds_lbrt[0]) && (x < bounds_lbrt[2]) && (y < bounds_lbrt[3]) && (y >= bounds_lbrt[1])); }





void writeDebug(std::string in_String){
 in_String="VR Tools : " +in_String+"\r\n";
 XPLMDebugString((char*)in_String.c_str());
}

/*End HelperFunction*/

/* Start Implementations */

PLUGIN_API int XPluginStart(
                        char *		outName,
                        char *		outSig,
                        char *		outDesc)
{

    strcpy(outName, "FPS show in VR, Filter for VRStick commands, displays a text file");
    strcpy(outSig, "a plug-in by jrKok");
    strcpy(outDesc, "A plug-in that shows FPS in a small floating window in VR and toggles a text file");
    XPLMDebugString("Begin VR_Tools Plugin");

    currentFPS=0.0f;
    counter=0;
    currentmeasure=0.0f;

    g_vr_dref    = XPLMFindDataRef("sim/graphics/VR/enabled");
    g_FPS        = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
    dref_SunPitch= XPLMFindDataRef("sim/graphics/scenery/sun_pitch_degrees");//float to estimate pos of sun
    nav1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_power");
    nav2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_power");
    com1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_power");
    com2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_power");
    adf1on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_power"); //returns an int 0 for Off 2 for ON
    adf2on       = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_power");
    nav1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_standby_frequency_hz");
    nav2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_standby_frequency_hz");
    com1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_hz");
    com2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_hz");
    com1freqk    = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_standby_frequency_khz");
    com2freqk    = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_standby_frequency_khz");
    adf1freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_standby_frequency_hz");
    adf2freq     = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf2_standby_frequency_hz");

    CommandFPS    = XPLMCreateCommand("VR/Custom/ShowFPS", "Show FPS");
    CommandFilter = XPLMCreateCommand("VR/Custom/Filter_Stick_Commands","Filter Stick");
    CommandText   = XPLMCreateCommand("VR/Custom/Toggle_Text_File","Toggle text");

    XPLMRegisterCommandHandler(CommandFPS,              // in Command name
                                MyFPSCommandHandler,       // in Handler
                                1,                      // Receive input before plugin windows.
                                (void *) 0);
    XPLMRegisterCommandHandler(CommandFilter,MyFilterCommandHandler,1,(void *)0); //to apply to the button handling the filtering like a "ctrl" key
    XPLMRegisterCommandHandler(CommandText,MyTextReaderCommandHandler,1,(void *)0);//Command to toggle tchotchke displaying text file

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

    if (g_textWindow!=NULL){
        XPLMDestroyWindow(g_textWindow);
        g_textWindow=NULL;
    }
}

PLUGIN_API void XPluginDisable(void) {UndoFiltering(); }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int msg, void * inParam){}

void	draw(XPLMWindowID in_window_id, void * in_refcon) // draw FPS Window
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

            glColor3fv(gray);
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
    case xplm_CommandBegin :

        if (g_window==NULL){//I prefer to recreate the window to keep a clean cockpit
            const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
            g_in_vr = vr_is_enabled;
            int global_desktop_bounds[4];
            XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

            XPLMCreateWindow_t params;
             params.structSize = sizeof(params);
             params.visible = 1;
             params.drawWindowFunc = draw;
             params.handleMouseClickFunc = dummy_mouse_handler;
             params.handleRightClickFunc = dummy_mouse_handler;
             params.handleMouseWheelFunc = dummy_wheel_handler;
             params.handleKeyFunc = dummy_key_handler;
             params.handleCursorFunc = dummy_cursor_status_handler;
             params.refcon = NULL;
             params.layer = xplm_WindowLayerFloatingWindows;
             params.decorateAsFloatingWindow = xplm_WindowDecorationNone;
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
    case xplm_CommandContinue :
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
    case xplm_CommandEnd :
        XPLMSetWindowIsVisible(g_window,0);//I'm going to remove the small Window to unclutter cockpit, otherwise handlers get displayed
        //XPLMSetWindowPositioningMode(g_window,xplm_WindowPositionFree,0); useless now since 11.20b3, unless tests say otherwise
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
    XPLMDebugString("VR_Tools plugin : setup Filtering commands\r\n");
    int nbcmd=0;
    std::ifstream jSettings("Output\\preferences\\X-Plane Joystick Settings.prf", std::ifstream::in);
    std::string lineRead;
    stringOps sto;
    while (getline(jSettings,lineRead)){
        if (sto.contains(lineRead,"AXIS_cmnd")){
            std::string eStr=sto.splitRight(lineRead," ");
            eStr=sto.cleanOut(eStr,"\r");
            eStr=sto.cleanOut(eStr,"\n");

            if (eStr!=""){//give this command, if it exists, a custom handler
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
       }
    }
    jSettings.close();
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
    case xplm_CommandBegin:
       {const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
        g_in_vr = vr_is_enabled;
        if (filterEnabled & g_in_vr)//I only can block commands if in VR and if CustomCommandHandlers are set
            commandBlock=false; //commands are processed
        else {if (g_in_vr)
                   filterEnabled=SetupFiltering();
              else filterEnabled=false;}
              }//If the filter button has never been pressed, commands are processed normally. I only begin filtering when button is pressed first
        break;
    case xplm_CommandContinue: //do nothing
        break;
    case xplm_CommandEnd:
        if (filterEnabled)
             commandBlock=true; //selected commands are blocked since filter button is released
        else commandBlock=false;
        break;
    }
    return 0;//in every case its my command and i've handled it
}

int MyBlockFilterCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){//Handels built-in commands selected by the filtering function
    switch (inPhase)
    {
    case xplm_CommandBegin: //at the start of the command I will evaluate if I have to block the concerned command
       { if (commandBlock){
            int refVal=*((int*)inRefcon);
            state[refVal]=0;
            return 0;}//block command here
        else{
            int refVal=*((int*)inRefcon);
            state[refVal]=1;
            return 1;//its up to X Plane or others to process command
        }}
        break;
    case xplm_CommandContinue:
     {int refVal=*((int*)inRefcon);
      return (state[refVal]);}//we've thought out before if we block or pass on the command
     break;
    case xplm_CommandEnd:
     {int refVal=*((int*)inRefcon);
      return (state[refVal]);} //same as before ...
     break;
    }
    return 1;//if nothing else go and do what you need X Plane !
}

/* And here the code for the text reader which is a custom command and a drawing function which uses
 * the textreader class to tailor the text file and iterate among the lines
 and a custom mouse handler*/

void drawText(XPLMWindowID in_window_id, void * in_refcon){
//intialize
    XPLMSetGraphicsState(
            0 /* no fog */,
            0 /* 0 texture units */,
            0 /* no lighting */,
            0 /* no alpha testing */,
            1 /* do alpha blend */,
            1 /* do depth testing */,
            0 /* no depth writing */
    );

    XPLMGetWindowGeometry(g_textWindow, &wLayout.l, &wLayout.t, &wLayout.r, &wLayout.b);
    wLayout.recalculate();



    float sunPtc = XPLMGetDataf(dref_SunPitch);

//draw background
            glColor3fv(darkgray);
            glBegin(GL_POLYGON);
            {
                glVertex2i(wLayout.generalR.left, wLayout.generalR.top);
                glVertex2i(wLayout.generalR.right, wLayout.generalR.top);
                glVertex2i(wLayout.generalR.right, wLayout.generalR.bottom);
                glVertex2i(wLayout.generalR.left, wLayout.generalR.bottom);
            }
            glEnd();

//draw commands on left column
            for (int I(0);I<wLayout.nButtons;I++){
                if (wLayout.tBoxes[I].isVisible){
                  glColor3fv(green);
                  glBegin(GL_LINE_LOOP);
                  {
                      glVertex2i(wLayout.tBoxes[I].left, wLayout.tBoxes[I].top);
                      glVertex2i(wLayout.tBoxes[I].right, wLayout.tBoxes[I].top);
                      glVertex2i(wLayout.tBoxes[I].right, wLayout.tBoxes[I].bottom);
                      glVertex2i(wLayout.tBoxes[I].left, wLayout.tBoxes[I].bottom);
                  }
                glEnd();
                XPLMDrawString(colorPaper,wLayout.tBoxes[I].posTextX,wLayout.tBoxes[I].posTextY,(char*)wLayout.tBoxes[I].buttonText.c_str(),NULL,xplmFont_Proportional);
                }
            }
//draw name of text file
            XPLMDrawString(cyan,wLayout.TitleLine.x,wLayout.TitleLine.y,(char*)wLayout.TitleLine.textOfLine.c_str(),NULL,xplmFont_Proportional);//when moving to displaying a random text file, center this
//draw text rectangle and text in right side, different colors
            if (sunPtc>=5){//sun bright in sky
                drawRectnText(colorPaper,colorInk,cyan);
            }else
            {if ((sunPtc>-3)&(sunPtc<5)){//sun at sunset darker duron paper white
                    drawRectnText(paperWhitesomber,colorInk,cyan);
                }
             else drawRectnText(blackpaper,amberInk,cyan); // this at night
            }
}

void drawRectnText(float cRect[3],  float cInk[3], float cSelect[]){
    glColor3fv(cRect);
    glBegin(GL_POLYGON);
    {
        glVertex2i(wLayout.textR.left, wLayout.textR.top);
        glVertex2i(wLayout.textR.right, wLayout.textR.top);
        glVertex2i(wLayout.textR.right, wLayout.textR.bottom);
        glVertex2i(wLayout.textR.left, wLayout.textR.bottom);
    }
    glEnd();
for (int I(0);I<=pageHeight;I++){
    std::string readStr=wLayout.nextTextLine();
    if (readStr!="")
        if (wLayout.tFileReader->atSelectedLine())
            XPLMDrawString(cSelect,wLayout.leftTextLine(I),wLayout.bottomTextLine(I),(char*)readStr.c_str(),NULL,xplmFont_Proportional);
           else XPLMDrawString(cInk,wLayout.leftTextLine(I),wLayout.bottomTextLine(I),(char*)readStr.c_str(),NULL,xplmFont_Proportional);
    }
}

int	handle_mouse_for_TextW (XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus mouse_status, void * in_refcon)
{
    if(mouse_status == xplm_MouseDown){
        char LB;
        int clickresult=wLayout.findClick(x,y,LB);
        if (clickresult>=0) {
        if (LB=='L'){
            wLayout.tFileReader->select(clickresult);

        }
        if (LB=='B'){
            switch(clickresult){
            case B_Page_Up:{
                wLayout.tFileReader->PreviousPage();
            }break;
            case B_Page_Down:{
                wLayout.tFileReader->NextPage();
            }break;
            case B_Line_Up:{
                wLayout.tFileReader->previousLine();
            }break;
            case B_Line_Down:{
                wLayout.tFileReader->nextLine();
            }break;
            case B_Toggle_Line:{
                wLayout.tFileReader->DeleteLine();
            }break;
            case B_Show_All:{
                wLayout.tFileReader->ShowAll();
            }break;
            case B_NAV1:{
                XPLMSetDatai(nav1freq,wLayout.tFileReader->freqNAV);
            }break;
            case B_NAV2:{
                XPLMSetDatai(nav2freq,wLayout.tFileReader->freqNAV);
            }break;
            case B_COM1:{
                int frac(0);
                float val=wLayout.tFileReader->freqCOM;
                XPLMSetDatai(com1freq,(int)val);
                val-=(float)XPLMGetDatai(com1freq);
                if (val!=0){
                    frac=XPLMGetDatai(com1freqk);
                    frac+=(int)(val*10.0);
                    XPLMSetDatai(com1freqk,frac);
                }

            }break;
            case B_COM2:{
                int frac(0);
                float val=wLayout.tFileReader->freqCOM;
                XPLMSetDatai(com2freq,(int)val);
                val-=(float)XPLMGetDatai(com2freq);
                if (val!=0){
                    frac=XPLMGetDatai(com2freqk);
                    frac+=(int)(val*10.0);
                    XPLMSetDatai(com2freqk,frac);
                }
            }break;
            case B_ADF1:{
                XPLMSetDatai(adf1freq,wLayout.tFileReader->freqADF);
            }break;
            case B_ADF2:{
                XPLMSetDatai(adf2freq,wLayout.tFileReader->freqADF);
            }break;
            }//end of switch

        }
        wLayout.CheckButtonsVisibility();
        //Check for Nav Buttons, show only if device is ON
        int on=XPLMGetDatai(nav1on); if (on==0) wLayout.tBoxes[B_NAV1].isVisible=false;
        on=XPLMGetDatai(nav2on); if (on==0) wLayout.tBoxes[B_NAV2].isVisible=false;
        on=XPLMGetDatai(com1on); if (on==0) wLayout.tBoxes[B_COM1].isVisible=false;
        on=XPLMGetDatai(com2on); if (on==0) wLayout.tBoxes[B_COM2].isVisible=false;
        on=XPLMGetDatai(adf1on); if (on==0) wLayout.tBoxes[B_ADF1].isVisible=false;
        on=XPLMGetDatai(adf2on); if (on==0) wLayout.tBoxes[B_ADF2].isVisible=false;
        return (0);
    }

}return 1;//mouse events handled by caller
}

int  MyTextReaderCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon){
    switch (inPhase)
    {
    case xplm_CommandBegin : //opens or closes the textreader, creates or destroys the window
    {
        if (g_textWindow==NULL){//Toggle window in and out of existence
            const int vr_is_enabled = XPLMGetDatai(g_vr_dref);
            g_in_vr = vr_is_enabled;
            XPLMGetScreenBoundsGlobal(&wLayout.gLeft, &wLayout.gTop, &wLayout.gRight, &wLayout.gBottom);
            if (wLayout.initiate()){
            pageHeight=wLayout.maxLines;
            XPLMCreateWindow_t params;
             params.structSize = sizeof(params);
             params.visible = 1;
             params.drawWindowFunc = drawText;
             params.handleMouseClickFunc = handle_mouse_for_TextW;
             params.handleRightClickFunc = dummy_mouse_handler;
             params.handleMouseWheelFunc = dummy_wheel_handler;
             params.handleKeyFunc = dummy_key_handler;
             params.handleCursorFunc = dummy_cursor_status_handler;
             params.refcon = NULL;
             params.layer = xplm_WindowLayerFloatingWindows;
             params.decorateAsFloatingWindow = xplm_WindowDecorationRoundRectangle;
             params.left = wLayout.wLeft;
             params.bottom = wLayout.wBottom-1;
             params.right = wLayout.wRight+2;
             params.top = wLayout.wTop+1;

            g_textWindow = XPLMCreateWindowEx(&params);
            XPLMSetWindowPositioningMode(g_textWindow, g_in_vr?xplm_WindowVR:xplm_WindowPositionFree, -1);
            XPLMSetWindowResizingLimits(g_textWindow, wLayout.generalR.width+2, wLayout.generalR.width+20, wLayout.generalR.height+2, wLayout.generalR.height+10); // Limit resizing our window
            XPLMSetWindowTitle(g_window, "FLIGHT NOTES");
            wLayout.CheckButtonsVisibility();
            writeDebug("text window initialised ");}
            else writeDebug(("couldn't initiate the textfile, file not found"));
        }
        else
        {   if (XPLMGetWindowIsVisible(g_textWindow)==0)
               {XPLMSetWindowIsVisible(g_textWindow,1);
                wLayout.CheckButtonsVisibility();}
            else{
                    XPLMDestroyWindow(g_textWindow);
                    wLayout.ClosegWindow();
                    g_textWindow=NULL;}
        }
    }
        break;
    case xplm_CommandContinue :
        break;
    case xplm_CommandEnd :
        break;
        return 0;
    }
    return 0;//no one else needs to handle this
}
