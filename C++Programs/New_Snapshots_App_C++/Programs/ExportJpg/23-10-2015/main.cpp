#include <fstream>
#include "ExtractFile.h"
#include "SystemFunctions.h"
using namespace std;

int main()
{
    SystemFunctions Sys;
    ofstream log;
    log.open("logs.txt", ios::app);
    if (log.fail())
    {
        log << Sys.current_time() << "Failed to open log file. Aborting!" << endl;
        exit(1);
    }
    // isRunning is not working as expected as it still runs, but if I make this true then it does work
    // maybe == false
    if (!Sys.isRunning("ExtractJpg.exe"))
    {
        ExtractFile MyExtractedFile;
        MyExtractedFile.GetNewFilenames();
        vector<string> MyVectorList = MyExtractedFile.GetVectors();

        if (!MyVectorList.empty())
        {
            for (unsigned int i=0; i < MyVectorList.size(); i++)
            {
                MyExtractedFile.CheckForExtractFile(MyVectorList.at(i));
            }
        }
    }
    else
    {
        log << Sys.current_time() << "Process is already running. Aborting!" << endl;
    }

    return 0;
}
