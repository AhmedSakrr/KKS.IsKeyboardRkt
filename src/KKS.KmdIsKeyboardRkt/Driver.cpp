/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/
#include <ntddk.h>
#include <minwindef.h>
#include <ntstrsafe.h>
#include "driver.h"
#include "driver.tmh"
#include "../Include/IS_KEYBOARD_RKT_OUTPUT_DATA.h"





// Method Segment
#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, KKSKmdIsKeyboardRktEvtDeviceAdd)
#pragma alloc_text (PAGE, KKSKmdIsKeyboardRktEvtDriverContextCleanup)
#pragma alloc_text (PAGE, WriteDebugInfo)
#pragma alloc_text (PAGE, WriteDebugInfoLine)
#pragma alloc_text (PAGE, WriteDebugInfoLineFormat)

#endif




// Extern 
//extern IS_KEYBOARD_RKT_OUTPUT_DATA g_outputData;


NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,

    STATUS_UNSUCCESSFUL otherwise.

--*/
{	
	WriteDebugInfoLine("DriverEntry - STARTED!");

    WDF_DRIVER_CONFIG config;
    NTSTATUS status;
    WDF_OBJECT_ATTRIBUTES attributes;

    //
    // Initialize WPP Tracing
    //
    //WPP_INIT_TRACING( DriverObject, RegistryPath );

    //TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    //
    // Register a cleanup callback so that we can call WPP_CLEANUP when
    // the framework driver object is deleted during driver unload.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = KKSKmdIsKeyboardRktEvtDriverContextCleanup;

    WDF_DRIVER_CONFIG_INIT(&config,
                           KKSKmdIsKeyboardRktEvtDeviceAdd
                           );

	WriteDebugInfoLine("#1");
	//status = InitializeIoMemMap();
	//if (!NT_SUCCESS(status)) {
	//	WriteDebugInfoLineFormat("#1.5 = 0x%X", status);
	//}
	WriteDebugInfoLine("#2");
    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE
                             );

    if (!NT_SUCCESS(status)) {
        //TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER, "WdfDriverCreate failed %!STATUS!", status);
        //WPP_CLEANUP(DriverObject);
        return status;
    }

    //TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	WriteDebugInfoLine("DriverEntry - ENDED!");

    return status;
}

NTSTATUS
KKSKmdIsKeyboardRktEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++
Routine Description:

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
{
	WriteDebugInfoLine("EvtDeviceAdd - STARTED!");

    NTSTATUS status;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    //TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

    status = KKSKmdIsKeyboardRktCreateDevice(DeviceInit);

    //TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");

	WriteDebugInfoLine("EvtDeviceAdd - ENDED!");

    return status;
}

VOID
KKSKmdIsKeyboardRktEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
/*++
Routine Description:

    Free all the resources allocated in DriverEntry.

Arguments:

    DriverObject - handle to a WDF Driver object.

Return Value:

    VOID.

--*/
{
	WriteDebugInfoLine("EvtDriverContextCleanup - STARTED!");

    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE ();

    //TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");

	//UnInitializeIoMemMap();

    //
    // Stop WPP Tracing
    //
    //WPP_CLEANUP( WdfDriverWdmGetDriverObject( (WDFDRIVER) DriverObject) );
	
	WriteDebugInfoLine("EvtDriverContextCleanup - ENDED!");
}

void WriteDebugInfo(char *pszInformation)
{
	if (pszInformation == NULL) {
		return;
	}

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, pszInformation);
	/*
	return;
	if (pszInformation == NULL)	{
		return;
	}
	DWORD dwLength = (DWORD)strlen(pszInformation);
	if (dwLength == 0) {
		return;
	}

	UNICODE_STRING uString;
	RtlInitUnicodeString(&uString, g_wszOutputDebugPath);

	OBJECT_ATTRIBUTES objectAttributes;
	InitializeObjectAttributes(&objectAttributes, &uString, 0, NULL, NULL);

	HANDLE hFile = NULL;

	IO_STATUS_BLOCK ioStatusBlock;
	memset(&ioStatusBlock, 0, sizeof(IO_STATUS_BLOCK));


	NTSTATUS ntStatus = ZwCreateFile(&hFile, GENERIC_READ | GENERIC_WRITE, &objectAttributes, &ioStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_WRITE_THROUGH | FILE_NON_DIRECTORY_FILE, NULL, 0);//, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING);
	if (!NT_SUCCESS(ntStatus)) {
		return;
	}

	FILE_STANDARD_INFORMATION fileStdInfo;
	ntStatus = ZwQueryInformationFile(hFile, &ioStatusBlock, (PVOID)&fileStdInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(ntStatus)) {
		ZwClose(hFile);
		return;
	}

	ntStatus = ZwWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, pszInformation, dwLength, &fileStdInfo.EndOfFile, NULL);

	ZwClose(hFile);
	*/
}

void WriteDebugInfoLineFormat(char *pszInformation, ...) 
{		
	if (pszInformation == NULL) {
		return;
	}
	
	va_list args;
	va_start(args, pszInformation);
	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, pszInformation, args);
	va_end(args);			
}

void FormatString(char *pszDest, size_t chLength, char *pszFormat, ...)
{
	if (pszDest == NULL || pszFormat == NULL || chLength < strlen(pszFormat) ) {
		return;
	}
	
	va_list args;
	va_start(args, pszFormat);
	::RtlStringCbVPrintfA(pszDest, chLength, pszFormat, args);
	va_end(args);
}

void WriteDebugInfoLine(char *pszInformation)
{
	if (pszInformation == NULL) {
		return;
	}

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL,pszInformation);
	/*return;
	if (pszInformation == NULL) {
		return;
	}
	DWORD dwLength = (DWORD)strlen(pszInformation);
	if (dwLength == 0) {
		return;
	}

	UNICODE_STRING uString;
	RtlInitUnicodeString(&uString, g_wszOutputDebugPath);

	OBJECT_ATTRIBUTES objectAttributes;
	InitializeObjectAttributes(&objectAttributes, &uString, 0, NULL, NULL);

	HANDLE hFile = NULL;

	IO_STATUS_BLOCK ioStatusBlock;
	memset(&ioStatusBlock, 0, sizeof(IO_STATUS_BLOCK));


	NTSTATUS ntStatus = ZwCreateFile(&hFile, GENERIC_READ | GENERIC_WRITE, &objectAttributes, &ioStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_WRITE_THROUGH | FILE_NON_DIRECTORY_FILE, NULL, 0);//, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING);
	if (!NT_SUCCESS(ntStatus)) {
		return;
	}

	FILE_STANDARD_INFORMATION fileStdInfo;
	ntStatus = ZwQueryInformationFile(hFile, &ioStatusBlock, (PVOID)&fileStdInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(ntStatus)) {
		ZwClose(hFile);
		return;
	}

	LARGE_INTEGER newOffset = { 0 };	
	newOffset = fileStdInfo.EndOfFile;
	newOffset.LowPart += dwLength;
	ntStatus = ZwWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, pszInformation, dwLength, &fileStdInfo.EndOfFile, NULL);
	ntStatus = ZwWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, "\r\n", 2, &newOffset, NULL);

	ZwClose(hFile);
	*/
}



