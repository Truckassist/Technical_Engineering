#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "Index.h"
using namespace std;

void _tagStoreFileIndex::create_jpg(string& filename)
{
    int year_nr, month_nr, day_nr, hour_nr, minute_nr, second_nr;
    year_nr = FileTime.wYear;
    month_nr = FileTime.wMonth;
    day_nr = FileTime.wDay;
    hour_nr = FileTime.wHour;
    minute_nr = FileTime.wMinute;
    second_nr = FileTime.wSecond;

    stringstream year_ss, month_ss, day_ss, hour_ss, minute_ss, second_ss;
    year_ss << year_nr;
    month_ss << month_nr;
    day_ss << day_nr;
    hour_ss << hour_nr;
    minute_ss << minute_nr;
    second_ss << second_nr;

    string year_str, month_str, day_str, hour_str, minute_str, second_str;
    year_str = year_ss.str();
    month_str = month_ss.str();
    day_str = day_ss.str();
    hour_str = hour_ss.str();
    minute_str = minute_ss.str();
    second_str = second_ss.str();

    string dev_id_str = szDevIDNO;
    string channel_str(0, cChannle);   // Not working, just a blank

    string base_dir = "snapshots/" + dev_id_str + "/" + channel_str + "/";

    string mkdir = "mkdir -p " + base_dir;

    system(mkdir.c_str());
    // Maybe make string a cstring?
    filename = base_dir + year_str + month_str + day_str + "-" +
                hour_str + minute_str + second_str + ".jpg";
}

int _tagStoreFileIndex::get_filelen()
{
    return nFileLen;
}
