// Index file data structure
#ifndef INDEX_H
#define INDEX_H
// #include "SystemTime.h"
#include <string>
#include <windows.h>
using namespace std;

class StoreFileIndex
{
public:
    void create_jpgfile(string& filename);
    // #1 Creates the correct directory tree using the device id
    // #2 Returns by reference the filename of the jpg file to be
    // created and written to in the main program
    int get_filelen();
    // Returns the length of the jpg extract in the picfile
    // This value is read from the index file

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
