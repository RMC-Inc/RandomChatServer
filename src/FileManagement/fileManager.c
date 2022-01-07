#include "fileManager.h"
#include <string.h>

int stringInside(const char* in, char left, char right, char* out){
    int strStart, strEnd;
    unsigned int strLen = strlen(in);

    for (strStart = 0; strStart < strLen && in[strStart] != '['; ++strStart);
    strStart++;
    for (strEnd = strStart; strEnd < strLen && in[strEnd] != ']'; ++strEnd);
    if(strEnd <= strStart) return 0;

    memcpy(out,in + strStart, strEnd - strStart);
    out[strEnd - strStart] = '\0';

    return strEnd - strStart;
}