// Index file data structure
#ifndef INDEX_H
#define INDEX_H
// #include "SystemTime.h"
#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

class StoreFileIndex
{
public:
    void create_jpg(string& filename);
    int get_filelen();
private:
    char szDevIDNO[32];		// device number
    char cFileType;			// file type  0 normal 1 alarm
    char cChannle;			// channel number（start from 0）
    char cEncode;			// coded format（not used）
    char cReserve;			// Alternate field
    SYSTEMTIME FileTime;    // File time
    int nFileOffst;			// File migration
    int nFileLen;			// File length
    char cEndFlag[4];		// End Flag
};

//  StoreFileIndex_S, *LPStoreFileIndex_S
const char INDEX_END_FLAG[] = "@@@@";   // End flag

#endif // INDEX_H
