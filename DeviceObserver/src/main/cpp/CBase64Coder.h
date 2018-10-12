#ifndef _BASE64_H_
#define _BASE64_H_

#include<string>
using namespace std;

//#include <conf.h>
class CBase64Coder
{
private :
    static char ch64[];
    char* buf;
    int size ;

private :
    static int BinSearch(char p);
    void allocMem(int NewSize);

public :
    CBase64Coder();
    ~CBase64Coder();

    const char* encode(const string& message);
    const char* encode(const char* buffer,int buflen);
    const char* decode(const char* buffer,int Length);

};

#endif