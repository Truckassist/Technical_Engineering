#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "Index.h"
using namespace std;

int main()
{
    StoreFileIndex IndexBlock[500];
    int counter = 0;
    char * memblock;
    ifstream infile_index, picfile;
    ofstream jpg_file;
    string indexfilename = "picfiles/20150916-131346.picfile.index";
    string picfilename = "picfiles/20150916-131346.picfile";

    cout << "------------------" << endl << "READING INDEX FILE" << endl
        << "------------------" << endl;
    infile_index.open(indexfilename.c_str(), ios::binary | ios::in);
    if (infile_index.fail())
    {
        cout << "Failed to open index file: " << indexfilename << ". Aborting!" << endl;
        exit(1);
    }
    cout << "Index file: " << indexfilename << " opened successfully." << endl;

    while (!infile_index.eof())
    {
        infile_index.read((char*)&IndexBlock[counter], sizeof(IndexBlock[counter]));
        /*if (! infile_index.read((char*)&IndexBlock[counter], sizeof(IndexBlock[counter])))
        {
            cout << "Cannot read index file " << indexfilename << ". Aborting!";
            exit(1);
        }*/
        counter++;
    }
    infile_index.close();
    cout << "Index file: " << indexfilename << " closed successfully." << endl
        << "------------------" << endl << endl;

    cout << "---------------" << endl << "OPENING PICFILE" << endl <<
        "---------------" << endl;
    picfile.open(picfilename.c_str(), ios::binary | ios::in);
    if (picfile.fail())
    {
        cout << "Failed to open pic file: " << picfilename << ". Aborting!" << endl;
        exit(1);
    }
    cout << "Picfile: " << picfilename << " opened successfully." << endl << endl;

    cout << "--------------------" << endl << "WRITING TO JPG FILES" << endl <<
        "--------------------" << endl << endl;

    for (int i=0; i < counter-1; i++)
    {
        cout << "----------------------------" << endl;
        string filename;
        IndexBlock[i].create_jpg(filename);

        jpg_file.open(filename.c_str(), ios::binary | ios::out);
        if (jpg_file.fail())
        {
            cout << "Failed to create jpg file! Aborting!" << endl;
            exit(1);
        }
        cout << "JPG file: " << filename << " created successfully." << endl;

        memblock = new char [IndexBlock[i].get_filelen()];

        picfile.read(memblock, IndexBlock[i].get_filelen());
        /*if (! picfile.read(memblock, IndexBlock[i].get_filelen()))
        {
            cout << "Cannot read picfile " << picfile << ". Aborting!";
            exit(1);
        }*/
        cout << "Picfile: " << picfilename << " read successfully." << endl;

        jpg_file.write(memblock, IndexBlock[i].get_filelen());
        if (! jpg_file.write(memblock, IndexBlock[i].get_filelen()))
        {
            cout << "Cannot write to JPG file: " << filename << ". Aborting!" << endl;
            exit(1);
        }
        cout << "Write to " << filename << " completed successfully." << endl;

        jpg_file.close();
        cout << "JPG file: " << filename << " closed successfully." << endl
            << "----------------------------" << endl << endl;
    }
    picfile.close();
    cout << "Picfile: " << picfilename << " closed successfully." << endl << endl
        << "---" << endl << "END" << endl << "---" << endl;

    delete [] memblock;

    return 0;
}
