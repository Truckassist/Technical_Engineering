#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Index.h"
using namespace std;

void StoreFileIndex::create_jpgfile(string& filename)
{
    // place SystemTime variables into local variables
    int yyyy, mm, dd, hh, ii, ss;
    yyyy = FileTime.wYear;
    mm   = FileTime.wMonth;
    dd   = FileTime.wDay;
    hh   = FileTime.wHour;
    ii   = FileTime.wMinute;
    ss   = FileTime.wSecond;
    char jpg_date[18], dir_date[11];

    sprintf(jpg_date, "%04d%02d%02d_%02d%02d%02d", yyyy, mm, dd, hh, ii, ss);
    sprintf(dir_date, "%04d-%02d-%02d", yyyy, mm, dd);
    // sprintf(channel, '%c', channel);

    // Implicitly convert the char variables to strings
    string dev_id_str = szDevIDNO;
    //string channel_str(1, cChannle);   // Not working, just a blank
    int channel_int = cChannle;
    ostringstream channel_ss;
    channel_ss << channel_int + 1;
    string channel_str = channel_ss.str();
    // Get the current dirrectory and place it in the character
    // variable szPath
 	TCHAR szPath[MAX_PATH] = _T("");
	GetCurrentDirectory(MAX_PATH, szPath);

    // Implicitly convert szPath to a string
    string base_dir = szPath;

    // create necessary folders
    // CreateDirectory only works for a single
    // directory unfortunately
    string newfolder1 = base_dir + "\\snapshots\\";
    CreateDirectory(newfolder1.c_str(), NULL);
    string newfolder2 = newfolder1 + dir_date + "\\";
    CreateDirectory(newfolder2.c_str(), NULL);
    string newfolder3 = newfolder2 + dev_id_str + "\\";
    CreateDirectory(newfolder3.c_str(), NULL);
    string newfolder4 = newfolder3 + "CH" + channel_str + "\\";
    CreateDirectory(newfolder4.c_str(), NULL);

    // Maybe make string a cstring?
    filename = newfolder4 + dev_id_str + "_" + jpg_date + "_N" + channel_str + "0.jpg";
}

int StoreFileIndex::get_filelen()
{
    return nFileLen;
}
