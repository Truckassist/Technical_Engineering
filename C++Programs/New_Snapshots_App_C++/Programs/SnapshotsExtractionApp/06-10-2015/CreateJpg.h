#ifndef CREATEJPG_H
#define CREATEJPG_H
#include <iostream>
#include <vector>
#include <string>
#include "Index.h"
#include "ReadIndexFile.h"
using namespace std;

void create_jpg(istream& picfile, vector<StoreFileIndex>& IndexBlockVector, const int& counter, string picfilename);

#endif // CREATEJPG_H
