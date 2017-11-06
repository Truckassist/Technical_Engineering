#include <string>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include "RenameFiles.h"
using namespace std;

void RenameFiles(string indexfilename_old, string picfilename_old)
{
    string newname_index = indexfilename_old + ".done";
    string newname_picfile = picfilename_old + ".done";

    // Rename index file
    int result_index = rename( indexfilename_old.c_str() , newname_index.c_str() );
    if ( result_index == 0 )
        cout << indexfilename_old << " successfully renamed to "
            << newname_index << "." << endl;
    else
        cout << "Error renaming " << indexfilename_old << endl;

    // Rename picfile
    int result_picfile = rename( picfilename_old.c_str() , newname_picfile.c_str() );
    if ( result_picfile == 0 )
        cout << picfilename_old << " successfully renamed to "
            << newname_picfile << "." << endl;
    else
        cout << "Error renaming " << picfilename_old << endl;
}
