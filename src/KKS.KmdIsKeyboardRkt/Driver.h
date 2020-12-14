/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#define INITGUID

#include "device.h"
#include "queue.h"
#include "trace.h"

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KKSKmdIsKeyboardRktEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP KKSKmdIsKeyboardRktEvtDriverContextCleanup;

EXTERN_C_END
