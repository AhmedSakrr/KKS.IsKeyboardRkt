#pragma once
#include <Windows.h>
namespace Error
{
	void GetW32ErrorMessage(DWORD dwError, PWCHAR pwszOutput, UINT cchOutput);
	void GetNTErrorMessage(NTSTATUS ntStatus, PWCHAR pwszOutput, UINT cchOutput);

	void GetW32ErrorMessage(DWORD dwError, PWCHAR pwszOutput, UINT cchOutput)
	{
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, pwszOutput, cchOutput, NULL);
	}
	void GetNTErrorMessage(NTSTATUS ntStatus, PWCHAR pwszOutput, UINT cchOutput)
	{
		HMODULE hNT = LoadLibraryW(L"NTDLL.DLL");
		if (hNT != INVALID_HANDLE_VALUE) {
			FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE, hNT, ntStatus, 0, pwszOutput, cchOutput, NULL);
			FreeLibrary(hNT);
		}
	}
}