#include <Windows.h>
#include <iostream>

using namespace std;

typedef struct
{
	LPBYTE lpbData;
	DWORD dwDataSize;
} BUFFER_WITH_SIZE;

typedef BUFFER_WITH_SIZE* PBUFFER_WITH_SIZE;

void read_file(LPCSTR filename, PBUFFER_WITH_SIZE pBws)
{
	HANDLE hProcHeap;
	HANDLE hFile;
	LONGLONG llFileSize;
	LARGE_INTEGER liFileSize;
	DWORD dwBytesRead;
	DWORD dwTotalBytesRead;
	LPBYTE lpFileData;
	BOOL bResult;

	hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		printf("Could not open file %s\n", filename);

	bResult = GetFileSizeEx(hFile, &liFileSize);
	if (!bResult)
		printf("Error getting size of file %s\n", filename);
	llFileSize = liFileSize.QuadPart;

	hProcHeap = GetProcessHeap();

	lpFileData = (LPBYTE)HeapAlloc(hProcHeap, HEAP_ZERO_MEMORY, llFileSize);
	if (lpFileData == NULL)
		printf("Error allocating memory\n");

	dwTotalBytesRead = 0;
	do
	{
		bResult = ReadFile(hFile, lpFileData + dwTotalBytesRead,
			llFileSize - dwTotalBytesRead, &dwBytesRead, NULL);
		dwTotalBytesRead += dwBytesRead;
	} while (!(bResult && dwBytesRead == 0) || !bResult);

	if (!bResult)
		printf("Error reading file %s\n", filename);

	CloseHandle(hFile);

	pBws->lpbData = lpFileData;
	pBws->dwDataSize = llFileSize;
}

int main()
{
	freopen("DLLBytes.cpp", "w", stdout);

	BUFFER_WITH_SIZE dll;
	read_file("FILENAME.dll", &dll);

	cout << "#include \"DLLBytes.h\"" << endl;
	cout << "unsigned char dllByteArray[" << dll.dwDataSize << "] = {" << endl << "    ";

	int currentCol = 0;
	for (int i = 0; i < dll.dwDataSize - 1; i++)
	{
		currentCol++;
		if (currentCol == 50)
		{
			currentCol = 0;
			cout << endl;
			cout << "    ";
		}

		printf("0x%02X, ", (byte)dll.lpbData[i]);
	}

	printf("0x%02X \n", (byte)dll.lpbData[dll.dwDataSize - 1]);
	cout << "};";

	freopen("DLLBytes.h", "w", stdout);
	cout << "#pragma once" << "\n\n";
	cout << "extern unsigned char dllByteArray[" << dll.dwDataSize << "];";
}