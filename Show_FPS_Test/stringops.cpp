#include "stringops.h"

stringOps::stringOps()
{

}

bool stringOps::contains(const std::string inString, const std::string inContains){
    std::size_t fd=inString.find(inContains);
    if (fd!=std::string::npos) return true;
        else return false;
}

std::string stringOps::splitRight(const std::string inString, const std::string inSplit){
    std::size_t len=inString.length(),lenSpl=inSplit.length();
    if ((len>0)&(lenSpl>0)){
        std::size_t fd=inString.find(inSplit);
        if(fd != std::string::npos){
            std::string retStr=inString.substr(fd+lenSpl,len-fd+lenSpl);
            return(retStr);
        }else return("");
    }
    return("");
}

std::string stringOps::cleanOut(const std::string inString,const std::string inToClean){
    size_t len=inToClean.size();
    std::string outString=inString;
    size_t pos=outString.find(inToClean);
    while(pos!=std::string::npos){
        outString=outString.erase(pos,len);
        pos=outString.find(inToClean);
    }
    return (outString);
}

std::string stringOps::bestLeft(std::string &inString,int MaxL){
    std::string left("");
    if (MaxL>0){
      left=inString.substr(0,MaxL);//do a brutal split at MaxL
      inString=inString.substr(MaxL);
      size_t posSpc=left.find_last_of(" ");//try to split at last space found
      if (posSpc!=std::string::npos){
          inString=left.substr(posSpc+1)+inString; // if possible do the split not including the space itself.
          left=left.substr(0,posSpc);
      }
    }
    return left;
}
