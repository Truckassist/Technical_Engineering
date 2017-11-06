#include <string>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <fstream>
//#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <tchar.h>
#include <stdio.h>
#include <sstream>
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
    log << current_time() << "Beginning to search for more files" << endl;
    cout << current_time() << "Beginning to search for more files" << endl;

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

            log << current_time() << "Found new file: " << NewFile << endl;

            // Get the extension (picfile or index)
            PosIndex = NewFile.find_last_of('.');
            ext = NewFile.substr(PosIndex, -1);

            if (ext == ".index")
            {
                int completed = get_completed(NewFile);
                if (completed == 0)
                {
                    log << current_time() << "New index file: " << NewFile << endl;
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
        /* could not open directory */
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

void ExtractFile::CheckForExtractFile(string fileP)
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
    cout << current_time() << "Reading index file: " << fileP << ".picfile.index" << endl;

    infile.open(fullfilename.c_str(), ios::binary | ios::in);

    if (infile.fail())
    {
        log << current_time() << "Failed to open file: " << fileP << ".picfile.index. Aborting!" << endl;
        exit(1);
    }
    log << current_time() << "Opened index file " << fileP << ".picfile.index" << endl;

    long offVal = get_counter(fileP + ".picfile.index");
    log << current_time() << "Begin Index Count: " << offVal << endl;
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
    log << current_time() << "End Index Count: " << counter << endl;
    infile.close();
    cout << current_time() << "Closing index file " << fileP << ".picfile.index" << endl;
    log << current_time() << "Closing index file " << fileP << ".picfile.index" << endl;
    if (counter == offVal)
    {
        log << current_time() << "This is the end of the file" << endl;
        cout << current_time() << "This is the end of the file" << endl;

        // Set file to done in ini file
        if ((fileP + ".picfile.index") != get_lastfile())
            set_completed(1, fileP + ".picfile.index");
    }
    else
    {
        picfile_offset = IndexBlockVector.at(0).get_fileoffset();

        // Set the counter to the counter offset of the next section
        set_completed(0, fileP + ".picfile.index");
        SaveJpgs(fileP);
    }

    log.close();
}

//--------------------------------------------------------------------------------------------------//
//---------------------------------------SAVE THE JPGS!---------------------------------------------//
//---Take the offset and file lengths from the index file and use it to create a separate jpg file--//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::SaveJpgs(string fileP)
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
    cout << current_time() << "Reading picfile: " << fileP << ".picfile" << endl;
    log << current_time() << "Reading picfile: " << fileP << ".picfile" << endl;

    // Get the previous turns file count
    int oldfilecount = get_counter(fileP + ".picfile.index");

    // using details from the index file we now open the picfile
    picfile.open(fullfilename.c_str(), ios::binary | ios::out);
    if (picfile.fail())
    {
        log << current_time() << "Failed to open " << fileP << ".picfile! Aborting!" << endl;
        exit(1);
    }

    // Proceed to the picfile's offset
    picfile.seekg(picfile_offset, ios::beg);
    int i=0;
    while (i < IndexBlockVector.size())
    {
        // Now read a block of the picfile using the filelen specified in index file
        char * memblock = new char [IndexBlockVector.at(i).get_filelen()];
        picfile.read(memblock, IndexBlockVector.at(i).get_filelen());

        // Create a JPG file to write to
        IndexBlockVector.at(i).create_jpgfile(jpgfilename);
        log << current_time() << jpgfilename << " file created." << endl;
        // Open JPG file
        jpg_file.open(jpgfilename.c_str(), ios::binary | ios::out);
        if (jpg_file.fail())
        {
            log << current_time() << "Failed to create " << jpgfilename << " file! Aborting!" << endl;
            exit(1);
        }

        // Write memblock array for newly created JPG file
        jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen());
        if (! jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen()))
        {
            log << current_time() << "Cannot write to JPG file: " << jpgfilename << ". Aborting!" << endl;
            exit(1);
        }

        jpg_file.close();
        if (i % 100 == 0)
        {
            set_counter(oldfilecount + i, fileP + ".picfile.index");
        }
        cout << '\r' << oldfilecount+i;
        i++;
        delete [] memblock;
    }
    set_counter(oldfilecount + i, fileP + ".picfile.index");
    cout << endl;
    cout << current_time() << "Closing picfile: " << fileP << ".picfile" << endl;
    log << current_time() << "Closing picfile: " << fileP << ".picfile" << endl;
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
//------------------------------------

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

void ExtractFile::set_completed(int answer, string file)
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
}

int ExtractFile::get_counter(string file)
{
    char* inifile = convert_tochar_darray(inifilename());
    CIniReader iniReader(inifile);
    delete[] inifile;
    char* section = convert_tochar_darray(file);
    int intVal = iniReader.ReadInteger(section, "Counter", 0);
    delete[] section;
    return intVal;
}

int ExtractFile::get_completed(string file)
{
    char* inifile = convert_tochar_darray(inifilename());
    CIniReader iniReader(inifile);
    delete[] inifile;
    char* section = convert_tochar_darray(file);
    int completed = iniReader.ReadInteger(section, "Completed", 0);
    delete[] section;
    return completed;
}

void ExtractFile::set_counter(int n, string file)
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
}

string ExtractFile::current_time()
{
    /*time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer, 80, "[%F %T] ", timeinfo);
    return buffer;*/
    time_t currentTime;
    struct tm *localTime;

    time( &currentTime );                   // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

    int dd   = localTime->tm_mday;
    int mm   = localTime->tm_mon + 1;
    int yyyy = localTime->tm_year + 1900;
    int HH   = localTime->tm_hour;
    int ii   = localTime->tm_min;
    int ss   = localTime->tm_sec;
    string year   = static_cast<ostringstream*>( &(ostringstream() << yyyy) )->str();
    string month  = static_cast<ostringstream*>( &(ostringstream() << mm) )->str();
    string day    = static_cast<ostringstream*>( &(ostringstream() << dd) )->str();
    string hour   = static_cast<ostringstream*>( &(ostringstream() << HH) )->str();
    string minute = static_cast<ostringstream*>( &(ostringstream() << ii) )->str();
    string second = static_cast<ostringstream*>( &(ostringstream() << ss) )->str();

    string Timestamp = "[" + year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second + "] ";
    return Timestamp;
}

char* ExtractFile::get_lastfile()
{
    char* inifile = convert_tochar_darray(inifilename());
    CIniReader iniReader(inifile);
    delete[] inifile;
    char* lastfile = iniReader.ReadString("Last File", "filename", "");
    return lastfile;
}

void ExtractFile::set_lastfile(string file)
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

}
