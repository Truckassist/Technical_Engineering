#include <string>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <tchar.h>
#include <stdio.h>
#include "ExtractFile.h"
#include "IniWriter.h"
#include "IniReader.h"

using namespace std;

//--------------------------------------------------------------------------------------------------//
//--------------------------------CREATE A VECTOR OF INDEX FILES------------------------------------//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::GetNewFilenames()
{
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    log << current_time() << "**START OF PROGRAM**" << endl;
    cout << current_time() << "**START OF PROGRAM**" << endl;
    log << current_time() << "Beginning to search for more files:" << endl;
    cout << current_time() << "Beginning to search for more files:" << endl;

    // Needs dirent.h --> For searching through the dir
    DIR *dir;
    struct dirent *ent;

    // Clear the vector of index files
    indexfiles.clear();

    if ((dir = opendir (picfiledir().c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            NewFile = ent->d_name;
            // Get the extension (picfile or index)
            PosIndex = NewFile.find_last_of('.');
            ext = NewFile.substr(PosIndex, -1);

            if (ext == ".index")
            {
                int completed = get_completed(NewFile);
                if (completed == 0)
                {
                    cout << current_time() << NewFile << endl;
                    log << current_time() << NewFile << endl;
                    // Get just the date part of the filename
                    int filedatepos = NewFile.find_first_of('.');
                    string filedate = NewFile.substr(0, filedatepos);
                    indexfiles.push_back(filedate);
                    set_lastfile(NewFile);
                }
            }
        }
    }
    else
    {
        log << current_time() << "Could not open directory picfiles. Aborting!" << endl;
        exit(1);
    }

    delete ent;

    closedir (dir);


    log.close();
}

//--------------------------------------------------------------------------------------------------//
//------------------------------------READ THE INDEX FILE-------------------------------------------//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::CheckForExtractFile(const string fileP)
{
    ifstream infile;
    ofstream log;

    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }

    string fullfilename = picfiledir() + fileP + ".picfile.index";
    cout << current_time() << "Reading " << fileP << ".picfile.index" << endl;
    log << current_time() << "Reading " << fileP << ".picfile.index" << endl;

    infile.open(fullfilename.c_str(), ios::binary | ios::in);

    if (infile.fail())
    {
        log << current_time() << "Failed to open " << fileP << ".picfile.index. Aborting!" << endl;
        exit(1);
    }
    log << current_time() << "Opened " << fileP << ".picfile.index" << endl;

    long offVal = get_counter(fileP + ".picfile.index");
    counter = 0;
    IndexBlockVector.clear();
    // This while loop reads from the last index block to the end of the file
    while (!infile.eof())
    {
        infile.read((char*)&IndexBlock, sizeof(IndexBlock));

        if (counter >= offVal)
        {
            // Create memory space for newly created object in the vector
            IndexBlockVector.push_back(IndexBlock);
        }
        counter++;
    }
    if (offVal > counter)
    {
        cout << current_time() << fileP + ".picfile.index file count specified is out of range. Aborting!" << endl;
        log << current_time() << fileP + ".picfile.index file count specified is out of range. Aborting!" << endl;
        exit(1);
    }
    log << current_time() << "Index vector size: " << counter << endl;
    cout << current_time() << "Index vector size: " << counter << endl;
    infile.close();
    cout << current_time() << "Closing " << fileP << ".picfile.index" << endl;
    log << current_time() << "Closing " << fileP << ".picfile.index" << endl;
    if (counter == offVal)
    {
        log << current_time() << "This is the end of the file" << endl;
        cout << current_time() << "This is the end of the file" << endl;

        // Set file to done in ini file (all except last file)
        if ((fileP + ".picfile.index") != get_lastfile())
            set_completed(1, fileP + ".picfile.index");
    }
    else
    {
        picfile_offset = IndexBlockVector.at(0).get_fileoffset();

        // The file has been appended to, therefore it is not completed
        set_completed(0, fileP + ".picfile.index");

        // Start exporting Jpg's using the offset and filelen
        // specified in index file
        SaveJpgs(fileP);
    }

    log.close();
}

//--------------------------------------------------------------------------------------------------//
//---------------------------------------SAVE THE JPGS!---------------------------------------------//
//---Take the offset and file lengths from the index file and use it to create a separate jpg file--//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::SaveJpgs(const string fileP)
{
    ifstream picfile;
    ofstream jpg_file;
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }

    string fullfilename = picfiledir() + fileP + ".picfile";
    cout << current_time() << "Reading " << fileP << ".picfile" << endl;
    log << current_time() << "Reading " << fileP << ".picfile" << endl;

    // Get the previous turns file count
    int oldfilecount = get_counter(fileP + ".picfile.index");
    log << current_time() << "Initial JPG: #" << oldfilecount << endl;
    cout << current_time() << "Initial JPG: #" << oldfilecount << endl;

    // using details from the index file we now open the picfile
    picfile.open(fullfilename.c_str(), ios::binary | ios::out);
    if (picfile.fail())
    {
        log << current_time() << "Failed to open " << fileP << ".picfile! Aborting!" << endl;
        exit(1);
    }

    // Proceed to the picfile's offset
    picfile.seekg(picfile_offset, ios::beg);
    unsigned int i=0;

    while (i < IndexBlockVector.size())
    {
        // Now read a block of the picfile using the filelen specified in index file
        char * memblock = new char [IndexBlockVector.at(i).get_filelen()];
        picfile.read(memblock, IndexBlockVector.at(i).get_filelen());

        // Create a JPG file to write to
        IndexBlockVector.at(i).create_jpgfile(jpgfilename);
        // Open JPG file
        jpg_file.open(jpgfilename.c_str(), ios::binary | ios::out);
        if (jpg_file.fail())
        {
            log << current_time() << "Failed to create " << jpgfilename << "! Aborting!" << endl;
            exit(1);
        }

        // Write memblock to newly created JPG file
        jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen());
        if (! jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen()))
        {
            log << current_time() << "Cannot write to " << jpgfilename << ". Aborting!" << endl;
            exit(1);
        }

        jpg_file.close();
        // Every 100 records update the ini file
        if (i % 100 == 0)
        {
            set_counter(oldfilecount + i, fileP + ".picfile.index");
        }
        i++;
        cout << '\r' << oldfilecount+i;

        delete [] memblock;
    }
    set_counter(oldfilecount + i, fileP + ".picfile.index");
    cout << endl;
    log << current_time() << "End JPG: #" << oldfilecount+i << endl;
    cout << current_time() << "End JPG: #" << oldfilecount+i << endl;
    cout << current_time() << "Closing " << fileP << ".picfile" << endl;
    log << current_time() << "Closing " << fileP << ".picfile" << endl;
    log.close();
    picfile.close();
}

//--------------------------------------------------------------------------------------------------//
//-----------------------------------------GET THE CWD----------------------------------------------//
//--------------------------------------------------------------------------------------------------//

string ExtractFile::basedir()
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

char* ExtractFile::convert_tochar_darray(const string filename)
{
    char* ch = new char[filename.length() + 1];
    strcpy(ch, filename.c_str());
    return ch;
}

//--------------------------------------------------------------------------------------------------//
//-------------------------------RETURN THE INI FILE FULL PATH--------------------------------------//
//--------------------------------------------------------------------------------------------------//

string ExtractFile::inifilename()
{
    return basedir() + "\\index_arraypos.ini";
}

//--------------------------------------------------------------------------------------------------//
//------------------------------RETURN THE VECTOR OF INDEX FILES------------------------------------//
//--------------------------------------------------------------------------------------------------//

vector<string> ExtractFile::GetVectors()
{
    return indexfiles;
}

//--------------------------------------------------------------------------------------------------//
//-----------------------------GET PiCFILE DIRECTORY FROM INI FILE----------------------------------//
//--------------------------------------------------------------------------------------------------//

string ExtractFile::picfiledir()
{
    char* get_ini_open = convert_tochar_darray(inifilename());
    CIniReader iniReader(get_ini_open);
    delete[] get_ini_open;
    char *picfilepath_ch = iniReader.ReadString("LOCATION", "Picfiles", "");
    string picfilepath = string(picfilepath_ch);
    delete[] picfilepath_ch;
    return picfilepath;
}

//--------------------------------------------------------------------------------------------------//
//------------------------------SET FILE AS COMPLETED IN INI FILE-----------------------------------//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::set_completed(const int answer, const string file)
{
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    if ((answer == 1) || (answer == 0))
    {
        // Set file to done in ini file
        char* inifile = convert_tochar_darray(inifilename());
        char* section = convert_tochar_darray(file);
        CIniWriter iniWriter(inifile);
        delete[] inifile;
        iniWriter.WriteInteger(section, "Completed", answer);
        delete[] section;
    }
    else
    {
        log << current_time() << "Invalid input to completed function." << endl;
        exit(1);
    }
    log.close();
}

//--------------------------------------------------------------------------------------------------//
//----------------------GET THE INDEX FILE COUNTER OFFSET FROM INI FILE-----------------------------//
//--------------------------------------------------------------------------------------------------//

int ExtractFile::get_counter(const string file)
{
    char* inifile = convert_tochar_darray(inifilename());
    CIniReader iniReader(inifile);
    delete[] inifile;
    char* section = convert_tochar_darray(file);
    int intVal = iniReader.ReadInteger(section, "Counter", 0);
    delete[] section;
    return intVal;
}

//--------------------------------------------------------------------------------------------------//
//---------------------------GET THE COMPLETED STATUS FROM INI FILE---------------------------------//
//--------------------------------------------------------------------------------------------------//

int ExtractFile::get_completed(const string file)
{
    char* inifile = convert_tochar_darray(inifilename());
    CIniReader iniReader(inifile);
    delete[] inifile;
    char* section = convert_tochar_darray(file);
    int completed = iniReader.ReadInteger(section, "Completed", 0);
    delete[] section;
    return completed;
}

//--------------------------------------------------------------------------------------------------//
//-------------------------SET INDEX FILE COUNTER OFFSET IN INI FILE--------------------------------//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::set_counter(const int n, const string file)
{
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    if (n >= 0)
    {
        // Set file to done in ini file
        char* inifile = convert_tochar_darray(inifilename());
        char* section = convert_tochar_darray(file);
        CIniWriter iniWriter(inifile);
        delete[] inifile;
        iniWriter.WriteInteger(section, "Counter", n);
        delete[] section;
    }
    else
    {
        log << current_time() << "Invalid input to counter function." << endl;
        exit(1);
    }
    log.close();
}

//--------------------------------------------------------------------------------------------------//
//--------------------------------GET CURDATE FOR USE IN LOGGING------------------------------------//
//--------------------------------------------------------------------------------------------------//

string ExtractFile::current_time()
{
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }

    time_t currentTime;
    struct tm *localTime;
    char Timestamp[25];
    time( &currentTime );                   // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

    int dd   = localTime->tm_mday;
    int mm   = localTime->tm_mon + 1;
    int yyyy = localTime->tm_year + 1900;
    int HH   = localTime->tm_hour;
    int ii   = localTime->tm_min;
    int ss   = localTime->tm_sec;

    sprintf(Timestamp, "[%04d-%02d-%02d %02d:%02d:%02d] ", yyyy,mm,dd,HH,ii,ss);
    if (Timestamp != NULL)
        string str(Timestamp);
    else
    {
        cout << "Error converting Timestamp to a string. Aborting!" << endl;
        log << "Error converting Timestamp to a string. Aborting!" << endl;
        exit(1);
    }
    log.close();
    //string Timestamp = "[" + year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second + "] ";
    return Timestamp;
}

//--------------------------------------------------------------------------------------------------//
//-------------------------------GET LAST FILENAME FROM INI FILE------------------------------------//
//--------------------------------------------------------------------------------------------------//

char* ExtractFile::get_lastfile()
{
    char* inifile = convert_tochar_darray(inifilename());
    CIniReader iniReader(inifile);
    delete[] inifile;
    char* lastfile = iniReader.ReadString("Last File", "filename", "");
    return lastfile;
}

//--------------------------------------------------------------------------------------------------//
//---------------------------------SET THE NEW LAST FILE VALUE--------------------------------------//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::set_lastfile(const string file)
{
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    char* inifile = convert_tochar_darray(inifilename());
    char* filename = convert_tochar_darray(file);
    CIniWriter iniWriter(inifile);
    delete[] inifile;
    iniWriter.WriteString("Last File", "filename", filename);
    delete[] filename;
    log.close();
}

//--------------------------------------------------------------------------------------------------//
//--------------------------------SET SNAPHOTS DIR FROM INI FILE------------------------------------//
//--------------------------------------------------------------------------------------------------//

string ExtractFile::snaphotsdir()
{
    char* get_ini_open = convert_tochar_darray(inifilename());
    CIniReader iniReader(get_ini_open);
    delete[] get_ini_open;
    char *snapshotpath_ch = iniReader.ReadString("LOCATION", "Snapshots", "");
    string snapshotpath = string(snapshotpath_ch);
    delete[] snapshotpath_ch;
    return snapshotpath;
}
