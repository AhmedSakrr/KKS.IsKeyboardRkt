// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>


//
// #define
//
#define IOCL_UMD_IS_KEYBOARD_RKT	0x8000
#define IOCTL_IS_KEYBOARD_RKT_READ CTL_CODE(IOCL_UMD_IS_KEYBOARD_RKT, 0x2800, METHOD_BUFFERED, FILE_READ_ACCESS)

#define MAX_RETRY_COUNT				0x20
#define RCBA_MASK					0xFFFFC000 

//
// GUID DEFINITIONS OF DEVICE INTERFACES
//
DEFINE_GUID(GUID_DEVINTERFACE_KKSUmdIsKeyboardRkt,
	0x5a104610, 0x4442, 0x45a4, 0x85, 0x5a, 0x41, 0x36, 0x8b, 0xe1, 0x4a, 0xab);

DEFINE_GUID(GUID_DEVCLASS_KKSUmdIsKeyboardRkt,
	0x78A1C341, 0x4539, 0x11d3, 0xB8, 0x8D, 0x00, 0xC0, 0x4F, 0xAD, 0x51, 0x71);

DEFINE_GUID(GUID_DEVINTERFACE_KKSKmdIsKeyboardRkt,
	0x676b766c, 0x5442, 0x4550, 0x8d, 0xb9, 0xff, 0xd5, 0x93, 0xbb, 0x3d, 0x70);

// TODO: reference additional headers your program requires here
#include <Windows.h>
#include <winioctl.h>
#include <aclapi.h>
#include <sddl.h>
#include <devioctl.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <initguid.h>
#include <devpkey.h>
#include <winerror.h>
#include <strsafe.h>
