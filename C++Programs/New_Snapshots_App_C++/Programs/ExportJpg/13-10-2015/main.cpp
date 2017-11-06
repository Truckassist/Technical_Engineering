#include <iostream>
#include "ExtractFile.h"

using namespace std;

int main()
{
    ExtractFile MyExtractedFile;

    while (true)
    {
        // inside SetNewFiles there will be a function that check whether there are new files
        // which will only count when this program starts
        MyExtractedFile.GetNewFilenames();
        while (!MyExtractedFile.isEnd())
        {
            MyExtractedFile.read_indexfile();
            MyExtractedFile.SaveJpgs();
            Sleep(20000);
            MyExtractedFile.EndOfFile();
        }
    }

    return 0;
}
