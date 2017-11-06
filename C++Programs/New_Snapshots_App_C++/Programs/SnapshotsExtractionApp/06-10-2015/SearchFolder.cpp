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

void SearchNewFiles(vector<string>& OldFileList, string& NewIndexFile, string& NewPicFile)
{
    // I want to search though the vector array
    // comparing the vector array to the current files in the directory.
    vector<string> NewFileList;
    string IndexNoExt, PicFileNoExt;
    int IndexFileCount = 0, PicFileCount = 0;
    DIR *dir;
    struct dirent *ent;
    // Get the current directory and place it in the character
    // variable szPath
    TCHAR szPath[MAX_PATH] = _T("");
    GetCurrentDirectory(MAX_PATH, szPath);

    // Implicitly convert szPath to a string
    string base_dir = szPath;
    string picfiles_dir = base_dir + "//picfiles//";
    if ((dir = opendir (picfiles_dir.c_str())) != NULL) {

        while ((ent = readdir (dir)) != NULL) {
            cout << ent->d_name << endl;
            NewFileList.push_back(ent->d_name);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        cout << "Could not open directory picfiles. Aborting!" << endl;
        exit(1);
    }

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
        if (counter == 0)
        {
            int PosIndex, FileNameDatePos;
            string ext;
            string NewFile = NewFileList.at(i);
            cout << "Found new file: " << NewFile << endl;
            Sleep(1000);
            PosIndex = NewFile.find_last_of('.');
            // Because of the .picfile.index
            FileNameDatePos = NewFile.find_first_of('.');
            ext = NewFile.substr(PosIndex, -1);
            if (ext == ".index")
            {
               OldFileList.push_back(NewFile);
               cout << "Added index file to FileList vector." << endl;
               NewIndexFile = NewFileList.at(i);
               cout << "New index file: " << NewIndexFile << endl;

               // 15 is the size of the date
               IndexNoExt = NewFile.substr(0, FileNameDatePos);
               cout << "Index Filename no extension: " << IndexNoExt << endl;
               IndexFileCount++;
               Sleep(1000);
            }
            else if (ext == ".picfile")
            {
               OldFileList.push_back(NewFile);
               cout << "Added Pic file to FileList vector." << endl;
               NewPicFile = NewFileList.at(i);
               cout << "New pic file: " << NewPicFile << endl;
               PicFileNoExt = NewFile.substr(0, FileNameDatePos);
               cout << "Pic Filename no extension: " << PicFileNoExt << endl;
               PicFileCount++;
               Sleep(1000);
            }
            else if (NewFile == "." || NewFile == "..")
            {
                cout << "System file: " << NewFile << endl;
            }
            else
            {
                cout << "Invalid File" << endl;
                if (remove(NewFile.c_str()) != 0)
                    cout << "Error deleting file" << endl;
                else
                    cout << "File successfully removed" << endl;
            }

            if (((IndexFileCount == 1) && (PicFileCount == 1)) && (IndexNoExt == PicFileNoExt))
            {
                cout << "Found a matching pair of files. Exiting the loop." << endl;
                Sleep(1000);
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
