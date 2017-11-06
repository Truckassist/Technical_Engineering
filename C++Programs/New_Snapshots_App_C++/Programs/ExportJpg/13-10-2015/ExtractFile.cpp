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
void ExtractFile::read_indexfile()
{
    ifstream infile;

    //open the index file for reading
    infile.open(indexfilename.c_str(), ios::binary | ios::in);
    if (infile.fail())
    {
        cout << "Failed to open file: " << indexfilename << ". Aborting!" << endl;
        exit(1);
    }
    cout << "Opened index file" << endl;

    // section: filename
    // key: Index Offset
    // value: whatever is there (default: 0)
    // adding a new section
    CIniReader iniReader("index_arraypos.ini");
    char *y = new char[indexfilename.length() + 1];
    strcpy(y, indexfilename.c_str());
    int offVal = iniReader.ReadInteger(y, "Index Offset", 0);
    cout << offVal << endl;
    delete[] y;
    Sleep(10000);

    // For picfile offset I need the old counter, before it's changed
    // so I get the first value
    start_block = counter;

    // I will start reading the index file from the array
    // position returned from the ini file
    infile.seekg(offVal, ios::beg);

    // This while loop reads from the index offset to the end of the file
    while (!infile.eof())
    {
        infile.read((char*)&IndexBlock, sizeof(IndexBlock));

        // Create memory space for newly created object in the vector
        IndexBlockVector.push_back(IndexBlock);
        cout << IndexBlock.get_fileoffset() << endl;

        // There where around 50 000 files in the GB file.
        cout << "Block " << counter << " read successfully." << endl;
        counter++;
    }

    end_block = counter;

    // To set the new index file offset value in the  ini file
    int arrayposint = counter * sizeof(IndexBlock) + 1;
    CIniWriter iniWriter("index_arraypos.ini");
    char * writable = new char[indexfilename.size() + 1];
    copy(indexfilename.begin(), indexfilename.end(), writable);
    iniWriter.WriteInteger(writable, "Index Offset", arrayposint);
    delete[] writable;

    // Then set the picfile offset to the array position

    picfile_offset = IndexBlockVector.at(start_block).get_fileoffset();
    cout << picfile_offset << endl;
    Sleep(10000);
    infile.close();
}

//--------------------------------------------------------------------------------------------------//
//---------------------------------------SAVE THE JPGS!---------------------------------------------//
//---Take the offset and file lengths from the index file and use it to create a separate jpg file--//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::SaveJpgs()
{
    ifstream picfile;
    ofstream jpg_file;
    int i=0;

    // using details from the index file we now open
    // the picfile
    picfile.open(picfilename.c_str(), ios::binary | ios::out);
    if (picfile.fail())
    {
        cout << "Failed to open " << picfilename << "! Aborting!" << endl;
        exit(1);
    }
    picfile.seekg(picfile_offset, ios::beg);
    while (!picfile.eof())
    {
        char * memblock = new char [IndexBlockVector.at(i).get_filelen()];
        picfile.read(memblock, IndexBlockVector.at(i).get_filelen());
        cout << "Picfile: " << picfilename << " read successfully." << endl;

        // This function works last time I checked
        // It created a jpg file that I can use to write to.
        IndexBlockVector.at(i).create_jpgfile(jpgfilename);

        // Open JPG file
        jpg_file.open(jpgfilename.c_str(), ios::binary | ios::out);
        if (jpg_file.fail())
        {
            cout << "Failed to create " << jpgfilename << " file! Aborting!" << endl;
            exit(1);
        }
        cout << "JPG file: " << jpgfilename << " created successfully." << endl;

        // Write memblock array for newly created JPG file
        jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen());
        if (! jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen()))
        {
            cout << "Cannot write to JPG file: " << jpgfilename << ". Aborting!" << endl;
            exit(1);
        }
        cout << "Write to " << jpgfilename << " completed successfully." << endl;

        jpg_file.close();
        cout << "JPG file: " << jpgfilename << " closed successfully." << endl
            << "--------------------------" << endl;


        i++;
        delete [] memblock;
        // from windows.h in milliseconds
    }
    picfile.close();
}

//--------------------------------------------------------------------------------------------------//
//-----------------------------------SEARCH FOR A NEW PAIR------------------------------------------//
//Find the first new pair (ie index and picfile) and save it to the string variables: indexfilename-//
//--------------------------------------& picfilename.----------------------------------------------//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::GetNewFilenames()
{

    int IndexFileCount = 0, PicFileCount = 0;

    DIR *dir;
    struct dirent *ent;
    // Get the current directory and place it in the character
    // variable szPath
    TCHAR szPath[MAX_PATH] = _T("");
    GetCurrentDirectory(MAX_PATH, szPath);

    // Implicitly convert szPath to a string
    base_dir = szPath;
    picfiles_dir = base_dir + "\\picfiles\\";

    // Create a vector of all the files in the current directory
    // We will use this to compare to the current vector of files
    if ((dir = opendir (picfiles_dir.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            NewFile = ent->d_name;

            // Prints out the file that has no counterpart
            // in the old file vector
            cout << "Found new file: " << NewFile << endl;

            // Get the extension (picfile or index)
            PosIndex = NewFile.find_last_of('.');

            // Use this variable to compare the name of the files
            // To find corresponding picfile and index files
            // This is assuming that the file list is alphabetical
            // Because of the .picfile.index
            FileNameDatePos = NewFile.find_first_of('.');

            // Place extension in a variable
            ext = NewFile.substr(PosIndex, -1);
            if (ext == ".index")
            {
                // Place the name of the index file into a variable
                NewIndexFile_local = NewFile;
                cout << "New index file: " << NewIndexFile_local << endl;

                // Get just the date part of the filename
                IndexNoExt = NewFile.substr(0, FileNameDatePos);
                cout << "Index Filename no extension: " << IndexNoExt << endl;
                IndexFileCount++;
                Sleep(1000);
            }
            else if (ext == ".picfile")
            {
                // Place the name of the picfile into a variable
                NewPicFile_local = NewFile;
                cout << "New pic file: " << NewPicFile_local << endl;

                // Get the date part of the filename
                PicFileNoExt = NewFile.substr(0, FileNameDatePos);
                cout << "Pic Filename no extension: " << PicFileNoExt << endl;
                PicFileCount++;
                Sleep(1000);
            }
            else if (NewFile == "." || NewFile == "..")
            {
                // Take account for system files
                cout << "System file: " << NewFile << endl;
            }
            else if (ext == ".do")
            {
                // intentionally empty
                // Don't delete
            }
            else
            {
                // Delete any files in the picfile directory
                // That are not picfiles or index files
                cout << "Invalid File" << endl;
                if (remove(NewFile.c_str()) != 0)
                    cout << "Error deleting file" << endl;
                else
                    cout << "File successfully removed" << endl;
            }

            // These conditions ensure that once the first matching pair has been found
            // and they have the same date name they will be
            if (((IndexFileCount == 1) && (PicFileCount == 1)) && (IndexNoExt == PicFileNoExt))
            {
                // Now commit these two filenames to the reference variables
                indexfilename = picfiles_dir + NewIndexFile_local;
                picfilename = picfiles_dir + NewPicFile_local;
                // Rename the files to .done to prevent re-reading
                // Place in Search New Files
                RenameFiles();
                //--------initializing values once found a new file---------//
                CIniWriter iniWriter("index_arraypos.ini");
                char * newindex = new char[indexfilename.size() + 1];
                copy(indexfilename.begin(), indexfilename.end(), newindex);
                iniWriter.WriteInteger(newindex, "Index Offset", 0);
                delete[] newindex;
                counter = 0;
                isEndOfFile = false;
                if (!IndexBlockVector.empty())
                    IndexBlockVector.clear();
                //----------------------------------------------------------//
                // Break out of the loop to prevent the searching of further picfile pairs
                // I used this method instead of putting all the new pairs into a vector
                // as I thought that it would be easier.
                cout << "Found a matching pair of files. Exiting the loop." << endl;

                // delete memory
                delete dir;
                delete ent;
                Sleep(3000);

                break;
            }
            else
            {
                cout << "Iterating again" << endl;
            }
        }
        closedir (dir);
    }
    else
    {
        /* could not open directory */
        cout << "Could not open directory picfiles. Aborting!" << endl;
        exit(1);
    }
}

//--------------------------------------------------------------------------------------------------//
//----------------------------------------RENAME FILES----------------------------------------------//
//--------------------------------------------------------------------------------------------------//

void ExtractFile::RenameFiles()
{
    string old_indexfilename = indexfilename;
    string old_picfilename = picfilename;
    string new_indexfilename = indexfilename + ".do";
    string new_picfilename = picfilename + ".do";

    // Rename index file
    int result_index = rename( old_indexfilename.c_str() , new_indexfilename.c_str() );
    if ( result_index == 0 )
    {
        cout << old_indexfilename << " successfully renamed to "
            << new_indexfilename << "." << endl;
        indexfilename = new_indexfilename;
    }

    else
        cout << "Error renaming " << old_indexfilename << endl;

    // Rename picfile
    int result_picfile = rename( old_picfilename.c_str() , new_picfilename.c_str() );
    if ( result_picfile == 0 )
    {
        cout << old_picfilename << " successfully renamed to "
            << new_picfilename << "." << endl;
        picfilename = new_picfilename;
    }

    else
        cout << "Error renaming " << old_picfilename << endl;
}

//--------------------------------------------------------------------------------------------------//

void ExtractFile::EndOfFile()
{
    DIR *dir;
    struct dirent *ent;
    // Get the current directory and place it in the character
    // variable szPath
    TCHAR szPath[MAX_PATH] = _T("");
    GetCurrentDirectory(MAX_PATH, szPath);

    // Implicitly convert szPath to a string
    base_dir = szPath;
    picfiles_dir = base_dir + "\\picfiles\\";

    if ((dir = opendir (picfiles_dir.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            // Get the extension (picfile or index)
            PosIndex = NewFile.find_last_of('.');

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
                    char c;
                    infile.get(c);
                    n++;
                }
                CIniReader iniReader("index_arraypos.ini");
                char *y = new char[indexfilename.length() + 1];
                strcpy(y, indexfilename.c_str());
                int offVal = iniReader.ReadInteger(y, "Index Offset", 0);
                cout << offVal << endl;
                delete[] y;
                if (n == offVal)
                    isEndOfFile = true;
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
