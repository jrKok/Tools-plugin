#ifndef TEXTREADER_H
#define TEXTREADER_H
#include <string>
#include <stringops.h>
#include <fstream>
#include <vector>
#include "show_fps_test_global.h"


class TextReader
{
public:
    TextReader();
    TextReader(std::string name);
    ~TextReader();

    int freqADF,freqNAV;
    float freqCOM;

    bool OpenFile(std::string name); //sets textFile and FileExists
    bool ReadFileToBuff(int maxLL); //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength
    size_t GetBuffSize();//reads sizeBuff
    size_t GetMaxLineLength();
    bool BeginRead();//puts currentIdx to 0 and verifies that sizeBuff is >0
    std::string ReadOn();//id currentIdx<sizeBuff returns string from currentIdx and increments currentIdx else ""
    bool BuffEnd();//tests if currentIdx=sizeBuff
    void SetPageHeight(int pgh);
    bool NextPage();
    bool PreviousPage();
    bool ShiftDown1Line();
    bool ShiftUp1Line();
    void BeginPage();
    std::string ReadOnPage();
    bool EndOfPage();
    std::string StringAt(int Idx);
    bool hasSelectedLine();
    bool atSelectedLine();
    void select(int in_number);
    bool nextLine();
    bool previousLine();
    bool IsSelectAtBegin();
    bool IsSelectAtEnd();
    bool HasHiddenLine();
    bool HasNav();
    bool HasCom();
    bool HasADF();
    bool LastPage();
    bool FirstPage();
    bool isADigit(std::string test);
    void DeleteLine();
    void ShowAll();
    int IndexFirstOfPage();
    int IndexLastOfPage();
    void closeReader();

private:
    std::vector<std::string> *textBuff;
    size_t sizeBuff,currentIdx;
    size_t maxLineLength,pageHeight,idxFirstOnPage,idxLastOnPage,measuredMaxLineLength;
    std::string fileName;
    std::ifstream textFile;
    bool fileExists;
    int lineSelected;
    bool hasSelected,hasHidden,hasNav,hasCom,hasADF;
    std::string digits;

    void CheckForFrequencies();
    void ResetSelect();
    void ResetFrequencies();

};

#endif // TEXTREADER_H
