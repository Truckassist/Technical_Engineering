#include <windows.h>
#include <iostream>
#include <string>
#include <tchar.h>

int main(int argc, char *argv[])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	std::cout<<"usage: "<<argv[0]<<" [path=.]"<<std::endl;
	std::cout << "    path=." << std::endl;
	std::wstring directorySearch = L"*.*";
	// Find the first file in the directory.
	hFind = FindFirstFile(LPCWSTR(L"*.*"), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("Invalid file handle. Error is %u.\n", GetLastError());
		std::getchar();
	}
	else
	{
		printf(". -- %ls\n", FindFileData.cFileName);
		// List all the other files in the directory.
		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			printf(". -- %ls\n", FindFileData.cFileName);
		}
		FindClose(hFind);
		std::getchar();
	}
}
