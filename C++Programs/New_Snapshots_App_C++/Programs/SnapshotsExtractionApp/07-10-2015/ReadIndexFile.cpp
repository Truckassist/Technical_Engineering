#include <iostream>
#include <vector>
#include "ReadIndexFile.h"
#include "Index.h"
using namespace std;

void read_indexfile(istream& infile, vector<StoreFileIndex>& IndexBlockVector, int& counter)
{
    while (!infile.eof())
    {
        StoreFileIndex IndexBlock;

        infile.read((char*)&IndexBlock, sizeof(IndexBlock));
        counter++;

        // Create memory space for newly created object in the vector
        IndexBlockVector.push_back(IndexBlock);
        //string channel_info = "0" + IndexBlock.get_channel();
        // There where around 50 000 files in the GB file.
        cout << "Block " << counter << " read successfully." << endl;
    }
}
