#include <stdio.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>
using namespace std;
//Simple struct to return from lsfiles
struct List {
	vector<string> files;
	vector<string> folders;
};
//All of the hard work
struct List lsfiles(string folder)  //(c) http://stackoverflow.com/a/20847429/1009816
{
	vector<string> files; //Will be added to List
	vector<string> folders; //Will be added to List
	char search_path[200];
	sprintf(search_path, "%s*.*", folder.c_str());
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// read all (real) files in current folder, delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				files.push_back(fd.cFileName);
			}
			else //Put folders into vector
			{
				folders.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	List me;
	me.files = files;
	me.folders = folders;
	return me;
}
int main(int argc, char *argv[])
{
	string where;
	if (argc > 1) { where = argv[1] + string("\\"); }
	else { where = ""; }
	List you = lsfiles(where); //Get contents of directory
	vector<string>::iterator files_begin = you.files.begin();
	vector<string>::iterator files_end = you.files.end();
	vector<string>::iterator folders_begin = you.folders.begin();
	vector<string>::iterator folders_end = you.folders.end();
	for (; folders_begin != folders_end; folders_begin++) {
		cout << "[D] " << *folders_begin << "\n";
	}
	for (; files_begin != files_end; files_begin++) {
		cout << "[F] " << *files_begin << "\n";
	}
	std::getchar();
	return 0;
}