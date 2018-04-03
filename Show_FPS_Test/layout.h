#ifndef LAYOUT_H
#define LAYOUT_H
#include <string>
#include <string.h>
#include <vector>
#include "XPLMDisplay.h"    // for window creation and manipulation
#include "XPLMGraphics.h"   // for window drawing
#include "XPLMDataAccess.h" // for the VR dataref
#include "XPLMPlugin.h"     // for XPLM_MSG_SCENERY_LOADED message
#include "XPLMUtilities.h"
#include "textreader.h"
#include "rectangles.h"
#include "textline.h"
#include "show_fps_test_global.h"


class TextReader;

class Layout
{
public:
    Layout();
    ~Layout();
    int gTop,gBottom,gLeft,gRight;//Used for initial calculations and coordinates,global coordinates
    int wTop,wBottom,wLeft,wRight,wWidth,wHeight,charHeight,charWidth,maxLines,maxChars,textPointX,textPointY;//Used for initial setup and resize instructions
    int t,b,l,r;//input from draw instruction mostly
    int textHeight,colWidth,idxSelected,nButtons;
    TextReader *tFileReader;
    TextLine TitleLine;
    rectangles generalR,textR;
    std::vector<TextLine> tLines;
    std::vector<rectangles> tBoxes;
    rectangles bLoad_File;

    void resize();
    void recalculate();
    bool initiate();
    int rightRectangle(int idx);
    int leftRectangle(int idx);
    int topRectangle(int idx);
    int bottomRectangle(int idx);
    int leftTextLine(int idx);
    int bottomTextLine(int idx);
    std::string nextTextLine();
    int findClick(int, int,char&);
    void defineButtons();
    void CheckButtonsVisibility();
    void ClosegWindow();
};

#endif // LAYOUT_H
