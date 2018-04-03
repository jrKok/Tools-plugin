#include "textline.h"

TextLine::TextLine() :
    textOfLine(""),
    index(0),
    x(0),y(0),offSetX(0),offSetY(0),in_top(0),in_left(0),width(0),height(0),top(0),right(0)
{

}


void TextLine::recalculate(){
    x=in_left+offSetX;
    y=in_top-offSetY;
    top=y+height;
    right=x+width;

}
void TextLine::setText (std::string in_Str){
    textOfLine=in_Str;
    index=-1;
}

bool TextLine::isHere(int x_in, int y_in){
    if ((x_in>=x)&(x_in<=right)&(y_in<=top)&(y_in>=y)) return true;
    else return false;
}
