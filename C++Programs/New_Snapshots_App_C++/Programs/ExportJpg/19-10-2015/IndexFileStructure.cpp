#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <sstream>
#include <dirent.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "IndexFileStructure.h"
#include "IniWriter.h"
#include "IniReader.h"
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
    char jpg_date[18], dir_date[11], year[4], month[4], day[4];

    sprintf(jpg_date, "%04d%02d%02d_%02d%02d%02d", yyyy, mm, dd, hh, ii, ss);
    sprintf(dir_date, "%04d-%02d-%02d", yyyy, mm, dd);
    sprintf(year, "%04d", yyyy);
    sprintf(month, "%02d", mm);
    sprintf(day, "%02d", dd);
    //sprintf(channel, '%c', channel);

    // Implicitly convert the char variables to strings
    string dev_id_str = szDevIDNO;
    //string channel_str(1, cChannle);   // Not working, just a blank
    int channel_int = cChannle;
    ostringstream channel_ss;
    channel_ss << channel_int + 1;
    string channel_str = channel_ss.str();

    // create necessary folders
    // CreateDirectory only works for a single
    // directory unfortunately
    string newfolder1 = snaphotsdir();
    CreateDirectory(newfolder1.c_str(), NULL);
    string newfolder2 = newfolder1 + dev_id_str + "\\";
    CreateDirectory(newfolder2.c_str(), NULL);
    string newfolder3 = newfolder2 + year + "\\";
    CreateDirectory(newfolder3.c_str(), NULL);
    string newfolder4 = newfolder3 + month + "\\";
    CreateDirectory(newfolder4.c_str(), NULL);
    string newfolder5 = newfolder4 + day + "\\";
    CreateDirectory(newfolder5.c_str(), NULL);
    string newfolder6 = newfolder5 + "CH" + channel_str + "\\";
    CreateDirectory(newfolder6.c_str(), NULL);

    // Maybe make string a cstring?
    filename = newfolder6 + dev_id_str + "_" + jpg_date + "_N" + channel_str + "0.jpg";
}

int StoreFileIndex::get_fileoffset()
{
    return nFileOffst;
}

int StoreFileIndex::get_filelen()
{
    return nFileLen;
}

string StoreFileIndex::snaphotsdir()
{
    char* get_ini_open = convert_tochar_darray(inifilename());
    CIniReader iniReader(get_ini_open);
    delete[] get_ini_open;
    char *snapshotpath_ch = iniReader.ReadString("LOCATION", "Snapshots", "");
    string snapshotpath = string(snapshotpath_ch);
    delete[] snapshotpath_ch;
    return snapshotpath;
}

string StoreFileIndex::basedir()
{
    // Get the current directory and place it in the character variable szPath
    // Uses tchar.h and stdio.h
    TCHAR szPath[MAX_PATH] = _T("");
    GetCurrentDirectory(MAX_PATH, szPath);

    // Implicitly convert szPath to a string
    string base_dir = szPath;
    return base_dir;
}

//--------------------------------------------------------------------------------------------------//
//-----------------------------------CONVERT STRING TO CHAR*----------------------------------------//
//--------------------------------------------------------------------------------------------------//

char* StoreFileIndex::convert_tochar_darray(const string filename)
{
    char* ch = new char[filename.length() + 1];
    strcpy(ch, filename.c_str());
    return ch;
}

//--------------------------------------------------------------------------------------------------//
//-------------------------------RETURN THE INI FILE FULL PATH--------------------------------------//
//--------------------------------------------------------------------------------------------------//

string StoreFileIndex::inifilename()
{
    return basedir() + "\\index_arraypos.ini";
}
