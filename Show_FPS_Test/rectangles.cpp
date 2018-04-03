#include "rectangles.h"

rectangles::rectangles() :
    top(0),left(0),bottom(0),right(0),in_top(0),in_left(0),height(0),width(0),offsetX(0),offsetY(0),
    posTextX(0),posTextY(0), offsetTextX(0), offsetTextY(0),
    isVisible(true),
    buttonText("")
{

}

void rectangles::recalculate(){
    top=in_top-offsetY;
    bottom=in_top-offsetY-height;
    left=in_left+offsetX;
    right=in_left+offsetX+width;
    posTextX=in_left+offsetTextX;
    posTextY=in_top-offsetTextY+2;
}

bool rectangles::isHere(int x, int y){
    if ((x>=left)&(x<=right)&(y<=top)&(y>=bottom)&isVisible) return true;
    else return false;

}

void rectangles::setText(std::string in_String){
    buttonText=in_String;
    //calculates Offsets for text, adjusts width of the rectangle if necessary
    //should occur after offsets of rectangle have been entered(I don't verify that until problems arise
    int tWidth=(int)XPLMMeasureString(xplmFont_Proportional,(char*)in_String.c_str(),in_String.length());
    int tHeight(0);
    XPLMGetFontDimensions(xplmFont_Proportional,NULL,&tHeight,NULL);
    offsetTextY=offsetY+(int)(height/2)+(int)(tHeight/2);
    if (tWidth>=(width-4)) width=tWidth+4;
    offsetTextX=offsetX+(int)(width/2)-(int)(tWidth/2);
}
