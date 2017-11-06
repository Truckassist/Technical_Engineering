#include <string>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <windows.h>
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
//------------------------------------READ THE INDEX FILE-------------------------------------------//
//Read the index file for the offset of the picfile and then the length of the individual snapshots-//
//------------------------------------in the binary file.-------------------------------------------//
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
    //open the index file for reading
    infile.open(fullfilename.c_str(), ios::binary | ios::in);
    if (infile.fail())
    {
        log << "Failed to open file: " << fileP << ".picfile.index. Aborting!" << endl;
        exit(1);
    }
    log << "Opened index file " << fileP << ".picfile.index" << endl;

    // section: filename
    // key: Index Offset
    // value: whatever is there (default: 0)
    // adding a new section
    char* get_ini_open = convert_tochar_darray(inifilename());
    CIniReader iniReader(get_ini_open);
    delete[] get_ini_open;
    char* get_section = convert_tochar_darray(fileP + ".picfile.index");
    int offVal = iniReader.ReadInteger(get_section, "Index Counter", 0);
    delete[] get_section;

    // For picfile offset I need the old counter, before it's changed
    // so I get the first value
    int counter = 0;

    // I will start reading the index file from the array
    // position returned from the ini file
    //infile.tellg()(offVal, ios::beg);

    // This while loop reads from the index offset to the end of the file
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
        isEndOfFile = true;
        log << "This is the end of the file" << endl;
        cout << "This is the end of the file" << endl;

        string newfullfilename = fullfilename + ".done";
        int result = rename( fullfilename.c_str() , newfullfilename.c_str() );
        if ( result == 0 )
        {
            log << fullfilename << " successfully renamed to "
                << newfullfilename << "." << endl;
        }
    }
    else
    {
        log << "ok, reading picfiles" << endl;
        // Set the counter to counter to the next section
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
    int i=0;
    string fullfilename = basedir() + "//picfiles//" + fileP + ".picfile";
    cout << "Reading picfile: " << fileP << ".picfile" << endl;
    log << "Reading picfile: " << fileP << ".picfile" << endl;
    // using details from the index file we now open
    // the picfile
    picfile.open(fullfilename.c_str(), ios::binary | ios::out);
    if (picfile.fail())
    {
        log << "Failed to open " << fileP << ".picfile! Aborting!" << endl;
        exit(1);
    }
    picfile.seekg(picfile_offset, ios::beg);
    while (!picfile.eof())
    {
        char * memblock = new char [IndexBlockVector.at(i).get_filelen()];
        picfile.read(memblock, IndexBlockVector.at(i).get_filelen());

        // This function works last time I checked
        // It created a jpg file that I can use to write to.
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
        // from windows.h in milliseconds
    }
    log.close();
    cout << "Closing picfile: " << fileP << ".picfile" << endl;
    log << "Closing picfile: " << fileP << ".picfile" << endl;
    picfile.close();
}

//--------------------------------------------------------------------------------------------------//
//-----------------------------------SEARCH FOR A NEW PAIR------------------------------------------//
//Find the first new pair (ie index and picfile) and save it to the string variables: indexfilename-//
//--------------------------------------& picfilename.----------------------------------------------//
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

    DIR *dir;
    struct dirent *ent;
    indexfiles.clear();
    string NewIndexFile_local;
    string NewPicFile_local;
    picfiles_dir = basedir() + "\\picfiles\\";
    AreNewFiles = false;
    // Create a vector of all the files in the current directory
    // We will use this to compare to the current vector of files
    if ((dir = opendir (picfiles_dir.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            NewFile = ent->d_name;

            // Prints out the file that has no counterpart
            // in the old file vector
            log << "Found new file: " << NewFile << endl;

            // Get the extension (picfile or index)
            PosIndex = NewFile.find_last_of('.');

            // Place extension in a variable
            ext = NewFile.substr(PosIndex, -1);
            if (ext == ".index")
            {
                log << "New index file: " << NewFile << endl;
                int filedatepos = NewFile.find_first_of('.');
                // Get just the date part of the filename
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
//----------------------------------------RENAME FILES----------------------------------------------//
//--------------------------------------------------------------------------------------------------//
/*
void ExtractFile::RenameFiles()
{
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    string old_indexfilename = indexfilename;
    string old_picfilename = picfilename;
    string new_indexfilename = indexfilename + ".do";
    string new_picfilename = picfilename + ".do";

    // Rename index file
    int result_index = rename( old_indexfilename.c_str() , new_indexfilename.c_str() );
    if ( result_index == 0 )
    {
        log << old_indexfilename << " successfully renamed to "
            << new_indexfilename << "." << endl;
        indexfilename = new_indexfilename;
    }

    else
        log << "Error renaming " << old_indexfilename << endl;

    // Rename picfile
    int result_picfile = rename( old_picfilename.c_str() , new_picfilename.c_str() );
    if ( result_picfile == 0 )
    {
        log << old_picfilename << " successfully renamed to "
            << new_picfilename << "." << endl;
        picfilename = new_picfilename;
    }

    else
        log << "Error renaming " << old_picfilename << endl;
    log.close();
}

//--------------------------------------------------------------------------------------------------//

void ExtractFile::EndOfFile()
{
    cout << "End of file function" << endl;
    DIR *dir;
    struct dirent *ent;

    picfiles_dir = basedir() + "\\picfiles\\";

    if ((dir = opendir (picfiles_dir.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            // Get the extension (picfile or index)
            PosIndex = NewFile.find_last_of('.');
            FileNameDatePos = NewFile.find_first_of('.');
            // Place extension in a variable
            ext = NewFile.substr(PosIndex, -1);
            if (ext == ".index")
            {
                // Get just the date part of the filename
                IndexNoExt = NewFile.substr(0, FileNameDatePos);
                cout << "Index Filename no extension: " << IndexNoExt << endl;
                Sleep(1000);
            }
            else if (ext == ".picfile")
            {
                // Get the date part of the filename
                PicFileNoExt = NewFile.substr(0, FileNameDatePos);
                cout << "Pic Filename no extension: " << PicFileNoExt << endl;
                Sleep(1000);
            }
            else
                cout << "not a pic or indexfile" << endl;

            // These conditions ensure that once the first matching pair has been found
            // and they have the same date name they will be
            if ((IndexNoExt == PicFileNoExt))
            {
                ifstream infile;
                int n = 0;
                infile.open(indexfilename.c_str());
                if (infile.fail())
                {
                    cout << "Opening index file for counting characters failed. Aborting!" << endl;
                    exit(1);
                }
                while(!infile.eof())
                {
                    infile.read((char*)&IndexBlock, sizeof(IndexBlock));
                    n++;
                }
                char* eof_ini_open = convert_tochar_darray(inifilename());
                CIniReader iniReader(eof_ini_open);
                delete[] eof_ini_open;
                char* eof_section = convert_tochar_darray(indexfilename);
                int offVal = iniReader.ReadInteger(eof_section, "Index Counter", 0);
                delete[] eof_section;

                cout << "n: " << n << " offval: "<< offVal << endl;
                Sleep(10000);
                if (n == offVal)
                {
                    cout << "Is end of file" << endl;
                    isEndOfFile = true;
                    Sleep(8000);
                }
                cout << "No new files" << endl;
                infile.close();
            }
            else
            {
                cout << "Iterating again" << endl;
            }
        }
        closedir (dir);
        isEndOfFile = false;
    }
}
//--------------------------------------------------------------------------------------------------------//
bool ExtractFile::isEnd()
{
    return isEndOfFile;
}
//---------------------------------------------------------------------------------------------------------//
*/
string ExtractFile::basedir()
{
    // Get the current directory and place it in the character
    // variable szPath
    TCHAR szPath[MAX_PATH] = _T("");
    GetCurrentDirectory(MAX_PATH, szPath);

    // Implicitly convert szPath to a string
    string base_dir = szPath;
    return base_dir;
}
//----------------------------------------------------------------------------------------------------------//
char* ExtractFile::convert_tochar_darray(const string filename)
{
    char* ch = new char[filename.length() + 1];
    strcpy(ch, filename.c_str());
    return ch;
}
//----------------------------------------------------------------------------------------------------------//
string ExtractFile::inifilename()
{
    return basedir() + "\\index_arraypos.ini";
}

//----------------------------------------------------------------------------------------------------------//
/*
bool ExtractFile::FoundNewFiles()
{
    return AreNewFiles;
}
//----------------------------------------------------------------------------------------------------------//
void ExtractFile::RenameErrorFiles(string filename)
{
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    string newfilename = filename + ".error";

    // Rename file
    int result = rename( filename.c_str() , newfilename.c_str() );
    if ( result == 0 )
    {
        log << filename << " successfully renamed to "
            << newfilename << "." << endl;
    }
    else
        log << "Error renaming " << filename << endl;

    log.close();
}
*/
//----------------------------------------------------------------------------------------------------------//

vector<string> ExtractFile::GetVectors()
{
    return indexfiles;
}
