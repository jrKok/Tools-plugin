#include "textreader.h"

TextReader::TextReader():
    textBuff(new std::vector<std::string>),
    sizeBuff(0),
    currentIdx(0),
    maxLineLength(MaxCharsPerLine),
    pageHeight(MaxLines),
    idxFirstOnPage(0),
    idxLastOnPage(0),
    measuredMaxLineLength(0),
    fileName(""),
    fileExists(false),
    lineSelected(-1),
    hasSelected(false),hasHidden(false),hasNav(false),hasCom(false),hasADF(false),
    digits("012456789"),
    freqADF(0),
    freqNAV(0),
    freqCOM(0)

{

}

TextReader::TextReader(std::string fileN):
    textBuff(new std::vector<std::string>),
    sizeBuff(0),
    currentIdx(0),
    maxLineLength(MaxCharsPerLine),
    pageHeight(MaxLines),
    idxFirstOnPage(0),
    idxLastOnPage(0),
    fileName(fileN),
    fileExists(false),
    lineSelected(-1),
    hasSelected(false),hasHidden(false),hasNav(false),hasCom(false),hasADF(false),
    digits("012456789"),
    freqADF(0),
    freqNAV(0),
    freqCOM(0)
{
    textFile.open(fileName,std::ifstream::in);
    if (textFile.is_open()){
        fileExists=true;
    }else{
        fileExists=false;
        fileName="";}

}

TextReader::~TextReader(){
    if (fileExists){
        textFile.clear();
        delete textBuff;
        textBuff=nullptr;
    }
}

bool TextReader::OpenFile(std::string name){ //sets textFile,FileName and FileExists
    fileName=name;
    textFile.open(fileName,std::ifstream::in);
    if (textFile.is_open()){
        fileExists=true;
    }else{
        fileExists=false;
        fileName="";}
    return fileExists;
}

bool TextReader::ReadFileToBuff(int maxLL){ //defines MaxLineLength and reads all lines into buffer, breaks at spaces,"-","(",or")" or at maxLineLength
  maxLineLength=maxLL;
  measuredMaxLineLength=0;
  if (!textFile.is_open()) textFile.open(fileName,std::ifstream::in);
  if ((maxLineLength==0)|!fileExists) return false;
  if (sizeBuff!=0) textBuff->clear();
  stringOps ops;
  std::string inputL;
  while (getline(textFile,inputL)){
     inputL=ops.cleanOut(inputL,"\r");
     inputL=ops.cleanOut(inputL,"\n");
     if (inputL.size()>measuredMaxLineLength) measuredMaxLineLength=inputL.size();
     while (inputL.size()>maxLineLength){
         std::string leftString=ops.bestLeft(inputL,maxLineLength);
         textBuff->push_back(leftString);
     }
     textBuff->push_back(inputL);
 }
  textFile.close();
  sizeBuff=textBuff->size();
  idxFirstOnPage=0;
  if (pageHeight<=sizeBuff) idxLastOnPage=pageHeight;
  else idxLastOnPage=sizeBuff;
  if (sizeBuff>0) return true;
  else return false;
}

size_t TextReader::GetBuffSize(){//reads sizeBuff
    return sizeBuff;
}

size_t TextReader::GetMaxLineLength(){
    return measuredMaxLineLength;
}

bool TextReader::BeginRead(){//puts currentIdx to 0 and verifies that sizeBuff is >0
    currentIdx=0;
    if (sizeBuff>0) return true;
    return false;
}

std::string TextReader::ReadOn(){//id currentIdx<sizeBuff returns string from currentIdx and increments currentIdx else ""
    if (currentIdx<sizeBuff){
    size_t oldId=currentIdx;
    currentIdx++;
    return (*textBuff)[oldId];}
    return("");
}

bool TextReader::BuffEnd(){//tests if currentIdx=sizeBuff
    if (currentIdx>=sizeBuff) return true;
    else return false;
}

void TextReader::SetPageHeight(int pgh){
    pageHeight=pgh;
    idxFirstOnPage=0;
    if (pageHeight<=sizeBuff) idxLastOnPage=pageHeight;
    else idxLastOnPage=sizeBuff;

}

bool TextReader::NextPage(){
    size_t oldFirst=idxFirstOnPage;
    idxFirstOnPage=idxLastOnPage;
    idxLastOnPage+=pageHeight;
    if (idxLastOnPage>sizeBuff)idxLastOnPage=sizeBuff;
    idxFirstOnPage=idxLastOnPage-pageHeight;
    if (idxFirstOnPage<0) idxFirstOnPage=0;

    BeginPage();
    if (oldFirst==idxFirstOnPage) return false;
    else{
        ResetSelect();
        ResetFrequencies();
        return true;}
}

bool TextReader::PreviousPage(){
    size_t oldFirst=idxFirstOnPage;
    if (idxFirstOnPage<=pageHeight) //if idxFirstOnPage gets negative, hell breaks lose (size_t is unsigned)
    {idxFirstOnPage=0;}
    else
    {idxFirstOnPage-=pageHeight;}
    idxLastOnPage=idxFirstOnPage+pageHeight;
    if (idxLastOnPage>sizeBuff) idxLastOnPage=sizeBuff;
    BeginPage();

    if (oldFirst==idxFirstOnPage) return false;
    else{
        if ((lineSelected>=idxFirstOnPage)&(lineSelected<idxLastOnPage)){
            return true;
        }

     ResetSelect();
     ResetFrequencies();
     return true;}
}

bool TextReader::ShiftDown1Line(){
    bool ret=true;
    idxLastOnPage++;
    if (idxLastOnPage>sizeBuff) {
        idxLastOnPage--;
        ret=false;}
    else idxFirstOnPage++;
    return ret;
}

bool TextReader::ShiftUp1Line(){
    bool ret=true;
    idxFirstOnPage--;
    if (idxFirstOnPage<0) {
        idxFirstOnPage=0;
        ret=false;}
    else idxLastOnPage--;
    return ret;
}

void TextReader::BeginPage(){
    currentIdx=idxFirstOnPage;
}

std::string TextReader::ReadOnPage(){
    if (currentIdx<idxLastOnPage){
    size_t oldId=currentIdx;
    currentIdx++;
    return (*textBuff)[oldId];}
    return("");
}

bool TextReader::EndOfPage(){
    if (currentIdx==idxLastOnPage) return true;
    else return false;
}
std::string TextReader::StringAt(int Idx){
    if (Idx<sizeBuff) return (*textBuff)[Idx];
    else return ("");
}

bool TextReader::hasSelectedLine(){
    return hasSelected;
}

bool TextReader::atSelectedLine(){
    if (lineSelected==(currentIdx-1)) return true;
    return false;
}

void TextReader::select(int in_number){
    int in_line=in_number+idxFirstOnPage;
    if (hasSelected){
        if (in_line==lineSelected) {
            ResetSelect();
            ResetFrequencies();
            return;
        }
    }
    hasSelected=true;
    lineSelected=in_line;
    CheckForFrequencies();

}

bool TextReader::nextLine(){
    if (hasSelected){
        lineSelected++;
        if (lineSelected==sizeBuff) lineSelected=sizeBuff-1;
        else {if (lineSelected==idxLastOnPage) {
                CheckForFrequencies();
                return (ShiftDown1Line());
            }
        }
    }
    else   return (ShiftDown1Line());
    return (true);
}

bool TextReader::previousLine(){
    if (hasSelected){
        lineSelected--;
        if (lineSelected<0) {
            lineSelected=0;
            return false;
        }
        else {
            CheckForFrequencies();
            if (lineSelected<idxFirstOnPage) return (ShiftUp1Line());
        }
    }
     else return (ShiftUp1Line());
    return(false);
}

bool TextReader::IsSelectAtBegin(){
    if (hasSelectedLine()&(lineSelected==0)) return (true);
    else return false;
}
bool TextReader::IsSelectAtEnd(){
    if (hasSelectedLine()&(lineSelected==sizeBuff-1)) return(true);
    else return false;
}

bool TextReader::HasNav(){
    return hasNav;
}
bool TextReader::HasCom(){
    return hasCom;
}
bool TextReader::HasADF(){
    return hasADF;
}
bool TextReader::LastPage(){
    if (idxLastOnPage==sizeBuff) return true;
    else return false;
}

bool TextReader::HasHiddenLine(){
    return hasHidden;
}

bool TextReader::FirstPage(){
    if (idxFirstOnPage==0) return true;
    else return false;
}

void TextReader::CheckForFrequencies(){
    float floatRead;
    int intRead,size;
    size_t pos;
    std::string remnant=(*textBuff)[lineSelected];
    ResetFrequencies();
    while (remnant.size()>0){
        if (isADigit(remnant.substr(0,1))){
              intRead=std::stoi(remnant,&pos);
              if ((intRead>=200)&(intRead<=1800)) {
                  freqADF=intRead;
                  hasADF=true;
              }
              else {
                  floatRead=std::stof(remnant,&pos);
                  if ((floatRead>=108.00) & (floatRead<=117.99)) {
                      freqNAV=(int)(floatRead*100.0f);
                      hasNav=true;
                  }
                  if ((floatRead>=118.00)&(floatRead<=139.99)){
                      freqCOM=floatRead*100.0f;
                      hasCom=true;
                  }
              }
              if (pos==0) pos=1;
              remnant=remnant.substr(pos);
        }
        else {remnant=remnant.substr(1);}
    }

}

bool TextReader::isADigit(std::string test){
    if (digits.find(test)!=std::string::npos) return (true);
    else return (false);
}

void TextReader::ShowAll(){
    ReadFileToBuff(maxLineLength);
    hasSelected=false;
    hasHidden=false;
    lineSelected=-1;
}

void TextReader::DeleteLine(){
    if ((lineSelected>=0)&(lineSelected<sizeBuff)){
    (*textBuff)[lineSelected]="";
    hasHidden=true;
    hasSelected=false;
    lineSelected=-1;
    }
}

void TextReader::ResetSelect(){
    hasSelected=false;
    lineSelected=-1;
}

void TextReader::ResetFrequencies(){
    hasNav=false;hasADF=false;hasCom=false;
    freqADF=0;freqCOM=0;freqNAV=0;
}

int TextReader::IndexFirstOfPage(){
    return(idxFirstOnPage);
}
int TextReader::IndexLastOfPage(){
    return(idxLastOnPage);
}
void TextReader::closeReader(){
    textBuff->clear();
    sizeBuff=0;
    currentIdx=0;
    maxLineLength=MaxCharsPerLine;
    pageHeight=MaxLines;
    idxFirstOnPage=0;
    idxLastOnPage=0;
    measuredMaxLineLength=0;
    lineSelected=-1;
    hasSelected=false;hasHidden=false;hasNav=false;hasCom=false;hasADF=false;
    freqADF=0;
    freqNAV=0;
    freqCOM=0;
}
