#include <string>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <fstream>
//#include <windows.h>
#include <cstdlib>
#include <cstring>
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
        log << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    log << "Beginning to search for more files" << endl;
    cout << "Beginning to search for more files" << endl;

    // Needs dirent.h --> For searching through the dir
    DIR *dir;
    struct dirent *ent;

    // Clear the vector of index files
    indexfiles.clear();
    picfiles_dir = basedir() + "\\picfiles\\";

    if ((dir = opendir (picfiles_dir.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            NewFile = ent->d_name;

            log << "Found new file: " << NewFile << endl;

            // Get the extension (picfile or index)
            PosIndex = NewFile.find_last_of('.');
            ext = NewFile.substr(PosIndex, -1);

            if (ext == ".index")
            {
                log << "New index file: " << NewFile << endl;
                // Get just the date part of the filename
                int filedatepos = NewFile.find_first_of('.');
                string filedate = NewFile.substr(0, filedatepos);
                indexfiles.push_back(filedate);
            }
        }
    }
    else
    {
        /* could not open directory */
        log << "Could not open directory picfiles. Aborting!" << endl;
        exit(1);
    }

    /* delete memory*/
    delete dir;
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
        log << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }

    string fullfilename = basedir() + "//picfiles//" + fileP + ".picfile.index";
    cout << "Reading index file: " << fileP << ".picfile.index" << endl;

    infile.open(fullfilename.c_str(), ios::binary | ios::in);
    if (infile.fail())
    {
        log << "Failed to open file: " << fileP << ".picfile.index. Aborting!" << endl;
        exit(1);
    }
    log << "Opened index file " << fileP << ".picfile.index" << endl;

    // section: filename
    // key: Index Counter
    // value: value there (default: 0)
    char* get_ini_open = convert_tochar_darray(inifilename());
    CIniReader iniReader(get_ini_open);
    delete[] get_ini_open;
    char* get_section = convert_tochar_darray(fileP + ".picfile.index");
    int offVal = iniReader.ReadInteger(get_section, "Index Counter", 0);
    delete[] get_section;

    int counter = 0;
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
    infile.close();
    cout << "Closing index file " << fileP << ".picfile.index" << endl;
    log << "Closing index file " << fileP << ".picfile.index" << endl;
    if (counter == offVal)
    {
        log << "This is the end of the file" << endl;
        cout << "This is the end of the file" << endl;

        // Rename the file to prevent re-reading
        string newfullfilename = fullfilename + ".done";
        int result = rename(fullfilename.c_str() , newfullfilename.c_str());
        if ( result == 0 )
        {
            log << fullfilename << " successfully renamed to "
                << newfullfilename << "." << endl;
        }
    }
    else
    {
        picfile_offset = IndexBlockVector.at(offVal).get_fileoffset();
        // Set the counter to the counter offset of the next section
        char* set_ini_open = convert_tochar_darray(inifilename());
        char* set_section = convert_tochar_darray(fileP + ".picfile.index");
        CIniWriter iniWriter(set_ini_open);
        delete[] set_ini_open;
        iniWriter.WriteInteger(set_section, "Index Counter", counter);
        delete[] set_section;

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
        log << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }

    string fullfilename = basedir() + "//picfiles//" + fileP + ".picfile";
    cout << "Reading picfile: " << fileP << ".picfile" << endl;
    log << "Reading picfile: " << fileP << ".picfile" << endl;

    // using details from the index file we now open the picfile
    picfile.open(fullfilename.c_str(), ios::binary | ios::out);
    if (picfile.fail())
    {
        log << "Failed to open " << fileP << ".picfile! Aborting!" << endl;
        exit(1);
    }

    // Proceed to the picfile's offset
    picfile.seekg(picfile_offset, ios::beg);
    int i=0;
    while (!picfile.eof())
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
            log << "Failed to create " << jpgfilename << " file! Aborting!" << endl;
            exit(1);
        }

        // Write memblock array for newly created JPG file
        jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen());
        if (! jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen()))
        {
            log << "Cannot write to JPG file: " << jpgfilename << ". Aborting!" << endl;
            exit(1);
        }

        jpg_file.close();

        i++;
        delete [] memblock;
    }

    cout << "Closing picfile: " << fileP << ".picfile" << endl;
    log << "Closing picfile: " << fileP << ".picfile" << endl;
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
