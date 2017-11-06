#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Index.h"
using namespace std;

int main()
{
    _tagStoreFileIndex IndexBlock[500];
    int i=0;
    ifstream infile;

    infile.open("snaps/20150916-131346.picfile.index");
    if (infile.fail())
    {
        cout << "Failed to open file! Aborting!" << endl;
        exit(1);
    }

    while (!infile.eof())
    {
        infile.read((char*)&IndexBlock[i], sizeof(IndexBlock[i]));
        cout << IndexBlock[i].nFileOffst << endl;
        cout << i << endl;
        i++;
    }

    infile.close();

    return 0;
}
