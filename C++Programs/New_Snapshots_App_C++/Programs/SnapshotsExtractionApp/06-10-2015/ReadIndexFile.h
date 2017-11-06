#ifndef READINDEXFILE_H
#define READINDEXFILE_H

#include <iostream>
#include <vector>
#include "Index.h"
using namespace std;

void read_indexfile(istream& infile, vector<StoreFileIndex>& IndexBlockVector, int& counter);

#endif // READINDEXFILE_H
