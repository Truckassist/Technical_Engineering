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
    void CheckForExtractFile(string fileP);
    void SaveJpgs(string fileP);
    void GetNewFilenames();
    void RenameFiles();
    void EndOfFile();
    bool FoundNewFiles();
    bool isEnd();
    vector<string> GetVectors();
private:
    vector<string> indexfiles;
    string jpgfilename;
    string inifilename();
    int picfile_offset;
    int start_block;
    int end_block;
    //vector<int> jpg_filelen;
    vector<StoreFileIndex> IndexBlockVector;
    //vector<string> filelist;
    StoreFileIndex IndexBlock;
    // For searching for new files
    string picfiles_dir;
    string IndexNoExt;
    string PicFileNoExt;

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
    bool AreNewFiles;
    // The ini file instance
    //CSimpleIniA ini;
    string basedir();
    char* convert_tochar_darray(const string filename);
};

#endif // EXTRACTFILE_H
