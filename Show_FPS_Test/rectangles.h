#ifndef RECTANGLES_H
#define RECTANGLES_H
#include <string>
#include <XPLMGraphics.h>

class rectangles
{
public:
    rectangles();
    int top,left,bottom,right,in_top,in_left,height,width,offsetX,offsetY;
    int posTextX,posTextY, offsetTextX, offsetTextY;
    bool isVisible;
    std::string buttonText;

    void recalculate();
    void setText(std::string);
    bool isHere(int,int);
};

#endif // RECTANGLES_H
