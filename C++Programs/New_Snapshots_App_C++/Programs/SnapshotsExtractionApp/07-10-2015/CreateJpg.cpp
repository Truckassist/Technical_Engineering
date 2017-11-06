#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include "CreateJpg.h"
#include "Index.h"
using namespace std;

void create_jpg(istream& picfile, vector<StoreFileIndex>& indices, const int& counter, string picfilename)
{
    ofstream jpg_file;
    for (int i=0; i < counter-1; i++)
    {
        cout << "--------------------------" << endl;
        int percentage = ((i + 1)*100) / counter;

        cout << "Percentage completed: " << setw(3) << percentage << "%" << endl;
        cout << "--------------------------" << endl;
        string filename;
        indices.at(i).create_jpgfile(filename);

        // Open JPG file
        jpg_file.open(filename.c_str(), ios::binary | ios::out);
        if (jpg_file.fail())
        {
            cout << "Failed to create jpg file! Aborting!" << endl;
            exit(1);
        }
        cout << "JPG file: " << filename << " created successfully." << endl;

        // Create a dynamic char array the size of the jpg file
        char * memblock = new char [indices.at(i).get_filelen()];

        // Copy JPG block into memblock array
        picfile.read(memblock, indices.at(i).get_filelen());
        cout << "Picfile: " << picfilename << " read successfully." << endl;

        // Write memblock array for newly created JPG file
        jpg_file.write(memblock, indices.at(i).get_filelen());
        if (! jpg_file.write(memblock, indices.at(i).get_filelen()))
        {
            cout << "Cannot write to JPG file: " << filename << ". Aborting!" << endl;
            exit(1);
        }
        cout << "Write to " << filename << " completed successfully." << endl;

        jpg_file.close();
        cout << "JPG file: " << filename << " closed successfully." << endl
            << "--------------------------" << endl;
        delete [] memblock;
        // from windows.h in milliseconds
    }
    cout << "--------------------------" << endl;
    cout << "Percentage completed: 100%" << endl;
    cout << "--------------------------" << endl << endl;
}

