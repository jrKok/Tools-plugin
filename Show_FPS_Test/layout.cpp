#include <layout.h>
#include <XPLMGraphics.h>

Layout::Layout() :
gTop(0),gBottom(0),gLeft(0),gRight(0),//Used for initial calculations and coordinates,global coordinates
wTop(0),wBottom(0),wLeft(0),wRight(0),wWidth(0),wHeight(0),charHeight(0),charWidth(0),maxLines(MaxLines),maxChars(MaxCharsPerLine),textPointX(0),textPointY(0),//Used for initial setup and resize instructions
t(0),b(0),l(0),r(0),//input from draw instruction mostly
textHeight(0),colWidth(74),idxSelected(-1),nButtons(0),
tFileReader(new TextReader("Output\\textfiles\\flightnotes.txt"))
{

}

Layout::~Layout(){
    delete tFileReader;
}


void Layout::resize(){//calculate offsets; areas of rectangles}
    TitleLine.textOfLine="flightnotes";
    XPLMGetFontDimensions(xplmFont_Proportional,NULL,&charHeight,NULL);

    tFileReader->BeginRead();
    textR.width=50;
    while (!tFileReader->BuffEnd()){
    std::string testString=tFileReader->ReadOn();
    int strWidth=(int)XPLMMeasureString(xplmFont_Proportional,(char*)testString.c_str(),testString.size());
    if (strWidth>textR.width) textR.width=strWidth;
    }
    int titleWidth=(int)XPLMMeasureString(xplmFont_Proportional,(char*)TitleLine.textOfLine.c_str(),TitleLine.textOfLine.size());
    textR.width=textR.width+20;
    if (textR.width<240) textR.width=240;//10 left margin; 10 right margin
    textR.offsetX=colWidth;
    textR.height=charHeight*maxLines+2*maxLines+30;
    if (textR.height<200) textR.height=200;//10 top margin; 10 under margin 2pts spacer between lines
    textR.offsetY=charHeight+12;//place for the title; eventually in a box if necessary
    generalR.width=textR.width+colWidth+10;//10 bx right margin
    generalR.height=textR.height+textR.offsetY+35;//10 bx upper margin and lower margin
    wWidth=generalR.width;
    wHeight=generalR.height;
    generalR.offsetX=0;
    generalR.offsetY=0;
    TitleLine.height=charHeight;
    TitleLine.width=titleWidth;
    TitleLine.offSetX=colWidth-(int)(titleWidth/2)+(int)(textR.width/2);//center the Title
    TitleLine.offSetY=charHeight+4;
    //create the Lines to display the text
    for (int I(0);I<maxLines;I++) {
        TextLine nTL;
        nTL.height=charHeight;
        nTL.index=I;
        nTL.width=textR.width-20;
        nTL.offSetX=colWidth+10;
        nTL.offSetY=textR.offsetY+15+charHeight*I+I*2;
        tLines.push_back(nTL);
    }
    //create the commandRectangles; with text
    defineButtons();
    nButtons=tBoxes.size();

}
bool Layout::initiate(){
    tFileReader->SetPageHeight(maxLines);
    if (!tFileReader->ReadFileToBuff(maxChars)) return false;
    size_t nbLines=tFileReader->GetBuffSize();
    size_t nbChpLine=tFileReader->GetMaxLineLength();
    if (nbLines<maxLines) maxLines=(int)nbLines;
    if (nbChpLine<maxChars) maxChars=(int)nbChpLine;


    resize();
    wLeft=gLeft+100;
    wRight=wLeft+wWidth;
    wTop=gTop-50;
    wBottom=wTop-wHeight;
    return true;
}
void Layout::recalculate(){
    tFileReader->BeginPage();
    if ((t!=wTop)|(l!=wLeft)){
        for (int I(0);I<maxLines;I++) {
           tLines[I].in_left=l;
           tLines[I].in_top=t;
           tLines[I].recalculate();
        }
        TitleLine.in_left=l;
        TitleLine.in_top=t;
        TitleLine.recalculate();
        generalR.in_left=l;
        generalR.in_top=t;
        generalR.recalculate();
        textR.in_left=l;
        textR.in_top=t;
        textR.recalculate();
        for (int I(0);I<nButtons;I++){
            tBoxes[I].in_top=t;
            tBoxes[I].in_left=l;
            tBoxes[I].recalculate();
        }

}
        //recalculate rectangles for buttons
}
int Layout::rightRectangle(int idx){
    return (tBoxes[idx].right);
}
int Layout::leftRectangle(int idx){
    return (tBoxes[idx].left);
}
int Layout::topRectangle(int idx){
    return (tBoxes[idx].top);
}
int Layout::bottomRectangle(int idx){
    return (tBoxes[idx].bottom);
}
int Layout::leftTextLine(int idx){
    return (tLines[idx].x);
}
int Layout::bottomTextLine(int idx){
    return (tLines[idx].y);}
std::string Layout::nextTextLine(){
    return (tFileReader->ReadOnPage());
}
int Layout::findClick(int mx, int my,char &LB){
    if (textR.isHere(mx,my)){
        for (int I(0);I<maxLines;I++){
            if (tLines[I].isHere(mx,my)) {
                LB='L';
                return(I);
            }
        }
      }else{
        for (int I(0);I<nButtons;I++){
            if (tBoxes[I].isHere(mx,my)){
                LB='B';
                return(I);
            }
        }
    }
    return(-1);
}

void Layout::defineButtons(){
    int middle=textR.offsetY+(int)(textR.height/2);
  /*  B_Load_File=0*/
    bLoad_File.isVisible=true;
    bLoad_File.height=14;
    bLoad_File.width=20;
    bLoad_File.offsetX=10;
    bLoad_File.offsetY=20;
    bLoad_File.setText("New");
    tBoxes.push_back(bLoad_File);
         //B_Page_Up=1;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=20;
    bLoad_File.offsetX=20;
    bLoad_File.offsetY=middle-13-3-20-3-20;
    bLoad_File.setText("PgUp");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
         //B_Line_Up=2;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=20;
    bLoad_File.offsetX=20;
    bLoad_File.offsetY=middle-13-3-20;
    bLoad_File.setText("Up");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
         //B_Toggle_Line=3;
    bLoad_File.isVisible=true;
    bLoad_File.height=15;
    bLoad_File.width=20;
    bLoad_File.offsetX=37;
    bLoad_File.offsetY=middle-13;
    bLoad_File.setText("Del");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
         //B_Line_Down=4;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=20;
    bLoad_File.offsetX=20;
    bLoad_File.offsetY=middle+7;
    bLoad_File.setText("Dn");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
        // B_Page_Down=5;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=20;
    bLoad_File.offsetX=20;
    bLoad_File.offsetY=middle+7+20+3;
    bLoad_File.setText("PgDn");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
         //B_Longer_Lines=6;
    bLoad_File.isVisible=true;
    bLoad_File.height=15;
    bLoad_File.width=20;
    bLoad_File.offsetX=colWidth+5;
    bLoad_File.offsetY=generalR.height-31;
    bLoad_File.setText("wider");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
        // B_Smaller_Lines=7;
    bLoad_File.isVisible=true;
    bLoad_File.height=15;
    bLoad_File.width=20;
    bLoad_File.offsetX=colWidth+47;
    bLoad_File.offsetY=generalR.height-31;
    bLoad_File.setText("narrower");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
         //B_More_Lines=8;
    bLoad_File.isVisible=true;
    bLoad_File.height=15;
    bLoad_File.width=20;
    bLoad_File.offsetX=colWidth+142;
    bLoad_File.offsetY=generalR.height-31;
    bLoad_File.setText("+Lines");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
        // B_Less_Lines=9;
    bLoad_File.isVisible=true;
    bLoad_File.height=15;
    bLoad_File.width=20;
    bLoad_File.offsetX=colWidth+190;
    bLoad_File.offsetY=generalR.height-31;
    bLoad_File.setText("-Lines");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
         //B_NAV1 = 10;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=35;
    bLoad_File.offsetX=1;
    bLoad_File.offsetY=generalR.height-76;
    bLoad_File.setText("Nav1");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
        // B_NAV2 = 11;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=35;
    bLoad_File.offsetX=38;
    bLoad_File.offsetY=generalR.height-76;
    bLoad_File.setText("Nav2");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
         //B_COM1 = 12;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=35;
    bLoad_File.offsetX=1;
    bLoad_File.offsetY=generalR.height-55;
    bLoad_File.setText("com1");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
        // B_COM2 = 13;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=35;
    bLoad_File.offsetX=38;
    bLoad_File.offsetY=generalR.height-55;
    bLoad_File.setText("com2");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
        // B_ADF1 = 14;
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=35;
    bLoad_File.offsetX=1;
    bLoad_File.offsetY=generalR.height-33;
    bLoad_File.setText("Adf1");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
        // B_ADF2 = 15*/
    bLoad_File.isVisible=true;
    bLoad_File.height=20;
    bLoad_File.width=35;
    bLoad_File.offsetX=38;
    bLoad_File.offsetY=generalR.height-33;
    bLoad_File.setText("Adf2");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
    //B_Show_All=16
    bLoad_File.isVisible=true;
    bLoad_File.height=15;
    bLoad_File.width=20;
    bLoad_File.offsetX=10;
    bLoad_File.offsetY=middle-13;
    bLoad_File.setText("All");//\u2C4 bas\CB85
    tBoxes.push_back(bLoad_File);
}

void Layout::CheckButtonsVisibility(){
    tBoxes[B_Load_File].isVisible=false;
    tBoxes[B_Smaller_Lines].isVisible=false;
    tBoxes[B_Longer_Lines].isVisible=false;
    tBoxes[B_More_Lines].isVisible=false;
    tBoxes[B_Less_Lines].isVisible=false;
    if (tFileReader->FirstPage()) {tBoxes[B_Page_Up].isVisible=false;tBoxes[B_Line_Up].isVisible=false;}
                             else {tBoxes[B_Page_Up].isVisible=true;tBoxes[B_Line_Up].isVisible=true;}
    if (tFileReader->LastPage()) {tBoxes[B_Page_Down].isVisible=false;tBoxes[B_Line_Down].isVisible=false;}
                            else {tBoxes[B_Page_Down].isVisible=true;tBoxes[B_Line_Down].isVisible=true;}
    if (!tFileReader->IsSelectAtBegin()&tFileReader->hasSelectedLine()) tBoxes[B_Line_Up].isVisible=true;
    if (!tFileReader->IsSelectAtEnd()&tFileReader->hasSelectedLine())  tBoxes[B_Line_Down].isVisible=true;
    if (tFileReader->hasSelectedLine()) tBoxes[B_Toggle_Line].isVisible=true;
                                   else tBoxes[B_Toggle_Line].isVisible=false;
    if (tFileReader->HasHiddenLine())   tBoxes[B_Show_All].isVisible=true;
                                   else tBoxes[B_Show_All].isVisible=false;
    if (tFileReader->HasNav()) { tBoxes[B_NAV1].isVisible=true;
                                 tBoxes[B_NAV2].isVisible=true;}
                           else {tBoxes[B_NAV1].isVisible=false;
                                 tBoxes[B_NAV2].isVisible=false;}
    if (tFileReader->HasCom()) { tBoxes[B_COM1].isVisible=true;
                                 tBoxes[B_COM2].isVisible=true;}
                           else {tBoxes[B_COM1].isVisible=false;
                                 tBoxes[B_COM2].isVisible=false;}
    if (tFileReader->HasADF()) { tBoxes[B_ADF1].isVisible=true;
                                 tBoxes[B_ADF2].isVisible=true;}
                           else {tBoxes[B_ADF1].isVisible=false;
                                 tBoxes[B_ADF2].isVisible=false;}
}

void Layout::ClosegWindow(){
    tFileReader->closeReader();
    tBoxes.clear();
    tLines.clear();
    gTop=0;gBottom=0;gLeft=0;gRight=0;
    wTop=0;wBottom=0;wLeft=0;wRight=0;wWidth=0;wHeight=0;charHeight=0;charWidth=0;maxLines=MaxLines;maxChars=MaxCharsPerLine;textPointX=0;textPointY=0;
    t=0;b=0;l=0;r=0;
    textHeight=0;colWidth=74;idxSelected=-1;nButtons=0;
}
