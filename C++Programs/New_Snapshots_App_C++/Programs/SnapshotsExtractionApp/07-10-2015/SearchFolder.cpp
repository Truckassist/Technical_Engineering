#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <tchar.h>
#include <stdio.h>
#include "SearchFolder.h"
using namespace std;

void SearchNewFiles(vector<string>& OldFileList, string& NewIndexFile, string& NewPicFile, bool& AreNewFiles)
{
    // I want to search though the vector array
    // comparing the vector array to the current files in the directory.
    vector<string> NewFileList;
    // I have used NewIndexFile_local and NewPicFile_local to store
    // the names of the new files before committing them to the reference variables
    string IndexNoExt, PicFileNoExt, NewIndexFile_local, NewPicFile_local;
    int IndexFileCount = 0, PicFileCount = 0;

    DIR *dir;
    struct dirent *ent;
    // Get the current directory and place it in the character
    // variable szPath
    TCHAR szPath[MAX_PATH] = _T("");
    GetCurrentDirectory(MAX_PATH, szPath);

    // Implicitly convert szPath to a string
    string base_dir = szPath;
    string picfiles_dir = base_dir + "\\picfiles\\";

    // Create a vector of all the files in the current directory
    // We will use this to compare to the current vector of files
    if ((dir = opendir (picfiles_dir.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            cout << ent->d_name << endl;
            NewFileList.push_back(ent->d_name);
        }
        closedir (dir);
    }
    else
    {
        /* could not open directory */
        cout << "Could not open directory picfiles. Aborting!" << endl;
        exit(1);
    }

    // Now loop through all the new files and check with the old file list
    for (unsigned int i=0; i < NewFileList.size(); i++)
    {
        int counter = 0;
        for (unsigned int j=0; j < OldFileList.size(); j++)
        {
            if (OldFileList.at(j) == NewFileList.at(i))
            {
                counter++;
                cout << counter << endl;
            }
        }
        // If there is an entry where there is no match
        // enter this if statement
        if (counter == 0)
        {
            int PosIndex, FileNameDatePos;
            string ext;
            string NewFile = NewFileList.at(i);

            // Prints out the file that has no counterpart
            // in the old file vector
            cout << "Found new file: " << NewFile << endl;
            Sleep(1000);

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
                NewIndexFile_local = NewFileList.at(i);
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
                NewPicFile_local = NewFileList.at(i);
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
            else if (NewFile == ".done")
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
                NewIndexFile = picfiles_dir + NewIndexFile_local;
                NewPicFile = picfiles_dir + NewPicFile_local;
                // Now add the filenames to the original file vector so that
                //they won't be used again
                OldFileList.push_back(NewIndexFile);
                cout << "Added index file to FileList vector." << endl;
                OldFileList.push_back(NewPicFile);
                cout << "Added Pic file to FileList vector." << endl;
                // AreNewFiles must be true now so that the loop in the main program
                // will loop through the new file without waiting for 1 hour.
                AreNewFiles = true;
                Sleep(1000);
                // Break out of the loop to prevent the searching of further picfile pairs
                // I used this method instead of putting all the new pairs into a vector
                // as I thought that it would be easier.
                cout << "Found a matching pair of files. Exiting the loop." << endl;
                break;
            }
            else
                cout << "Iterating again" << endl;
        }
    }
    cout << NewIndexFile << endl;
    cout << NewPicFile << endl;
    // delete memory
    delete dir;
    delete ent;
}
