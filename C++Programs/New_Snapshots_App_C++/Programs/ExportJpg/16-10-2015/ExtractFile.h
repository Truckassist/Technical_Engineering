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
    string jpgfilename;
    string inifilename();
    vector<StoreFileIndex> IndexBlockVector;
    StoreFileIndex IndexBlock;
    // For searching for new files
    int PosIndex, FileNameDatePos, picfile_offset;
    string ext;
    string NewFile;
    string picfiles_dir;
    string basedir();
    char* convert_tochar_darray(const string filename);
};

#endif // EXTRACTFILE_H
