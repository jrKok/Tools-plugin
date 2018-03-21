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
