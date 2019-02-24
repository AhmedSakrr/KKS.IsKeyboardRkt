/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#include "public.h"

EXTERN_C_START

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    ULONG PrivateDeviceData;  // just a placeholder

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
KKSKmdIsKeyboardRktCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );

EXTERN_C_END

/*
;
; KKS.KmdIsKeyboardRkt.inf
;

[Version]
Signature="$WINDOWS NT$"
Class="Keyboard Rootkitted"
ClassGuid={18A1C341-4539-11d3-B88D-00C04FAD5171}
Provider=%ManufacturerName%
CatalogFile=KKS.KmdIsKeyboardRkt.cat
DriverVer=10/07/2015,1.0.0.0

[DestinationDirs]
DefaultDestDir = 12

; ================= Class section =====================

[ClassInstall32]
Addreg=SampleClassReg

[SampleClassReg]
HKR,,,0,%ClassName%
HKR,,Icon,,-5

[SourceDisksNames]
1 = %DiskName%,,,""

[SourceDisksFiles]
KKS.KmdIsKeyboardRkt.sys  = 1,,

;*****************************************
; Install Section
;*****************************************

[Manufacturer]
%ManufacturerName%=Standard,NT$ARCH$

[Standard.NT$ARCH$]
%KKS.KmdIsKeyboardRkt.DeviceDesc%=KKS.KmdIsKeyboardRkt_Device, Root\IsKeyboardRootKitted

[KKS.KmdIsKeyboardRkt_Device.NT]
CopyFiles=Drivers_Dir

[Drivers_Dir]
KKS.KmdIsKeyboardRkt.sys

;-------------- Service installation
[KKS.KmdIsKeyboardRkt_Device.NT.Services]
AddService = KKS.KmdIsKeyboardRkt,%SPSVCINST_ASSOCSERVICE%, KKS.KmdIsKeyboardRkt_Service_Inst

; -------------- KKS.KmdIsKeyboardRkt driver install sections
[KKS.KmdIsKeyboardRkt_Service_Inst]
DisplayName    = %KKS.KmdIsKeyboardRkt.SVCDESC%
ServiceType    = 1               ; SERVICE_KERNEL_DRIVER
StartType      = 3               ; SERVICE_DEMAND_START
ErrorControl   = 1               ; SERVICE_ERROR_NORMAL
ServiceBinary  = %12%\KKS.KmdIsKeyboardRkt.sys

;
;--- KKS.KmdIsKeyboardRkt_Device Coinstaller installation ------
;

[DestinationDirs]
KKS.KmdIsKeyboardRkt_Device_CoInstaller_CopyFiles = 11

[KKS.KmdIsKeyboardRkt_Device.NT.CoInstallers]
AddReg=KKS.KmdIsKeyboardRkt_Device_CoInstaller_AddReg
CopyFiles=KKS.KmdIsKeyboardRkt_Device_CoInstaller_CopyFiles

[KKS.KmdIsKeyboardRkt_Device_CoInstaller_AddReg]
HKR,,CoInstallers32,0x00010000, "WdfCoInstaller$KMDFCOINSTALLERVERSION$.dll,WdfCoInstaller"

[KKS.KmdIsKeyboardRkt_Device_CoInstaller_CopyFiles]
WdfCoInstaller$KMDFCOINSTALLERVERSION$.dll

[SourceDisksFiles]
WdfCoInstaller$KMDFCOINSTALLERVERSION$.dll=1 ; make sure the number matches with SourceDisksNames

[KKS.KmdIsKeyboardRkt_Device.NT.Wdf]
KmdfService =  KKS.KmdIsKeyboardRkt, KKS.KmdIsKeyboardRkt_wdfsect
[KKS.KmdIsKeyboardRkt_wdfsect]
KmdfLibraryVersion = $KMDFVERSION$

[Strings]
SPSVCINST_ASSOCSERVICE= 0x00000002
ManufacturerName="Kjetil Kristoffer Solberg"
ClassName="Is Keyboard Rootkitted"
DiskName = "Kernel Mode Driver - Is Keyboard Rootkitted Installation Disk"
KKS.KmdIsKeyboardRkt.DeviceDesc = "Is Keyboard Rootkitted Device"
KKS.KmdIsKeyboardRkt.SVCDESC = "Is Keyboard Rootkitted Driver"

*/