#ifndef STRINGOPS_H
#define STRINGOPS_H
#include <string>
#include <vector>
#include <fstream>

class stringOps
{
public:
    stringOps();
    bool contains(const std::string inString, const std::string inContains);
    std::string splitRight(const std::string inString, const std::string split);
    std::string cleanOut(const std::string inString,const std::string inToClean);
    std::string bestLeft(std::string &inString, int MaxL);
};

#endif // STRINGOPS_H
