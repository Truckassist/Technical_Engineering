#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <windows.h>
#include "Index.h"
using namespace std;

int main()
{
    int counter = 0;
    //StoreFileIndex IndexBlock;
    // We make a vector of StoreFileIndex that will
    // store the individual object entries
    vector<StoreFileIndex> IndexBlockVector;

    char * memblock;
    ifstream infile_index, picfile;
    ofstream jpg_file;
    string indexfilename = "picfiles/test.index";
    string picfilename = "picfiles/test.picfile";

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
    while (!infile_index.eof())
    {
        StoreFileIndex IndexBlock;

        infile_index.read((char*)&IndexBlock, sizeof(IndexBlock));
        /*if (! infile_index.read((char*)&IndexBlock[counter], sizeof(IndexBlock[counter])))
        {
            cout << "Cannot read index file " << indexfilename << ". Aborting!";
            exit(1);
        }*/

        counter++;

        // Create memory space for newly created object in the vector
        IndexBlockVector.push_back(IndexBlock);
        // There where around 50 000 files in the GB file.
        cout << "Block " << counter << " read successfully." << endl;
        Sleep(5);
    }
    infile_index.close();
    cout << "Index file: " << indexfilename << " closed successfully." << endl
        << "------------------" << endl << endl;

    cout << "---------------" << endl
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

    for (int i=0; i < counter-1; i++)
    {
        cout << "----------------------------" << endl;
        string filename;
        IndexBlockVector.at(i).create_jpg(filename);

        // Open JPG file
        jpg_file.open(filename.c_str(), ios::binary | ios::out);
        if (jpg_file.fail())
        {
            cout << "Failed to create jpg file! Aborting!" << endl;
            exit(1);
        }
        cout << "JPG file: " << filename << " created successfully." << endl;

        // Create a dynamic char array the size of the jpg file
        memblock = new char [IndexBlockVector.at(i).get_filelen()];

        // Copy JPG block into memblock array
        picfile.read(memblock, IndexBlockVector.at(i).get_filelen());
        /*if (! picfile.read(memblock, IndexBlock[i].get_filelen()))
        {
            cout << "Cannot read picfile " << picfile << ". Aborting!";
            exit(1);
        }*/
        cout << "Picfile: " << picfilename << " read successfully." << endl;

        // Write memblock array for newly created JPG file
        jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen());
        if (! jpg_file.write(memblock, IndexBlockVector.at(i).get_filelen()))
        {
            cout << "Cannot write to JPG file: " << filename << ". Aborting!" << endl;
            exit(1);
        }
        cout << "Write to " << filename << " completed successfully." << endl;

        jpg_file.close();
        cout << "JPG file: " << filename << " closed successfully." << endl
            << "----------------------------" << endl << endl;
        delete [] memblock;
        // from windows.h in milliseconds
        Sleep(50);
    }
    picfile.close();
    cout << "Picfile: " << picfilename << " closed successfully." << endl << endl
        << "--------------" << endl
        << "END OF PROCESS" << endl
        << "--------------" << endl;



    return 0;
}
