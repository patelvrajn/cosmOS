#ifndef UEFI_H
#define UEFI_H

#include <stdint.h>

/* UEFI Data Types UEFI Specification v2.10 2.3.1 
typedef uint8_t     UEFI_BOOLEAN;
typedef int64_t     UEFI_INTN;
typedef uint64_t    UEFI_UINTN;
typedef int8_t      UEFI_INT8;
typedef uint8_t     UEFI_UINT8;
typedef int16_t     UEFI_INT16;
typedef uint16_t    UEFI_UINT16;
typedef int32_t     UEFI_INT32;
typedef uint32_t    UEFI_UINT32;
typedef int64_t     UEFI_INT64;
typedef uint64_t    UEFI_UINT64;
typedef __int128_t  UEFI_INT128;
typedef __uint128_t UEFI_UINT128;
typedef char        UEFI_CHAR8;
typedef char16_t    UEFI_CHAR16;
typedef void        UEFI_VOID;

#define UEFI_CONST const
*/

/*UEFI Specification v2.10 Appendix A */
typedef struct {
    uint32_t TimeLow;
    uint16_t TimeMid;
    uint16_t TimeHighAndVersion;
    uint8_t  ClockSeqHighAndReserved;
    uint8_t  ClockSeqLow;
    uint8_t  Node[6];
} UEFI_GUID;

/* UEFI Data Types UEFI Specification v2.10 2.3.1 */
typedef uint64_t UEFI_STATUS;
typedef void*    UEFI_HANDLE;
typedef void*    UEFI_EVENT;
typedef uint64_t UEFI_LBA;
typedef uint64_t UEFI_TPL;
typedef uint8_t  UEFI_MAC_ADDRESS[32];
typedef uint8_t  UEFI_IPV4_ADDRESS[4];
typedef uint8_t  UEFI_IPV6_ADDRESS[16];
typedef uint8_t  UEFI_IP_ADDRESS[16];

#define IN
#define OUT
#define OPTIONAL

/* The calling convention for UEFI defined functions 
is the x86_64 Microsoft calling convention according
to UEFI Specification v2.10 2.3.4.2.*/
#define UEFI_API __attribute__((ms_abi))

/* UEFI Specification v2.10 4.2.1 */
typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} UEFI_TABLE_HEADER;

/* UEFI Specification v2.10 7.1.1 */
typedef void (UEFI_API* UEFI_EVENT_NOTIFY) (
    IN UEFI_EVENT Event,
    IN void*      Context
);

typedef UEFI_STATUS (UEFI_API* UEFI_CREATE_EVENT) (
    IN  uint32_t          Type,
    IN  UEFI_TPL          NotifyTpl,
    IN  UEFI_EVENT_NOTIFY NotifyFunction,
    IN  void*             NotifyContext,
    OUT UEFI_EVENT*       Event
);

/* UEFI Specification v2.10 7.1.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_CREATE_EVENT_EX) (
    IN uint32_t                   Type,
    IN UEFI_TPL                   NotifyTpl,
    OPTIONAL IN UEFI_EVENT_NOTIFY NotifyFunction,
    OPTIONAL IN const void*       NotifyContext,
    OPTIONAL IN const UEFI_GUID*  EventGroup,
    OUT UEFI_EVENT*               Event
);

/* UEFI Specification v2.10 7.1.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_CLOSE_EVENT) (
    IN UEFI_EVENT Event
);

/* UEFI Specification v2.10 7.1.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_SIGNAL_EVENT) (
    IN UEFI_EVENT Event
);

/* UEFI Specification v2.10 7.1.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_WAIT_FOR_EVENT) (
    IN uint64_t    NumberOfEvents,
    IN UEFI_EVENT* Event,
    OUT uint64_t*  Index
);

/* UEFI Specification v2.10 7.1.6 */
typedef UEFI_STATUS (UEFI_API* UEFI_CHECK_EVENT) (
    IN UEFI_EVENT Event
);

/* UEFI Specification v2.10 7.1.7 */
typedef enum {
    TimerCancel,
    TimerPeriodic,
    TimerRelative
} UEFI_TIMER_DELAY;

typedef UEFI_STATUS (UEFI_API* UEFI_SET_TIMER) (
    IN UEFI_EVENT       Event,
    IN UEFI_TIMER_DELAY Type,
    IN uint64_t         TriggerTime
);

/* UEFI Specification v2.10 7.1.8 */
typedef UEFI_TPL (UEFI_API* UEFI_RAISE_TPL) (
    IN UEFI_TPL NewTpl
);

/* UEFI Specification v2.10 7.1.9 */
typedef void (UEFI_API* UEFI_RESTORE_TPL) (
    IN UEFI_TPL OldTpl
);

/* UEFI Specification v2.10 7.2.1 */
typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
} UEFI_ALLOCATE_TYPE;

typedef enum {
    UefiReservedMemoryType,
    UefiLoaderCode,
    UefiLoaderData,
    UefiBootServicesCode,
    UefiBootServicesData,
    UefiRuntimeServicesCode,
    UefiRuntimeServicesData,
    UefiConventionalMemory,
    UefiUnusableMemory,
    UefiACPIReclaimMemory,
    UefiACPIMemoryNVS,
    UefiMemoryMappedIO,
    UefiMemoryMappedIOPortSpace,
    UefiPalCode,
    UefiPersistentMemory,
    UefiUnacceptedMemoryType,
    UefiMaxMemoryType
} UEFI_MEMORY_TYPE;

typedef uint64_t UEFI_PHYSICAL_ADDRESS;

typedef UEFI_STATUS (UEFI_API* UEFI_ALLOCATE_PAGES) (
    IN UEFI_ALLOCATE_TYPE         Type,
    IN UEFI_MEMORY_TYPE           MemoryType,
    IN uint64_t                   Pages,
    IN OUT UEFI_PHYSICAL_ADDRESS* Memory
);

/* UEFI Specification v2.10 7.2.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_FREE_PAGES) (
    IN UEFI_PHYSICAL_ADDRESS Memory,
    IN uint64_t              Pages
);

/* UEFI Specification v2.10 7.2.3 */
typedef uint64_t UEFI_VIRTUAL_ADDRESS;

typedef struct {
    uint32_t              Type;
    UEFI_PHYSICAL_ADDRESS PhysicalStart;
    UEFI_VIRTUAL_ADDRESS  VirtualStart;
    uint64_t              NumberOfPages;
    uint64_t              Attribute;
} UEFI_MEMORY_DESCRIPTOR;

typedef UEFI_STATUS (UEFI_API* UEFI_GET_MEMORY_MAP) (
    IN OUT uint64_t* MemoryMapSize,
    OUT UEFI_MEMORY_DESCRIPTOR* MemoryMap,
    OUT uint64_t* MapKey,
    OUT uint64_t* DescriptorSize,
    OUT uint32_t* DescriptorVersion
);

/* UEFI Specification v2.10 7.2.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_ALLOCATE_POOL) (
    IN UEFI_MEMORY_TYPE PoolType,
    IN uint64_t Size,
    OUT void** Buffer
);

/* UEFI Specification v2.10 7.2.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_FREE_POOL) (
    IN void* Buffer
);

/* UEFI Specification v2.10 7.3.2 */
typedef enum {
    UEFI_NATIVE_INTERFACE
} UEFI_INTERFACE_TYPE;

typedef UEFI_STATUS (UEFI_API* UEFI_INSTALL_PROTOCOL_INTERFACE) (
    IN OUT UEFI_HANDLE*    Handle,
    IN UEFI_GUID*          Protocol,
    IN UEFI_INTERFACE_TYPE InterfaceType,
    IN void*               Interface
);

/* UEFI Specification v2.10 7.3.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_UNINSTALL_PROTOCOL_INTERFACE) (
    IN UEFI_HANDLE Handle,
    IN UEFI_GUID*  Protocol,
    IN void*       Interface
);

/* UEFI Specification v2.10 7.3.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_REINSTALL_PROTOCOL_INTERFACE) (
    IN UEFI_HANDLE Handle,
    IN UEFI_GUID*  Protocol,
    IN void*       OldInterface,
    IN void*       NewInterface
);

/* UEFI Specification v2.10 7.3.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_REGISTER_PROTOCOL_NOTIFY) (
    IN UEFI_GUID* Protocol,
    IN UEFI_EVENT Event,
    OUT void**    Registration
);

/* UEFI Specification v2.10 7.3.6 */
typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol
} UEFI_LOCATE_SEARCH_TYPE;

typedef UEFI_STATUS (UEFI_API* UEFI_LOCATE_HANDLE) (
    IN UEFI_LOCATE_SEARCH_TYPE SearchType,
    OPTIONAL IN UEFI_GUID*     Protocol,
    OPTIONAL IN void*          SearchKey,
    IN OUT uint64_t*           BufferSize,
    OUT UEFI_HANDLE*           Buffer
);

/* UEFI Specification v2.10 7.3.7 */
typedef UEFI_STATUS (UEFI_API* UEFI_HANDLE_PROTOCOL) (
    IN UEFI_HANDLE Handle,
    IN UEFI_GUID   Protocol,
    OUT void**     Interface
);

/* UEFI Specification v2.10 10.2 */
typedef struct {
    uint8_t Type;
    uint8_t SubType;
    uint8_t Length[2];
} UEFI_DEVICE_PATH_PROTOCOL;

/* UEFI Specification v2.10 7.3.8 */
typedef UEFI_STATUS (UEFI_API* UEFI_LOCATE_DEVICE_PATH) (
    IN UEFI_GUID*                      Protocol,
    IN OUT UEFI_DEVICE_PATH_PROTOCOL** DevicePath,
    OUT UEFI_HANDLE*                   Device
);

/* UEFI Specification v2.10 7.3.9 */
typedef UEFI_STATUS (UEFI_API* UEFI_OPEN_PROTOCOL) (
    IN UEFI_HANDLE      Handle,
    IN UEFI_GUID*       Protocol,
    OPTIONAL OUT void** Interface,
    IN UEFI_HANDLE      AgentHandle,
    IN UEFI_HANDLE      ControllerHandle,
    IN uint32_t         Attributes
);

/* UEFI Specification v2.10 7.3.10 */
typedef UEFI_STATUS (UEFI_API* UEFI_CLOSE_PROTOCOL) (
    IN UEFI_HANDLE Handle,
    IN UEFI_GUID*  Protocol,
    IN UEFI_HANDLE AgentHandle,
    IN UEFI_HANDLE ControllerHandle
);

/* UEFI Specification v2.10 7.3.11 */
typedef struct {
    UEFI_HANDLE AgentHandle;
    UEFI_HANDLE ControllerHandle;
    uint32_t    Attributes;
    uint32_t    OpenCount;
} UEFI_OPEN_PROTOCOL_INFORMATION_ENTRY;

typedef UEFI_STATUS (UEFI_API* UEFI_OPEN_PROTOCOL_INFORMATION) (
    IN  UEFI_HANDLE                            Handle,
    IN  UEFI_GUID*                             Protocol,
    OUT UEFI_OPEN_PROTOCOL_INFORMATION_ENTRY** EntryBuffer,
    OUT uint64_t*                              EntryCount
);

/* UEFI Specification v2.10 7.3.12 */
typedef UEFI_STATUS (UEFI_API* UEFI_CONNECT_CONTROLLER) (
    IN UEFI_HANDLE                         ControllerHandle,
    OPTIONAL IN UEFI_HANDLE*               DriverImageHandle,
    OPTIONAL IN UEFI_DEVICE_PATH_PROTOCOL* RemainingDevicePath,
    IN uint8_t                             Recursive
);

/* UEFI Specification v2.10 7.3.13 */
typedef UEFI_STATUS (UEFI_API* UEFI_DISCONNECT_CONTROLLER) (
    IN UEFI_HANDLE          ControllerHandle,
    OPTIONAL IN UEFI_HANDLE DriverImageHandle,
    OPTIONAL IN UEFI_HANDLE ChildHandle
);

/* UEFI Specification v2.10 7.3.14 */
typedef UEFI_STATUS (UEFI_API* UEFI_PROTOCOLS_PER_HANDLE) (
    IN UEFI_HANDLE   Handle,
    OUT UEFI_GUID*** ProtocolBuffer,
    OUT uint64_t*    ProtocolBufferCount
);

/* UEFI Specification v2.10 7.3.15 */
typedef UEFI_STATUS (UEFI_API *UEFI_LOCATE_HANDLE_BUFFER) (
    IN UEFI_LOCATE_SEARCH_TYPE SearchType,
    OPTIONAL IN UEFI_GUID*     Protocol,
    OPTIONAL IN void*          SearchKey,
    OUT uint64_t*              NoHandles,
    OUT UEFI_HANDLE**          Buffer
);

/* UEFI Specification v2.10 7.3.16 */
typedef UEFI_STATUS (UEFI_API* UEFI_LOCATE_PROTOCOL) (
    IN UEFI_GUID*     Protocol,
    OPTIONAL IN void* Registration,
    OUT void**        Interface
);

/* UEFI Specification v2.10 7.3.17 */
typedef UEFI_STATUS (UEFI_API* UEFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
    IN OUT UEFI_HANDLE *Handle,
    ...
);

/* UEFI Specification v2.10 7.3.18 */
typedef UEFI_STATUS (UEFI_API* UEFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES) (
    IN UEFI_HANDLE Handle,
    ...
);

/* UEFI Specification v2.10 7.4.1 */
typedef UEFI_STATUS (UEFI_API* UEFI_IMAGE_LOAD) (
    IN uint8_t                             BootPolicy,
    IN UEFI_HANDLE                         ParentImageHandle,
    OPTIONAL IN UEFI_DEVICE_PATH_PROTOCOL* DevicePath,
    OPTIONAL IN void*                      SourceBuffer,
    IN uint64_t                            SourceSize,
    OUT UEFI_HANDLE*                       ImageHandle
);

/* UEFI Specification v2.10 7.4.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_IMAGE_START) (
    IN UEFI_HANDLE          ImageHandle,
    OUT uint64_t*           ExitDataSize,   
    OPTIONAL OUT char16_t** ExitData 
);

/* UEFI Specification v2.10 7.4.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_IMAGE_UNLOAD) (
    IN UEFI_HANDLE ImageHandle
);

/* UEFI Specification v2.10 7.4.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_EXIT) (
    IN UEFI_HANDLE        ImageHandle,
    IN UEFI_STATUS        ExitStatus,
    IN uint64_t           ExitDataSize,
    OPTIONAL IN char16_t* ExitData
);

/* UEFI Specification v2.10 7.4.6 */
typedef UEFI_STATUS (UEFI_API* UEFI_EXIT_BOOT_SERVICES) (
    IN UEFI_HANDLE ImageHandle,
    IN uint64_t    MapKey
);

/* UEFI Specification v2.10 7.5.1 */
typedef UEFI_STATUS (UEFI_API* UEFI_SET_WATCHDOG_TIMER) (
    IN uint64_t           Timeout,
    IN uint64_t           WatchdogCode,
    IN uint64_t           DataSize,
    OPTIONAL IN char16_t* WatchdogData
);

/* UEFI Specification v2.10 7.5.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_STALL) (
    IN uint64_t Microseconds
);

/* UEFI Specification v2.10 7.5.3 */
typedef void (UEFI_API* UEFI_COPY_MEM) (
    IN void*    Destination,
    IN void*    Source,
    IN uint64_t Length
);

/* UEFI Specification v2.10 7.5.4 */
typedef void (UEFI_API* UEFI_SET_MEM) (
    IN void*    Buffer,
    IN uint64_t Size,
    IN uint8_t  Value
);

/* UEFI Specification v2.10 7.5.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_GET_NEXT_MONOTONIC_COUNT) (
    OUT uint64_t* Count
);

/* UEFI Specification v2.10 7.5.6 */
typedef UEFI_STATUS (UEFI_API* UEFI_INSTALL_CONFIGURATION_TABLE) (
    IN UEFI_GUID* Guid,
    IN void*      Table
);

/* UEFI Specification v2.10 7.5.7 */
typedef UEFI_STATUS (UEFI_API* UEFI_CALCULATE_CRC32) (
    IN  void*     Data,
    IN  uint64_t  DataSize,
    OUT uint32_t* Crc32
);

/* UEFI Specification v2.10 4.4.1 */
typedef struct {
    UEFI_TABLE_HEADER                           Hdr;
    UEFI_RAISE_TPL                              RaiseTPL;
    UEFI_RESTORE_TPL                            RestoreTPL;
    UEFI_ALLOCATE_PAGES                         AllocatePages;
    UEFI_FREE_PAGES                             FreePages;
    UEFI_GET_MEMORY_MAP                         GetMemoryMap;
    UEFI_ALLOCATE_POOL                          AllocatePool;
    UEFI_FREE_POOL                              FreePool;
    UEFI_CREATE_EVENT                           CreateEvent;
    UEFI_SET_TIMER                              SetTimer;
    UEFI_WAIT_FOR_EVENT                         WaitForEvent;
    UEFI_SIGNAL_EVENT                           SignalEvent;
    UEFI_CLOSE_EVENT                            CloseEvent;
    UEFI_CHECK_EVENT                            CheckEvent;
    UEFI_INSTALL_PROTOCOL_INTERFACE             InstallProtocolInterface;
    UEFI_REINSTALL_PROTOCOL_INTERFACE           ReinstallProtocolInterface;
    UEFI_UNINSTALL_PROTOCOL_INTERFACE           UninstallProtocolInterface;
    UEFI_HANDLE_PROTOCOL                        HandleProtocol;
    UEFI_REGISTER_PROTOCOL_NOTIFY               RegisterProtocolNotify;
    UEFI_LOCATE_HANDLE                          LocateHandle;
    UEFI_LOCATE_DEVICE_PATH                     LocateDevicePath;
    UEFI_INSTALL_CONFIGURATION_TABLE            InstallConfigurationTable;
    UEFI_IMAGE_LOAD                             LoadImage;
    UEFI_IMAGE_START                            StartImage;
    UEFI_EXIT                                   Exit;
    UEFI_IMAGE_UNLOAD                           UnloadImage;
    UEFI_EXIT_BOOT_SERVICES                     ExitBootServices;
    UEFI_GET_NEXT_MONOTONIC_COUNT               GetNextMonotonicCount;
    UEFI_STALL                                  Stall;
    UEFI_SET_WATCHDOG_TIMER                     SetWatchdogTimer;
    UEFI_CONNECT_CONTROLLER                     ConnectController;
    UEFI_DISCONNECT_CONTROLLER                  DisconnectController;
    UEFI_OPEN_PROTOCOL                          OpenProtocol;
    UEFI_CLOSE_PROTOCOL                         CloseProtocol;
    UEFI_OPEN_PROTOCOL_INFORMATION              OpenProtocolInformation;
    UEFI_PROTOCOLS_PER_HANDLE                   ProtocolsPerHandle;
    UEFI_LOCATE_HANDLE_BUFFER                   LocateHandleBuffer;
    UEFI_LOCATE_PROTOCOL                        LocateProtocol;
    UEFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES   InstallMultipleProtocolInterfaces;
    UEFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES UninstallMultipleProtocolInterfaces;
    UEFI_CALCULATE_CRC32                        CalculateCrc32;
    UEFI_COPY_MEM                               CopyMem;
    UEFI_SET_MEM                                SetMem;
    UEFI_CREATE_EVENT_EX                        CreateEventEx;        
} UEFI_BOOT_SERVICES;

/* UEFI Specification v2.10 8.2.1 */
typedef UEFI_STATUS (UEFI_API* UEFI_GET_VARIABLE) (
    IN char16_t*           VariableName,
    IN UEFI_GUID*          VendorGuid,
    OPTIONAL OUT uint32_t* Attributes,
    IN OUT uint64_t*       DataSize,
    OPTIONAL OUT void*     Data
);

/* UEFI Specification v2.10 8.2.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_GET_NEXT_VARIABLE_NAME) (
    IN OUT uint64_t*  VariableNameSize,
    IN OUT char16_t*  VariableName,
    IN OUT UEFI_GUID* VendorGuid
);

/* UEFI Specification v2.10 8.2.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_SET_VARIABLE) (
    IN char16_t*  VariableName,
    IN UEFI_GUID* VendorGuid,
    IN uint32_t   Attributes,
    IN uint64_t   DataSize,
    IN void*      Data
);

/* UEFI Specification v2.10 8.2.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_QUERY_VARIABLE_INFO) (
    IN  uint32_t  Attributes,
    OUT uint64_t* MaximumVariableStorageSize,
    OUT uint64_t* RemainingVariableStorageSize,
    OUT uint64_t* MaximumVariableSize
);

/* UEFI Specification v2.10 8.3.1 */
typedef struct {
    uint32_t Resolution;
    uint32_t Accuracy;
    uint8_t  SetsToZero;
} UEFI_TIME_CAPABILITIES;

typedef struct {
    uint16_t Year;
    uint8_t  Month;
    uint8_t  Day;
    uint8_t  Hour;
    uint8_t  Minute;
    uint8_t  Second;
    uint8_t  Pad1;
    uint32_t Nanosecond;
    int16_t  TimeZone; 
    uint8_t  Daylight;
    uint8_t  Pad2;
} UEFI_TIME;

typedef UEFI_STATUS (UEFI_API* UEFI_GET_TIME) (
    OUT UEFI_TIME*              Time,
    OUT UEFI_TIME_CAPABILITIES* Capabilities
);

/* UEFI Specification v2.10 8.3.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_SET_TIME) (
    IN UEFI_TIME* Time
);

/* UEFI Specification v2.10 8.3.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_GET_WAKEUP_TIME) (
    OUT uint8_t*   Enabled,
    OUT uint8_t*   Pending,
    OUT UEFI_TIME* Time
);

/* UEFI Specification v2.10 8.3.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_SET_WAKEUP_TIME) (
    IN uint8_t             Enable,
    OPTIONAL IN UEFI_TIME* Time
);

/* UEFI Specification v2.10 8.4.1 */
typedef UEFI_STATUS (UEFI_API* UEFI_SET_VIRTUAL_ADDRESS_MAP) (
    IN uint64_t                MemoryMapSize,
    IN uint64_t                DescriptorSize,
    IN uint32_t                DescriptorVersion,
    IN UEFI_MEMORY_DESCRIPTOR* VirtualMap
);

/* UEFI Specification v2.10 8.4.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_CONVERT_POINTER) (
    IN uint64_t DebugDisposition,
    IN void**   Address
);

/* UEFI Specification v2.10 8.5.1.1 */
typedef enum {
    UefiResetCold,
    UefiResetWarm,
    UefiResetShutdown,
    UefiResetPlatformSpecific
} UEFI_RESET_TYPE;

typedef void (UEFI_API* UEFI_RESET_SYSTEM) (
    IN UEFI_RESET_TYPE ResetType,
    IN UEFI_STATUS     ResetStatus,
    IN uint64_t        DataSize,
    OPTIONAL IN void*  ResetData 
);

/* UEFI Specification v2.10 8.5.2.1 */
typedef UEFI_STATUS (UEFI_API* UEFI_GET_NEXT_HIGH_MONO_COUNT) (
    OUT uint32_t* HighCount
);

/* UEFI Specification v2.10 8.5.3.1 */
typedef struct {
    UEFI_GUID CapsuleGuid;
    uint32_t  HeaderSize;
    uint32_t  Flags;
    uint32_t  CapsuleImageSize;
} UEFI_CAPSULE_HEADER;

typedef UEFI_STATUS (UEFI_API* UEFI_UPDATE_CAPSULE) (
    IN UEFI_CAPSULE_HEADER**          CapsuleHeaderArray,
    IN uint64_t                       CapsuleCount,
    OPTIONAL IN UEFI_PHYSICAL_ADDRESS ScatterGatherList
);

/* UEFI Specification v2.10 8.5.3.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_QUERY_CAPSULE_CAPABILITIES) (
    IN  UEFI_CAPSULE_HEADER** CapsuleHeaderArray, 
    IN  uint64_t              CapsuleCount,
    OUT uint64_t*             MaximumCapsuleSize,
    OUT UEFI_RESET_TYPE*      ResetType
);

/* UEFI Specification v2.10 4.5.1 */
typedef struct {
    UEFI_TABLE_HEADER               Hdr;
    UEFI_GET_TIME                   GetTime;
    UEFI_SET_TIME                   SetTime;
    UEFI_GET_WAKEUP_TIME            GetWakeupTime;
    UEFI_SET_WAKEUP_TIME            SetWakeupTime;
    UEFI_SET_VIRTUAL_ADDRESS_MAP    SetVirtualAddressMap;
    UEFI_CONVERT_POINTER            ConvertPointer;
    UEFI_GET_VARIABLE               GetVariable;
    UEFI_GET_NEXT_VARIABLE_NAME     GetNextVariableName;
    UEFI_SET_VARIABLE               SetVariable;
    UEFI_GET_NEXT_HIGH_MONO_COUNT   GetNextHighMonotonicCount;
    UEFI_RESET_SYSTEM               ResetSystem;
    UEFI_UPDATE_CAPSULE             UpdateCapsule;
    UEFI_QUERY_CAPSULE_CAPABILITIES QueryCapsuleCapabilities;
    UEFI_QUERY_VARIABLE_INFO        QueryVariableInfo;
} UEFI_RUNTIME_SERVICES;

/* UEFI Specification v2.10 4.6.1 */
typedef struct{
    UEFI_GUID VendorGuid;
    void*     VendorTable;
} UEFI_CONFIGURATION_TABLE;

// Forward declaration because of circular dependencies.
typedef struct UEFI_SIMPLE_TEXT_INPUT_PROTOCOL UEFI_SIMPLE_TEXT_INPUT_PROTOCOL;

/* UEFI Specification v2.10 12.3.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_INPUT_RESET) (
    IN UEFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
    IN uint8_t                          ExtendedVerification
);

/* UEFI Specification v2.10 12.3.3 */
typedef struct {
    uint16_t ScanCode;
    char16_t UnicodeChar;
} UEFI_INPUT_KEY;

typedef UEFI_STATUS (UEFI_API* UEFI_INPUT_READ_KEY) (
    IN  UEFI_SIMPLE_TEXT_INPUT_PROTOCOL* This,
    OUT UEFI_INPUT_KEY*                  Key
);

/* UEFI Specification v2.10 12.3.1 */
typedef struct UEFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    UEFI_INPUT_RESET    Reset;
    UEFI_INPUT_READ_KEY ReadKeyStroke;
    UEFI_EVENT          WaitForKey;
} UEFI_SIMPLE_TEXT_INPUT_PROTOCOL;

// Forward declaration because of circular dependencies.
typedef struct UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* UEFI Specification v2.10 12.4.1 */
typedef struct {
    int32_t MaxMode;
    int32_t CurrentMode;
    int32_t Attribute;
    int32_t CursorColumn;
    int32_t CursorRow;
    uint8_t CursorVisible;
} SIMPLE_TEXT_OUTPUT_MODE;

/* UEFI Specification v2.10 12.4.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_RESET) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
    IN uint8_t                           ExtendedVerification
);

/* UEFI Specification v2.10 12.4.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_STRING) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
    IN char16_t*                         String
);

/* UEFI Specification v2.10 12.4.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_TEST_STRING) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
    IN char16_t                          String
);

/* UEFI Specification v2.10 12.4.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_QUERY_MODE) (
    IN  UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
    IN  uint64_t                          ModeNumber,
    OUT uint64_t*                         Columns,
    OUT uint64_t*                         Rows
);

/* UEFI Specification v2.10 12.4.6 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_SET_MODE) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
    IN uint64_t                          ModeNumber
);

/* UEFI Specification v2.10 12.4.7 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_SET_ATTRIBUTE) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
    IN uint64_t                          Attribute
);

#define UEFI_FOREGROUND_BLACK        0x00
#define UEFI_FOREGROUND_BLUE         0x01
#define UEFI_FOREGROUND_GREEN        0x02
#define UEFI_FOREGROUND_CYAN         0x03
#define UEFI_FOREGROUND_RED          0x04
#define UEFI_FOREGROUND_MAGENTA      0x05
#define UEFI_FOREGROUND_BROWN        0x06
#define UEFI_FOREGROUND_LIGHTGRAY    0x07
#define UEFI_FOREGROUND_DARKGRAY     0x08
#define UEFI_FOREGROUND_LIGHTBLUE    0x09
#define UEFI_FOREGROUND_LIGHTGREEN   0x0A
#define UEFI_FOREGROUND_LIGHTCYAN    0x0B
#define UEFI_FOREGROUND_LIGHTRED     0x0C
#define UEFI_FOREGROUND_LIGHTMAGENTA 0x0D
#define UEFI_FOREGROUND_YELLOW       0x0E
#define UEFI_FOREGROUND_WHITE        0x0F

#define UEFI_BACKGROUND_BLACK     0x00
#define UEFI_BACKGROUND_BLUE      0x10
#define UEFI_BACKGROUND_GREEN     0x20
#define UEFI_BACKGROUND_CYAN      0x30
#define UEFI_BACKGROUND_RED       0x40
#define UEFI_BACKGROUND_MAGENTA   0x50
#define UEFI_BACKGROUND_BROWN     0x60
#define UEFI_BACKGROUND_LIGHTGRAY 0x70

#define UEFI_TEXT_ATTR(Foreground, Background) (Foreground | Background)

/* UEFI Specification v2.10 12.4.8 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_CLEAR_SCREEN) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This
);

/* UEFI Specification v2.10 12.4.9 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_SET_CURSOR_POSITION) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* This,
    IN uint64_t                          Column,
    IN uint64_t                          Row
);

/* UEFI Specification v2.10 12.4.10 */
typedef UEFI_STATUS (UEFI_API* UEFI_TEXT_ENABLE_CURSOR) (
    IN UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    IN uint8_t                          Visible
);

/* UEFI Specification v2.10 12.4.1 */
typedef struct UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    UEFI_TEXT_RESET               Reset;
    UEFI_TEXT_STRING              OutputString;
    UEFI_TEXT_TEST_STRING         TestString;
    UEFI_TEXT_QUERY_MODE          QueryMode;
    UEFI_TEXT_SET_MODE            SetMode;
    UEFI_TEXT_SET_ATTRIBUTE       SetAttribute;
    UEFI_TEXT_CLEAR_SCREEN        ClearScreen;
    UEFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
    UEFI_TEXT_ENABLE_CURSOR       EnableCursor;
    SIMPLE_TEXT_OUTPUT_MODE       *Mode;
} UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* UEFI Specification v2.10 4.3.1 */
typedef struct {
    UEFI_TABLE_HEADER                 Hdr;
    char16_t*                         FirmwareVendor;
    uint32_t                          FirmwareRevision;
    UEFI_HANDLE                       ConsoleInHandle;
    UEFI_SIMPLE_TEXT_INPUT_PROTOCOL*  ConIn;
    UEFI_HANDLE                       ConsoleOutHandle;
    UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* ConOut;
    UEFI_HANDLE                       StandardErrorHandle;
    UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* StdErr;
    UEFI_RUNTIME_SERVICES*            RuntimeServices;
    UEFI_BOOT_SERVICES*               BootServices;
    uint64_t                          NumberOfTableEntries;
    UEFI_CONFIGURATION_TABLE*         ConfigurationTable;
} UEFI_SYSTEM_TABLE;

/* UEFI Specification v2.10 4.1.1 */
typedef UEFI_STATUS (UEFI_API* UEFI_IMAGE_ENTRY_POINT) (
    IN UEFI_HANDLE       ImageHandle,
    IN UEFI_SYSTEM_TABLE *SystemTable
);

/* UEFI Success Code UEFI Specification v2.10 Appendix D */
#define UEFI_SUCCESS 0

/* UEFI Error Macros UEFI Specification v2.10 Appendix D */
#define UEFI_ERROR_BIT       0x8000000000000000
#define UEFI_ENCODE_ERROR(x) (UEFI_ERROR_BIT | (x))
#define UEFI_IS_ERROR(x)     ((int64_t)((uint64_t)(x)) < 0) // Convert error to 64 bit signed and check sign bit.

/* UEFI Warning Statuses UEFI Specification v2.10 Appendix D */
#define UEFI_WARN_UNKNOWN_GLYPH    1
#define UEFI_WARN_DELETE_FAILURE   2
#define UEFI_WARN_WRITE_FAILURE    3
#define UEFI_WARN_BUFFER_TOO_SMALL 4
#define UEFI_WARN_STALE_DATA       5
#define UEFI_WARN_FILE_SYSTEM      6
#define UEFI_WARN_RESET_REQUIRED   7

/* UEFI Error Statuses UEFI Specification v2.10 Appendix D */
#define UEFI_LOAD_ERROR           UEFI_ENCODE_ERROR(1)
#define UEFI_INVALID_PARAMETER    UEFI_ENCODE_ERROR(2)
#define UEFI_UNSUPPORTED          UEFI_ENCODE_ERROR(3)
#define UEFI_BAD_BUFFER_SIZE      UEFI_ENCODE_ERROR(4)
#define UEFI_BUFFER_TOO_SMALL     UEFI_ENCODE_ERROR(5)
#define UEFI_NOT_READY            UEFI_ENCODE_ERROR(6)
#define UEFI_DEVICE_ERROR         UEFI_ENCODE_ERROR(7)
#define UEFI_WRITE_PROTECTED      UEFI_ENCODE_ERROR(8)
#define UEFI_OUT_OF_RESOURCES     UEFI_ENCODE_ERROR(9)
#define UEFI_VOLUME_CORRUPTED     UEFI_ENCODE_ERROR(10)
#define UEFI_VOLUME_FULL          UEFI_ENCODE_ERROR(11)
#define UEFI_NO_MEDIA             UEFI_ENCODE_ERROR(12)
#define UEFI_MEDIA_CHANGED        UEFI_ENCODE_ERROR(13)
#define UEFI_NOT_FOUND            UEFI_ENCODE_ERROR(14)
#define UEFI_ACCESS_DENIED        UEFI_ENCODE_ERROR(15)
#define UEFI_NO_RESPONSE          UEFI_ENCODE_ERROR(16)
#define UEFI_NO_MAPPING           UEFI_ENCODE_ERROR(17)
#define UEFI_TIMEOUT              UEFI_ENCODE_ERROR(18)
#define UEFI_NOT_STARTED          UEFI_ENCODE_ERROR(19)
#define UEFI_ALREADY_STARED       UEFI_ENCODE_ERROR(20)
#define UEFI_ABORTED              UEFI_ENCODE_ERROR(21)
#define UEFI_ICMP_ERROR           UEFI_ENCODE_ERROR(22)
#define UEFI_TFTP_ERROR           UEFI_ENCODE_ERROR(23)
#define UEFI_PROTOCOL_ERROR       UEFI_ENCODE_ERROR(24)
#define UEFI_INCOMPATIBLE_VERSION UEFI_ENCODE_ERROR(25)
#define UEFI_SECURITY_VIOLATION   UEFI_ENCODE_ERROR(26)
#define UEFI_CRC_ERROR            UEFI_ENCODE_ERROR(27)
#define UEFI_END_OF_MEDIA         UEFI_ENCODE_ERROR(28)
// ERRORS 29 - 30 Missing from specifications?
#define UEFI_END_OF_FILE          UEFI_ENCODE_ERROR(31)
#define UEFI_INVALID_LANGUAGE     UEFI_ENCODE_ERROR(32)
#define UEFI_COMPROMISED_DATA     UEFI_ENCODE_ERROR(33)
#define UEFI_IP_ADDRESS_CONFLICT  UEFI_ENCODE_ERROR(34)
#define UEFI_HTTP_ERROR           UEFI_ENCODE_ERROR(35)

#endif
/* END OF UEFI.H */
