// Index file data structure
#ifndef INDEX_H
#define INDEX_H

#pragma pack(4)
typedef struct _SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

// TOTAL BYTE SIZE:
typedef struct _tagStoreFileIndex
{
    char szDevIDNO[32];		// device number
    char cFileType;			// file type  0 normal 1 alarm
    char cChannle;			// channel number（start from 0）
    char cEncode;			// coded format（not used）
    char cReserve;			// Alternate field
    SYSTEMTIME FileTime;    // File time
    int nFileOffst;			// File migration
    int nFileLen;			// File length
    char cEndFlag[4];		// End Flag
} StoreFileIndex_S, *LPStoreFileIndex_S;
#pragma pack()

const char INDEX_END_FLAG[] = "@@@@";   // End flag

#endif // INDEX_H
