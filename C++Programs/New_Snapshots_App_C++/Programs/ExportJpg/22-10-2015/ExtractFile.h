#ifndef EXTRACTFILE_H
#define EXTRACTFILE_H
#include <string>
#include <vector>
#include "IndexFileStructure.h"
using namespace std;

class ExtractFile
{
public:
    void CheckForExtractFile(const string fileP);
    void SaveJpgs(const string fileP);
    void GetNewFilenames();
    vector<string> GetVectors();
private:
    vector<string> indexfiles;
    string basedir();
    string picfiledir();
    string jpgfilename;
    string inifilename();
    vector<StoreFileIndex> IndexBlockVector;
    StoreFileIndex IndexBlock;
    // For searching for new files
    int PosIndex, FileNameDatePos;
    long picfile_offset;
    string ext;
    string NewFile;
    long counter;
    char* convert_tochar_darray(const string filename);
    void set_completed(const int answer, const string file);
    int get_counter(const string file);
    int get_completed(const string file);
    void set_counter(const int n, const string file);
    string current_time();
    char* get_lastfile();
    void set_lastfile(const string file);
};

#endif // EXTRACTFILE_H
