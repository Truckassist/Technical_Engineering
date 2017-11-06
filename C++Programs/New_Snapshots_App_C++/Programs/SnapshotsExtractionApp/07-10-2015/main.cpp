#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include "Index.h"
#include "ReadIndexFile.h"
#include "CreateJpg.h"
#include "SearchFolder.h"
#include "RenameFiles.h"
using namespace std;

int main()
{
    int counter = 0;
    // We make a vector of StoreFileIndex that will
    // store the individual object entries
    vector<StoreFileIndex> indices;
    vector<string> FileList;
    ifstream infile_index, picfile;
    ofstream jpg_file;
    string indexfilename;
    string picfilename;
    bool AreNewFiles = false;

    // Searches the directory for new files
    // Returns a matching index and picfile
    SearchNewFiles(FileList, indexfilename, picfilename, AreNewFiles);
    cout << '\a' << endl;
    while (true)
    {

        cout << "------------------" << endl
            << "READING INDEX FILE" << endl
            << "------------------" << endl;

        // Open index file
        infile_index.open(indexfilename.c_str(), ios::binary | ios::in);
        if (infile_index.fail())
        {
            cout << "Failed to open index file: " << indexfilename << ". Aborting!" << endl;
            exit(1);
        }
        cout << "Index file: " << indexfilename << " opened successfully." << endl;

        // Read index file and place each jpg file block into an array of
        // StoreFileIndex objects
        read_indexfile(infile_index, indices, counter);

        infile_index.close();
        cout << "Index file: " << indexfilename << " closed successfully." << endl
            << "------------------" << endl << endl
            << "---------------" << endl
            << "OPENING PICFILE" << endl
            << "---------------" << endl;

        // open picfile
        picfile.open(picfilename.c_str(), ios::binary | ios::in);
        if (picfile.fail())
        {
            cout << "Failed to open pic file: " << picfilename << ". Aborting!" << endl;
            exit(1);
        }
        cout << "Picfile: " << picfilename << " opened successfully." << endl << endl;

        cout << "--------------------" << endl
            << "WRITING TO JPG FILES" << endl
            << "--------------------" << endl << endl;

        // Opens pic file and uses the indeices vector
        // to parse the jpg block and create a jpg file
        create_jpg(picfile, indices, counter, picfilename);

        picfile.close();
        cout << "Picfile: " << picfilename << " closed successfully." << endl << endl
            << "--------------" << endl
            << "END OF PROCESS" << endl
            << "--------------" << endl;

        // For the naming of used files
        string indexfilename_old = indexfilename;
        string picfilename_old = picfilename;
        cout << '\a' << endl;
        // Search for new files again
        // These will  be files that are not in the FileList vector
        SearchNewFiles(FileList, indexfilename, picfilename, AreNewFiles);
        // using the AreNewFiles boolean
        // if there are no new files then you can sleep the program,
        // but if there are new files then you can continue reading the new file
        if (AreNewFiles == false)
        {
            cout << "No new files." << endl;
            Sleep(3600000);
        }
        else
            RenameFiles(indexfilename_old, picfilename_old);

    }
    return 0;

}
