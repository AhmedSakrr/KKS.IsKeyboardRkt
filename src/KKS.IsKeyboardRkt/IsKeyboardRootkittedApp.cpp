// stdafx.h
#include "stdafx.h"

// C++ headers
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <algorithm>
using namespace std;

// Windows headers
#include <Windows.h>
#include <winerror.h>

// Custom headers
#include "IsKeyboardRootkittedApp.h"
#include "Console.h"
#include "Error.h"
#include "StopWatch.h"
#include "../Include/IS_KEYBOARD_RKT_OUTPUT_DATA.h"

namespace IsKeyboardRootkitted 
{
	DEFINE_GUID(GUID_DEVINTERFACE_KKSKmdIsKeyboardRkt,
		0x676b766c, 0x5442, 0x4550, 0x8d, 0xb9, 0xff, 0xd5, 0x93, 0xbb, 0x3d, 0x70);

	#define AUTO_RUNTIME_SECONDS	60
	#define PROGRESS_CHANGE_TIME_MS 400

	//
	// Function: Main
	// Scope: Public
	// Information: Main entry point.
	//
	int IsKeyboardRootkittedApp::Main()
	{			
		Console::InitConsole();
		wcout << L"##" << endl;
		wcout << L"## IT Software - Is Keyboard Rootkitted" << endl;
		wcout << L"## by Kjetil Kristoffer Solberg" << endl;
		wcout << L"##" << endl;

		wcout << L"Retrieving Device Name... ";
		wchar_t completeDeviceName[MAX_PATH];
		if (!GetDevicePath(const_cast<LPCGUID>(&GUID_DEVINTERFACE_KKSKmdIsKeyboardRkt), completeDeviceName, sizeof(completeDeviceName) / sizeof(completeDeviceName[0])))
		{
			wcout << L"[ERROR]" << endl;
			Console::SetWarningConsoleTextAttributes();
			wcout << completeDeviceName << endl;
			Console::SetNormalConsoleTextAttributes();
			return AlertPressAnyKeyAndReturn(-1);
		}
		wcout << "[OK]" << endl;


		wcout << L"Creating file for device... ";
		HANDLE hDevice = CreateFileW(completeDeviceName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_READ_ACCESS, NULL);

		if (hDevice == INVALID_HANDLE_VALUE)
		{
			wcout << L"[ERROR] " << endl;
			AlertW32ErrorMessage(GetLastError());
			return AlertPressAnyKeyAndReturn(-1);
		}
		wcout << L"[OK]" << endl;
		
	GotoMainMenu:
		this->ClearState();
		
		wstring wszChoice;
		wchar_t wchChoice = L'\0';				
		do {				
			
			wcout << endl;
			wcout << L"(press ? or help to show menu)" << endl;			
			wcout << endl;
			wcout << L">";			
			
			getline(std::wcin, wszChoice, L'\n');
			if (wszChoice.length() == 1) {
				wchChoice = wszChoice[0];
			}				
			
			for_each(wszChoice.begin(), wszChoice.end(), [](wchar_t& in) { in = ::tolower(in); });
		} while (wcscmp(wszChoice.c_str(),L"help") != 0 && wchChoice != L's' && wchChoice != L'S' && wchChoice != L'Q' && wchChoice != L'q' && wchChoice != L'v' && wchChoice != 'V' && wchChoice != L'?');

		if (wchChoice == L'Q' || wchChoice == L'q') {
			CloseHandle(hDevice);
			return AlertPressAnyKeyAndReturn(0);
		}

		if (wchChoice == L'V' || wchChoice == L'v') {
			this->m_bVerboseOutput = !this->m_bVerboseOutput;
			wcout << L"Verbose Output" << ((this->m_bVerboseOutput) ? L" SET" : L" NOT SET") << endl;
			goto GotoMainMenu;
		}

		if (wchChoice == L'?' || wcscmp(wszChoice.c_str(), L"help") == 0) {
			wcout << endl;
			wcout << L"s - search" << endl;
			wcout << L"v - verbose output" << ((this->m_bVerboseOutput) ? L" SET" : L" NOT SET") << endl;
			wcout << L"q - quit" << endl;
			wcout << L"? - help" << endl;		
			goto GotoMainMenu;
		}

		DWORD dwRetryCount(MAX_RETRY_COUNT);
		BOOL bResult(FALSE);
		IS_KEYBOARD_RKT_OUTPUT_DATA outputData = { 0 };
		long lPass(0);
		DWORD dwNumBytesRead(0);
	GotoRetryReadFile:
		dwRetryCount = MAX_RETRY_COUNT;
		bResult = ReadFile(hDevice, &outputData, sizeof(IS_KEYBOARD_RKT_OUTPUT_DATA), &dwNumBytesRead, NULL);
		if (!bResult) {
			wcout << L"Communicating with Device... [ERROR]" << endl;
			AlertW32ErrorMessage(GetLastError());
			CloseHandle(hDevice);
			return AlertPressAnyKeyAndReturn(-1);
		}

		if (dwNumBytesRead != sizeof(IS_KEYBOARD_RKT_OUTPUT_DATA))
		{		
			wcout << L"Checking data returned of correct size... [ERROR]" << endl;
			CloseHandle(hDevice);
			return AlertPressAnyKeyAndReturn(-1);
		}

		if (outputData.ntStatusCode != 0 ) {
			wcout << L"Checking if Driver call was a success... [ERROR]" << endl;
			AlertDriverError(&outputData);
			CloseHandle(hDevice);
			return AlertPressAnyKeyAndReturn(-1);
		}

		// Check if we need to try again.
		if (--dwRetryCount > 0 &&
			(!(outputData.dwRootComplexBaseAddress & 1) ||
				outputData.ntStatusCode != 0 ))
		{
			goto GotoRetryReadFile;
		}

		
		lPass++;

		ProcessResult(&outputData);		

		wcout << endl;
		wcout << L"##" << endl,
		wcout << L"## Result - Scan" << endl;
		wcout << L"## [pass(" << lPass << L"), looped(" << MAX_RETRY_COUNT - dwRetryCount << L")]" << endl;
		wcout << L"##" << endl;
		wcout << endl;
		wcout << L"## I/O APIC ##" << endl;
		if (this->m_bVerboseOutput) {
			for (int i = 0; i < IO_APIC_IRQ_COUNT; i++)
			{
				wcout << L"I/O APIC - IRQ" << i << L" is 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOAPIC_REDTBL[i] << (((outputData.qwIOAPIC_REDTBL[i] & 0b1'0000'0000'0000'0000) == 0) ? L" Interrupt Mask-bit NOT SET" : L" Interrupt Mask-bit SET") << endl;
			}
		}
		else {
			for (int i = 1; i < 2; i++) {
				wcout << L"I/O APIC - IRQ" << i << L" is 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOAPIC_REDTBL[i] << (((outputData.qwIOAPIC_REDTBL[i] & 0b1'0000'0000'0000'0000) == 0) ? L" Interrupt Mask-bit NOT SET" : L" Interrupt Mask-bit SET") << endl;
			}
		}
		wcout << endl;
		if (this->m_bVerboseOutput) {
			wcout << L"## Root Complex Base Address Register ##" << endl;
			wcout << L"RCBA value is 0x" << std::hex << std::setw(8) << std::setfill(L'0') << std::uppercase << outputData.dwRootComplexBaseAddress << ((outputData.dwRootComplexBaseAddress & 1) ? L" EN-bit SET" : L" EN-bit NOT SET") << endl;
			wcout << endl;
		}
		wcout << L"## IOTRn - I/O Trap Register (0-3) ##" << endl;
		wcout << L"IOTR0 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[0] << ((outputData.qwIOTRn[0] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
		wcout << L"IOTR1 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[1] << ((outputData.qwIOTRn[1] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
		wcout << L"IOTR2 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[2] << ((outputData.qwIOTRn[2] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
		wcout << L"IOTR3 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[3] << ((outputData.qwIOTRn[3] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
		wcout << endl;

		bool bSmiHandlerFound(false);
		if (this->m_bHitIOTR0) {
			AlertSmiHandlerFound(L"IOTR0", this->m_wHitPortIOTR0);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIOTR1) {
			AlertSmiHandlerFound(L"IOTR1", this->m_wHitPortIOTR1);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIOTR2) {
			AlertSmiHandlerFound(L"IOTR2", this->m_wHitPortIOTR2);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIOTR3) {
			AlertSmiHandlerFound(L"IOTR3", this->m_wHitPortIOTR3);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIoApicIRQ1) {
			AlertSmiHandlerFound(L"I/O APIC - IRQ 1 - DELMOD-bit SMI SET");
			bSmiHandlerFound = true;
		}

		if (!bSmiHandlerFound)
		{
			Console::SetGoodConsoleTextAttributes();
			wcout << L"No SMI Handler trapping the keyboard on IOTR0-IOTR3 or IRQ1!" << endl;
			Console::SetNormalConsoleTextAttributes();
		}		
		
		goto GotoMainMenu;
	}

	BOOL IsKeyboardRootkittedApp::ProcessResult(IS_KEYBOARD_RKT_OUTPUT_DATA *pOutputData)
	{	
		// IOTR0
		if (!this->m_bHitIOTR0) {
			if ((DWORD)pOutputData->qwIOTRn[0] == 0x61)
			{
				this->m_bHitIOTR0 = true;
				this->m_wHitPortIOTR0 = 0x60;
				this->m_qwHitIOTR0 = pOutputData->qwIOTRn[0];
			}

			if ((DWORD)pOutputData->qwIOTRn[0] == 0x65)
			{
				this->m_bHitIOTR0 = true;
				this->m_wHitPortIOTR0 = 0x64;
				this->m_qwHitIOTR0 = pOutputData->qwIOTRn[0];
			}
		}

		// IOTR1
		if (!this->m_bHitIOTR1) {
			if ((DWORD)pOutputData->qwIOTRn[1] == 0x61)
			{
				this->m_bHitIOTR1 = true;
				this->m_wHitPortIOTR1 = 0x60;
				this->m_qwHitIOTR1 = pOutputData->qwIOTRn[1];
			}

			if ((DWORD)pOutputData->qwIOTRn[1] == 0x65)
			{
				this->m_bHitIOTR1 = true;
				this->m_wHitPortIOTR1 = 0x64;
				this->m_qwHitIOTR1 = pOutputData->qwIOTRn[1];
			}
		}

		// IOTR2
		if (!this->m_bHitIOTR2) {
			if ((DWORD)pOutputData->qwIOTRn[2] == 0x61)
			{
				this->m_bHitIOTR2 = true;
				this->m_wHitPortIOTR2 = 0x60;
				this->m_qwHitIOTR2 = pOutputData->qwIOTRn[2];
			}

			if ((DWORD)pOutputData->qwIOTRn[2] == 0x65)
			{
				this->m_bHitIOTR2 = true;
				this->m_wHitPortIOTR2 = 0x64;
				this->m_qwHitIOTR2 = pOutputData->qwIOTRn[2];
			}
		}

		// IOTR3
		if (!this->m_bHitIOTR3) {
			if ((DWORD)pOutputData->qwIOTRn[3] == 0x61)
			{
				this->m_bHitIOTR3 = true;
				this->m_wHitPortIOTR3 = 0x60;
				this->m_qwHitIOTR3 = pOutputData->qwIOTRn[3];
			}

			if ((DWORD)pOutputData->qwIOTRn[3] == 0x65)
			{
				this->m_bHitIOTR3 = true;
				this->m_wHitPortIOTR3 = 0x64;
				this->m_qwHitIOTR3 = pOutputData->qwIOTRn[3];
			}
		}

		if (((pOutputData->qwIOAPIC_REDTBL[1] & 0b0111'0000'0000) == 0b0010'0000'0000)) {
			this->m_bHitIoApicIRQ1 = true;
		}

		if (this->m_bRunFirstHit) {
			return !(this->m_bHitIOTR0 || this->m_bHitIOTR1 || this->m_bHitIOTR2 || this->m_bHitIOTR3 || this->m_bHitIoApicIRQ1);
		}
		return !(this->m_bHitIOTR0 && this->m_bHitIOTR1 && this->m_bHitIOTR2 && this->m_bHitIOTR3 && this->m_bHitIoApicIRQ1);
	}

	//
	// Function: GetDevicePath
	// Scope: Private 
	// ?: Returnes true if it finds a device path to device with given device interface.
	// ?: ... false if not. Output device path in DevicePath.
	//
	bool IsKeyboardRootkittedApp::GetDevicePath(LPCGUID GUID_DEVINTEFACE, PWCHAR DevicePath, size_t BufLen)
	{
		CONFIGRET cr = CR_SUCCESS;
		PWSTR deviceInterfaceList = NULL;
		ULONG deviceInterfaceListLength = 0;
		PWSTR nextInterface;
		HRESULT hr = E_FAIL;
		bool bRet = true;

		cr = CM_Get_Device_Interface_List_Size(
			&deviceInterfaceListLength,
			const_cast<LPGUID>(GUID_DEVINTEFACE),
			NULL,
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
		if (cr != CR_SUCCESS) {
			wsprintfW(DevicePath, L"Error 0x%X retrieving device interface list size.", cr);
			goto clean0;
		}

		if (deviceInterfaceListLength <= 1) {
			bRet = FALSE;
			wsprintfW(DevicePath, L"No active device interfaces found. Is the sample driver loaded?");
			goto clean0;
		}

		deviceInterfaceList = (PWSTR)malloc(deviceInterfaceListLength * sizeof(WCHAR));
		if (deviceInterfaceList == NULL) {
			wsprintfW(DevicePath, L"Error allocating memory for device interface list.");
			goto clean0;
		}
		ZeroMemory(deviceInterfaceList, deviceInterfaceListLength * sizeof(WCHAR));

		cr = CM_Get_Device_Interface_List(
			const_cast<LPGUID>(GUID_DEVINTEFACE),
			NULL,
			deviceInterfaceList,
			deviceInterfaceListLength,
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
		if (cr != CR_SUCCESS) {
			wsprintfW(DevicePath, L"Error 0x%X retrieving device interface list.", cr);
			goto clean0;
		}

		nextInterface = deviceInterfaceList + wcslen(deviceInterfaceList) + 1;
		if (*nextInterface != UNICODE_NULL) {
			wsprintfW(DevicePath, L"Warning: More than one device interface instance found. Selecting first matching device.");
		}

		hr = StringCchCopy(DevicePath, BufLen, deviceInterfaceList);
		if (FAILED(hr)) {
			bRet = FALSE;
			wsprintfW(DevicePath, L"Error: StringCchCopy failed with HRESULT 0x%x", hr);
			goto clean0;
		}

	clean0:
		if (deviceInterfaceList != NULL) {
			free(deviceInterfaceList);
		}
		if (CR_SUCCESS != cr) {
			bRet = false;
		}

		return bRet;
	}// GetDevicePath


	void IsKeyboardRootkittedApp::AlertSmiHandlerFound(PWCHAR iotr, WORD port)
	{		
		Console::SetWarningConsoleTextAttributes();
		wcout << "Keyboard Is Trapped by SMI Handler on " << iotr << L" I/O Address(IOAD) 0x" << std::hex << std::uppercase << port << endl;
		Console::SetNormalConsoleTextAttributes();
	}

	void IsKeyboardRootkittedApp::AlertSmiHandlerFound(PWCHAR onMsg)
	{
		Console::SetWarningConsoleTextAttributes();
		wcout << "Keyboard Is Trapped by SMI Handler on " << onMsg << endl;
		Console::SetNormalConsoleTextAttributes();
	}

	void IsKeyboardRootkittedApp::AlertNTErrorMessage(NTSTATUS ntStatusCode)
	{
		Console::SetWarningConsoleTextAttributes();
		wchar_t wszBuffer[MAX_STRING_BUFFER_SIZE];
		Error::GetNTErrorMessage(ntStatusCode, wszBuffer, MAX_STRING_BUFFER_SIZE);
		wcout << wszBuffer << endl;
		Console::SetNormalConsoleTextAttributes();
	}

	void IsKeyboardRootkittedApp::AlertW32ErrorMessage(DWORD dwError)
	{
		Console::SetWarningConsoleTextAttributes();
		wchar_t wszBuffer[MAX_STRING_BUFFER_SIZE];
		Error::GetW32ErrorMessage(dwError, wszBuffer, MAX_STRING_BUFFER_SIZE);
		wcout << wszBuffer << endl;
		Console::SetNormalConsoleTextAttributes();
	}
	
	int IsKeyboardRootkittedApp::AlertPressAnyKeyAndReturn(int returnCode)
	{
		system("pause");
		return returnCode;
	}


	//
	// Function: PrintOutDriverErrorMessage(PIS_KEYBOARD_RKT_OUTPUT_DATA pOutputData)
	//
	void IsKeyboardRootkittedApp::AlertDriverError(PIS_KEYBOARD_RKT_OUTPUT_DATA pOutputData)
	{
		wcout << endl << L"## DRIVER ERROR MESSAGE (General) ##" << endl;

		wchar_t wszStatusCode[MAX_STRING_BUFFER_SIZE];
		wcscpy_s(wszStatusCode, MAX_STRING_BUFFER_SIZE, L"");

		bool bCrLf = false;		
		if (pOutputData->ntStatusCode != 0) {								
			Error::GetNTErrorMessage(pOutputData->ntStatusCode, wszStatusCode, MAX_STRING_BUFFER_SIZE);
			if (wszStatusCode[wcslen(wszStatusCode) - 1] == L'\n') {
				bCrLf = true;
			}			
			wcout << L"0x" << std::hex << std::uppercase << std::setw(8) << std::setfill(L'0') << pOutputData->ntStatusCode << endl << wszStatusCode;
			if (!bCrLf) {
				wcout << endl;
			}
			wcout << pOutputData->szErrorMessage << endl << endl;
		}
		
	}


}

/*
GotoMainMenu:
this->ClearState();

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

CONSOLE_CURSOR_INFO conCurInfo = { 0 };
GetConsoleCursorInfo(hConsole, &conCurInfo);

conCurInfo.bVisible = TRUE;
SetConsoleCursorInfo(hConsole, &conCurInfo);

wstring wszChoice;
wchar_t wchChoice = L'\0';
do {
wcout << endl;
wcout << L"## Main Menu ##" << endl;
wcout << L"[1] Manual Scan" << endl;
//wcout << L"[2] Automatic First Hit Scan" << endl;
//wcout << L"[3] Automatic 1 Minute Scan" << endl;
wcout << L"[Q] Quit" << endl;
wcout << endl;
wcout << L">";

getline(std::wcin, wszChoice, L'\n');
if (wszChoice.length() == 1) {
wchChoice = wszChoice[0];
}

//wcin.seekg(wcin.gcount());
} while (wchChoice != L'1' && wchChoice != L'2' && wchChoice != L'3' && wchChoice != L'Q' && wchChoice != L'q');


if (wchChoice == L'Q' || wchChoice == L'q') {
CloseHandle(hDevice);
return AlertPressAnyKeyAndReturn(0);
}


bool bRunAutomatic = false;
if (wchChoice == L'2' || wchChoice == L'3') {
if (wchChoice == L'2') {
this->m_bRunFirstHit = true;
}
bRunAutomatic = true;
}

DWORD dwRetryCount(0);
ULONGLONG lPass(0);
IS_KEYBOARD_RKT_OUTPUT_DATA outputData = { 0 };
DWORD dwNumBytesRead(0);
BOOL bResult(FALSE);
StopWatch::Clock timer;

if (bRunAutomatic) {
	timer.Start();
}


COORD coordCursor = Console::GetCursorPosition();

conCurInfo.bVisible = FALSE;
SetConsoleCursorInfo(hConsole, &conCurInfo);

WORD wProgressCurrentPosition = 0;
StopWatch::Clock timerProgress;
timerProgress.Start();
GotoRetryReadFile:
dwRetryCount = MAX_RETRY_COUNT;
bResult = ReadFile(hDevice, &outputData, sizeof(IS_KEYBOARD_RKT_OUTPUT_DATA), &dwNumBytesRead, NULL);
if (!bResult) {
	if (bRunAutomatic) {
		coordCursor.X += 1;
		Console::SetCursorPosition(coordCursor);
	}
	wcout << L"Communicating with Device... [ERROR]" << endl;
	AlertW32ErrorMessage(GetLastError());
	CloseHandle(hDevice);
	return AlertPressAnyKeyAndReturn(-1);
}

if (dwNumBytesRead != sizeof(IS_KEYBOARD_RKT_OUTPUT_DATA))
{
	if (bRunAutomatic) {
		coordCursor.X += 1;
		Console::SetCursorPosition(coordCursor);
	}

	wcout << L"Checking data returned of correct size... [ERROR]" << endl;
	CloseHandle(hDevice);
	return AlertPressAnyKeyAndReturn(-1);
}

if (outputData.ntStatusCode != 0) {
	if (bRunAutomatic) {
		coordCursor.X += 1;
		Console::SetCursorPosition(coordCursor);
	}
	wcout << L"Checking if Driver call was a success... [ERROR]" << endl;
	AlertDriverError(&outputData);
	CloseHandle(hDevice);
	return AlertPressAnyKeyAndReturn(-1);
}

// Check if we need to try again.
if (--dwRetryCount > 0 &&
	(!(outputData.dwRootComplexBaseAddress & 1) ||
		outputData.ntStatusCode != 0))
{
	goto GotoRetryReadFile;
}

if (bRunAutomatic) {
	BOOL bAgain = ProcessResult(&outputData);
	ULONGLONG elapsedMs = timer.GetSnapshotTimeInMs();
	if (elapsedMs < AUTO_RUNTIME_SECONDS * 1000 && bAgain) {
		lPass++;

		ULONGLONG elapsedTime = timerProgress.GetSnapshotTimeInMs();
		if (wProgressCurrentPosition == 0 && elapsedTime >= PROGRESS_CHANGE_TIME_MS) {
			wcout << L".  ";
			wProgressCurrentPosition++;
			timerProgress.Stop();
			timerProgress.Start();
		}
		else if (wProgressCurrentPosition == 1 && elapsedTime >= PROGRESS_CHANGE_TIME_MS) {
			wcout << L".. ";
			wProgressCurrentPosition++;
			timerProgress.Stop();
			timerProgress.Start();
		}
		else if (wProgressCurrentPosition == 2 && elapsedTime >= PROGRESS_CHANGE_TIME_MS) {
			wcout << L"...";
			wProgressCurrentPosition++;
			timerProgress.Stop();
			timerProgress.Start();
		}
		else if (wProgressCurrentPosition == 3 && elapsedTime >= PROGRESS_CHANGE_TIME_MS) {
			wcout << L" ..";
			wProgressCurrentPosition++;
			timerProgress.Stop();
			timerProgress.Start();
		}
		else if (wProgressCurrentPosition == 4 && elapsedTime >= PROGRESS_CHANGE_TIME_MS) {
			wcout << L"  .";
			wProgressCurrentPosition++;
			timerProgress.Stop();
			timerProgress.Start();
		}
		else if (wProgressCurrentPosition == 5 && elapsedTime >= PROGRESS_CHANGE_TIME_MS) {
			wcout << L"   ";
			wProgressCurrentPosition++;
			timerProgress.Stop();
			timerProgress.Start();
		}

		if (wProgressCurrentPosition > 5) {
			wProgressCurrentPosition = 0;
		}

		Console::SetCursorPosition(coordCursor);

		goto GotoRetryReadFile;
	}
	else {
		timer.Stop();
		timerProgress.Stop();

		coordCursor.X += 1;
		Console::SetCursorPosition(coordCursor);

		wcout << endl;
		wcout << L"##" << endl,
			wcout << L"## Result - Automatic Mode" << ((this->m_bRunFirstHit) ? L" First Hit" : L"") << endl;
		wcout << L"## [pass(" << std::dec << lPass << L"), time(" << timer.GetElapsedTimeInMs() << L" ms)]" << endl;
		wcout << L"##" << endl;
		wcout << endl;
		wcout << L"## I/O APIC ##" << endl;
		//for (int i = 0; i < IO_APIC_IRQ_COUNT; i++) {
		for (int i = 1; i < 2; i++) {
			wcout << L"I/O APIC - IRQ" << i << L" is 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOAPIC_REDTBL[i] << (((outputData.qwIOAPIC_REDTBL[i] & 0b1'0000'0000'0000'0000) == 0) ? L" Interrupt Mask-bit NOT SET" : L" Interrupt Mask-bit SET") << endl;
		}
		wcout << endl;
		wcout << L"## Root Complex Base Address Register ##" << endl;
		wcout << L"RCBA value is 0x" << std::hex << std::setw(8) << std::setfill(L'0') << std::uppercase << outputData.dwRootComplexBaseAddress << ((outputData.dwRootComplexBaseAddress & 1) ? L" EN-bit SET" : L" EN-bit NOT SET") << endl;
		wcout << endl;
		wcout << L"## IOTRn - I/O Trap Register (0-3) ##" << endl;
		if (this->m_bHitIOTR0) {
			wcout << L"IOTR0 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << this->m_qwHitIOTR0 << L" TRSE-bit SET" << endl;
		}
		if (this->m_bHitIOTR1) {
			wcout << L"IOTR1 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << this->m_qwHitIOTR1 << L" TRSE-bit SET" << endl;
		}
		if (this->m_bHitIOTR2) {
			wcout << L"IOTR2 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << this->m_qwHitIOTR2 << L" TRSE-bit SET" << endl;
		}
		if (this->m_bHitIOTR3) {
			wcout << L"IOTR3 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << this->m_qwHitIOTR3 << L" TRSE-bit SET" << endl;
		}

		wcout << endl;

		bool bSmiHandlerFound(false);
		if (this->m_bHitIOTR0) {
			AlertSmiHandlerFound(L"IOTR0", this->m_wHitPortIOTR0);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIOTR1) {
			AlertSmiHandlerFound(L"IOTR1", this->m_wHitPortIOTR1);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIOTR2) {
			AlertSmiHandlerFound(L"IOTR2", this->m_wHitPortIOTR2);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIOTR3) {
			AlertSmiHandlerFound(L"IOTR3", this->m_wHitPortIOTR3);
			bSmiHandlerFound = true;
		}
		if (this->m_bHitIoApicIRQ1) {
			AlertSmiHandlerFound(L"I/O APIC - IRQ 1 - DELMOD-bit SMI SET");
			bSmiHandlerFound = true;
		}

		if (!bSmiHandlerFound)
		{
			Console::SetGoodConsoleTextAttributes();
			wcout << L"No SMI Handler trapping the keyboard on IOTR0-IOTR3 or IRQ1!" << endl;
			Console::SetNormalConsoleTextAttributes();
		}

		goto GotoMainMenu;
	}
}

lPass++;

ProcessResult(&outputData);

wcout << endl;
wcout << L"##" << endl,
wcout << L"## Result - Manual Mode" << endl;
wcout << L"## [pass(" << lPass << L"), looped(" << MAX_RETRY_COUNT - dwRetryCount << L")]" << endl;
wcout << L"##" << endl;
wcout << endl;
wcout << L"## I/O APIC ##" << endl;
//for (int i = 0; i < IO_APIC_IRQ_COUNT; i++) {
for (int i = 1; i < 2; i++) {
	wcout << L"I/O APIC - IRQ" << i << L" is 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOAPIC_REDTBL[i] << (((outputData.qwIOAPIC_REDTBL[i] & 0b1'0000'0000'0000'0000) == 0) ? L" Interrupt Mask-bit NOT SET" : L" Interrupt Mask-bit SET") << endl;
}
wcout << endl;
wcout << L"## Root Complex Base Address Register ##" << endl;
wcout << L"RCBA value is 0x" << std::hex << std::setw(8) << std::setfill(L'0') << std::uppercase << outputData.dwRootComplexBaseAddress << ((outputData.dwRootComplexBaseAddress & 1) ? L" EN-bit SET" : L" EN-bit NOT SET") << endl;
wcout << endl;
wcout << L"## IOTRn - I/O Trap Register (0-3) ##" << endl;
wcout << L"IOTR0 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[0] << ((outputData.qwIOTRn[0] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
wcout << L"IOTR1 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[1] << ((outputData.qwIOTRn[1] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
wcout << L"IOTR2 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[2] << ((outputData.qwIOTRn[2] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
wcout << L"IOTR3 0x" << std::hex << std::setw(16) << std::setfill(L'0') << std::uppercase << outputData.qwIOTRn[3] << ((outputData.qwIOTRn[3] & 1) ? L" TRSE-bit SET" : L" TRSE-bit NOT SET") << endl;
wcout << endl;

bool bSmiHandlerFound(false);
if (this->m_bHitIOTR0) {
	AlertSmiHandlerFound(L"IOTR0", this->m_wHitPortIOTR0);
	bSmiHandlerFound = true;
}
if (this->m_bHitIOTR1) {
	AlertSmiHandlerFound(L"IOTR1", this->m_wHitPortIOTR1);
	bSmiHandlerFound = true;
}
if (this->m_bHitIOTR2) {
	AlertSmiHandlerFound(L"IOTR2", this->m_wHitPortIOTR2);
	bSmiHandlerFound = true;
}
if (this->m_bHitIOTR3) {
	AlertSmiHandlerFound(L"IOTR3", this->m_wHitPortIOTR3);
	bSmiHandlerFound = true;
}
if (this->m_bHitIoApicIRQ1) {
	AlertSmiHandlerFound(L"I/O APIC - IRQ 1 - DELMOD-bit SMI SET");
	bSmiHandlerFound = true;
}

if (!bSmiHandlerFound)
{
	Console::SetGoodConsoleTextAttributes();
	wcout << L"No SMI Handler trapping the keyboard on IOTR0-IOTR3 or IRQ1!" << endl;
	Console::SetNormalConsoleTextAttributes();
}

conCurInfo.bVisible = TRUE;
SetConsoleCursorInfo(hConsole, &conCurInfo);

wchChoice = L'\0';
do {
	wcout << L"Run Again [Y, N]>";
	getline(std::wcin, wszChoice, L'\n');
	if (wszChoice.length() == 1) {
		wchChoice = wszChoice[0];
	}

	//wcin.seekg(wcin.gcount());
} while (wchChoice != L'y' && wchChoice != L'n' && wchChoice != L'Y' && wchChoice != L'N');

if (wchChoice == L'y' || wchChoice == L'Y') {
	this->ClearState();
	goto GotoRetryReadFile;
}

goto GotoMainMenu;
*/