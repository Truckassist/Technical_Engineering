#ifndef EXTRACTFILE_H
#define EXTRACTFILE_H
#include <string>
#include <vector>
//#include "simpleini-master/SimpleIni.h"
#include "IndexFileStructure.h"
using namespace std;

class ExtractFile
{
public:
    ExtractFile() : counter(0){};
    void read_indexfile();
    void SaveJpgs();
    void GetNewFilenames();
    void RenameFiles();
    void EndOfFile();
    bool isEnd();
private:
    string indexfilename;
    string picfilename;
    string jpgfilename;
    int picfile_offset;
    int start_block;
    int end_block;
    int counter;
    //vector<int> jpg_filelen;
    vector<StoreFileIndex> IndexBlockVector;
    //vector<string> filelist;
    StoreFileIndex IndexBlock;
    // For searching for new files
    string base_dir;
    string picfiles_dir;
    string IndexNoExt;
    string PicFileNoExt;
    string NewIndexFile_local;
    string NewPicFile_local;
    int PosIndex, FileNameDatePos;
    string ext;
    string NewFile;
    // For renaming the file
    string old_indexfilename;
    string old_picfilename;
    string new_indexfile;
    string new_picfile;
    // For the eof condition
    bool isEndOfFile;
    // The ini file instance
    //CSimpleIniA ini;
};

#endif // EXTRACTFILE_H
