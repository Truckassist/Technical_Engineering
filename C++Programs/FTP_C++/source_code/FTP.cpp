#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <time.h>
#include <wininet.h>
#include <sys\stat.h> 
#include <iostream>


	int main(void) {
	char ftp[] = "129.000.000.300";
	char user[] = "Test_FTP";
	char password[] = "Password_1";
	char localFile[] = "C:\\Users\\Agartha\\Desktop\\a.log"; // Use "file.txt" to send a file from the root dir.
	//Do some formatting for the filename to be uploaded: COMPUTERNAME-DATE-TIME.
	char time[9]; _strtime_s(time, 9); char date[9]; _strdate_s(date, 9);
	char computerName[MAX_COMPUTERNAME_LENGTH + 1]; DWORD buffer = MAX_COMPUTERNAME_LENGTH + 1;
	if (GetComputerName(computerName, &buffer) == 0) printf("Could not retrieve the computer name.\n");
	char remoteFile[MAX_COMPUTERNAME_LENGTH + 30] = "logs/"; // Sends .log file to folder in FTP host called logs.
	strcat_s(remoteFile, MAX_COMPUTERNAME_LENGTH + 30, computerName); strcat_s(remoteFile, MAX_COMPUTERNAME_LENGTH + 30, "-"); strcat_s(remoteFile, MAX_COMPUTERNAME_LENGTH + 30, date);
	strcat_s(remoteFile, MAX_COMPUTERNAME_LENGTH + 30, "-"); strcat_s(remoteFile, MAX_COMPUTERNAME_LENGTH + 30, time); strcat_s(remoteFile, MAX_COMPUTERNAME_LENGTH + 30, ".log"); // Change .log to whatever file extension you want.
	for (int i = 5;i<(sizeof(remoteFile) / sizeof(char));i++) {
		if (remoteFile[i] == '/' || remoteFile[i] == ':') remoteFile[i] = '-';
	}
	HINTERNET hInternet;
	HINTERNET hFtpSession;

	if (InternetAttemptConnect(0) == ERROR_SUCCESS) printf("Internet available, uploading...\n");
	else printf("Internet unavailable.\n");

	hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet != NULL) {
		hFtpSession = InternetConnect(hInternet, ftp, INTERNET_DEFAULT_FTP_PORT, user, password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (hFtpSession != NULL) {
			if (FtpPutFile(hFtpSession, localFile, remoteFile, FTP_TRANSFER_TYPE_BINARY, 0)) {
				InternetCloseHandle(hFtpSession);
				InternetCloseHandle(hInternet);
			}
			else return -1;
		}
		else return -1;
	}
	else  return -1;
	printf("File uploaded.\n");
	std::cout << std::endl << "Press any key to continue...";
	getchar();
	return 0;
}