#include <iostream>
#include <fstream>
#include "ExtractFile.h"
using namespace std;

int main()
{
    ExtractFile MyExtractedFile;
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    MyExtractedFile.GetNewFilenames();
    vector<string> MyVectorList = MyExtractedFile.GetVectors();

    if (!MyVectorList.empty())
    {
        for (unsigned int i=0; i < MyVectorList.size(); i++)
        {
            MyExtractedFile.CheckForExtractFile(MyVectorList.at(i));
        }
    }



    return 0;
}
