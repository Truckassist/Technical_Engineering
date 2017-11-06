#ifndef EXTRACTFILE_H
#define EXTRACTFILE_H
#include <string>
#include <vector>
#include "IndexFileStructure.h"
using namespace std;

class ExtractFile
{
public:
    // Reads index file and extract the JPGs using that info
    void CheckForExtractFile(const string fileP);
    // Searches for index files that are not finished yet
    void GetNewFilenames();
    // returns a vector of index files
    vector<string> GetVectors();
    string snaphotsdir();
    // Get current time for log file
    string current_time();
private:
    // Takes the info from index file and creates individual JPGs
    void SaveJpgs(const string fileP);
    // Different file paths
    vector<string> indexfiles;
    string basedir(), picfiledir(), jpgfilename, inifilename();
    // vector of the index blocks
    vector<StoreFileIndex> IndexBlockVector;
    // object of StoreFileIndex
    StoreFileIndex IndexBlock;
    // For searching for new files
    int PosIndex, FileNameDatePos;
    string ext, NewFile;
    // Used for counting index blocks
    long counter;
    // Where to start reading in picfile
    long picfile_offset;
    // For use with ini file
    char* convert_tochar_darray(const string filename);
    void set_completed(const int answer, const string file);
    int get_counter(const string file);
    int get_completed(const string file);
    void set_counter(const int n, const string file);
    char* get_lastfile();
    void set_lastfile(const string file);

};

#endif // EXTRACTFILE_H
