#ifndef EXTRACTFILE_H
#define EXTRACTFILE_H
#include <string>
#include <vector>
#include "IndexFileStructure.h"
using namespace std;

class ExtractFile
{
public:
    void CheckForExtractFile(string fileP);
    void SaveJpgs(string fileP);
    void GetNewFilenames();
    void RenameFiles();
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
    void set_completed(int answer, string file);
    int get_counter(string file);
    int get_completed(string file);
    void set_counter(int n, string file);
    string current_time();
    char* get_lastfile();
    void set_lastfile(string file);
};

#endif // EXTRACTFILE_H
