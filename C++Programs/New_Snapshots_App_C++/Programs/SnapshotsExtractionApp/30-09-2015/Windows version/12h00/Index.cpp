#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
// #include <sstream>
#include <cstdlib>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Index.h"
using namespace std;

void StoreFileIndex::create_jpg(string& filename)
{


    // place SystemTime variables into local variables
    int yyyy, mm, dd, hh, ii, ss;
    yyyy = FileTime.wYear;
    mm = FileTime.wMonth;
    dd = FileTime.wDay;
    hh = FileTime.wHour;
    ii = FileTime.wMinute;
    ss = FileTime.wSecond;
    char jpg_date[18];
    sprintf(jpg_date, "%04d%02d%02d-%02d%02d%02d", yyyy, mm, dd, hh, ii, ss);

    // convert the integers to string streams
    // in order to convert them to strings
    /*stringstream year_ss, month_ss, day_ss, hour_ss, minute_ss, second_ss;
    year_ss << FileTime.wYear;
    month_ss << FileTime.wMonth;
    day_ss << FileTime.wDay;
    hour_ss << FileTime.wHour;
    minute_ss << FileTime.wMinute;
    second_ss << FileTime.wSecond;

    // Convert the string streams to strings
    string year_str, month_str, day_str, hour_str, minute_str, second_str;
    year_str = year_ss.str();
    month_str = month_ss.str();
    day_str = day_ss.str();
    hour_str = hour_ss.str();
    minute_str = minute_ss.str();
    second_str = second_ss.str();*/

    // Implicitly convert the char variables to strings
    string dev_id_str = szDevIDNO;
    string channel_str(0, cChannle);   // Not working, just a blank

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
    string newfolder2 = newfolder1 + dev_id_str + "\\";
    CreateDirectory(newfolder2.c_str(), NULL);
	/*if (!CreateDirectory(full_path.c_str(), NULL))
    {
        cout << "ERROR! CANNOT CREATE BASE FILE" << endl;
    }*/

    /*
    string mkdir = "mkdir -p " + base_dir;

    system(mkdir.c_str());
    */

    // Maybe make string a cstring?
    filename = newfolder2 + jpg_date + ".jpg";
    // year_str + month_str + day_str + "-" +
    // hour_str + minute_str + second_str + ".jpg";
}

int StoreFileIndex::get_filelen()
{
    return nFileLen;
}
