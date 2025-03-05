#pragma once

#include <stdint.h>

/******************************************************************************
UEFI BASIC DECLARATIONS
******************************************************************************/

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

/******************************************************************************
UEFI TABLE HEADER DECLARATION
******************************************************************************/

/* UEFI Specification v2.10 4.2.1 */
typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} UEFI_TABLE_HEADER;

/******************************************************************************
UEFI BOOT SERVICES DECLARATIONS
******************************************************************************/

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

// Event types
#define EVT_TIMER                         0x80000000
#define EVT_RUNTIME                       0x40000000
#define EVT_NOTIFY_WAIT                   0x00000100
#define EVT_NOTIFY_SIGNAL                 0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES     0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

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

// Task priority levels
#define TPL_APPLICATION 4
#define TPL_CALLBACK    8
#define TPL_NOTIFY      16
#define TPL_HIGH_LEVEL  31

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
    UefiReservedMemoryType,      // UEFI Memory Type 0
    UefiLoaderCode,              // UEFI Memory Type 1
    UefiLoaderData,              // UEFI Memory Type 2
    UefiBootServicesCode,        // UEFI Memory Type 3
    UefiBootServicesData,        // UEFI Memory Type 4
    UefiRuntimeServicesCode,     // UEFI Memory Type 5
    UefiRuntimeServicesData,     // UEFI Memory Type 6
    UefiConventionalMemory,      // UEFI Memory Type 7
    UefiUnusableMemory,          // UEFI Memory Type 8
    UefiACPIReclaimMemory,       // UEFI Memory Type 9
    UefiACPIMemoryNVS,           // UEFI Memory Type 10
    UefiMemoryMappedIO,          // UEFI Memory Type 11
    UefiMemoryMappedIOPortSpace, // UEFI Memory Type 12
    UefiPalCode,                 // UEFI Memory Type 13
    UefiPersistentMemory,        // UEFI Memory Type 14
    UefiUnacceptedMemoryType,    // UEFI Memory Type 15
    UefiMaxMemoryType            // UEFI Memory Type 16
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
    IN OUT uint64_t*            MemoryMapSize,
    OUT UEFI_MEMORY_DESCRIPTOR* MemoryMap,
    OUT uint64_t*               MapKey,
    OUT uint64_t*               DescriptorSize,
    OUT uint32_t*               DescriptorVersion
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
    IN UEFI_GUID*  Protocol,
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

#define UEFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL  0x00000001
#define UEFI_OPEN_PROTOCOL_GET_PROTOCOL        0x00000002
#define UEFI_OPEN_PROTOCOL_TEST_PROTOCOL       0x00000004
#define UEFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define UEFI_OPEN_PROTOCOL_BY_DRIVER           0x00000010
#define UEFI_OPEN_PROTOCOL_EXCLUSIVE           0x00000020

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
typedef UEFI_STATUS (UEFI_API* UEFI_LOCATE_HANDLE_BUFFER) (
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
    void*                                       Reserved;
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

/******************************************************************************
UEFI RUNTIME SERVICES DECLARATIONS
******************************************************************************/

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

/******************************************************************************
UEFI CONFIGURATION TABLE DECLARATION
******************************************************************************/

/* UEFI Specification v2.10 4.6.1 */
typedef struct{
    UEFI_GUID VendorGuid;
    void*     VendorTable;
} UEFI_CONFIGURATION_TABLE;

/******************************************************************************
UEFI SIMPLE TEXT INPUT PROTOCOL DECLARATIONS
******************************************************************************/

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

/******************************************************************************
UEFI SIMPLE TEXT OUTPUT PROTOCOL DECLARATIONS
******************************************************************************/

// Forward declaration because of circular dependencies.
typedef struct UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL UEFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

/* UEFI Specification v2.10 12.4.1 */
typedef struct {
    int32_t MaxMode;
    int32_t Mode;
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

/******************************************************************************
UEFI SYSTEM TABLE AND ENTRY POINT DECLARATIONS
******************************************************************************/

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

/******************************************************************************
UEFI STATUSES
******************************************************************************/

/* UEFI Success Code UEFI Specification v2.10 Appendix D */
#define UEFI_SUCCESS 0

/* UEFI Error Macros UEFI Specification v2.10 Appendix D */
#define UEFI_ERROR_BIT       0x8000000000000000
#define UEFI_ENCODE_ERROR(x) (UEFI_ERROR_BIT | (x))
#define UEFI_IS_ERROR(x)     ((int64_t)((uint64_t)(x)) < 0) // Convert error to 64 bit signed and check sign bit.

#define UEFI_PRINT_ERROR(SystemTable, Status, ErrorString) \
  if (UEFI_IS_ERROR(status)) { \
    uefi_printf (SystemTable, u"UEFI Error %h : %s.\r\nPrint any key to continue...\r\n", Status, ErrorString); \
    uefi_wait_for_keystroke (SystemTable); \
  }

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

/******************************************************************************
UEFI GRAPHICS OUTPUT PROTOCOL
******************************************************************************/

/* UEFI Specification v2.10 12.9.2 */
#define UEFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
{0x9042a9de, 0x23dc, 0x4a38, 0x96, 0xfb, {0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a}}

typedef struct {
    uint32_t RedMask;
    uint32_t GreenMask;
    uint32_t BlueMask;
    uint32_t ReservedMask;
} UEFI_PIXEL_BITMASK;

typedef enum {
    PixelRedGreenBlueReserved8BitPerColor,
    PixelBlueGreenRedReserved8BitPerColor,
    PixelBitMask,
    PixelBltOnly,
    PixelFormatMax
} UEFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
    uint32_t                   Version;
    uint32_t                   HorizontalResolution;
    uint32_t                   VerticalResolution;
    UEFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
    UEFI_PIXEL_BITMASK         PixelInformation;
    uint32_t                   PixelsPerScanLine;
} UEFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
    uint32_t                              MaxMode;
    uint32_t                              Mode;
    UEFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    uint64_t                              SizeOfInfo;
    UEFI_PHYSICAL_ADDRESS                 FrameBufferBase;
    uint64_t                              FrameBufferSize;
} UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// Forward declaration because of circular dependencies.
typedef struct UEFI_GRAPHICS_OUTPUT_PROTOCOL UEFI_GRAPHICS_OUTPUT_PROTOCOL;

/* UEFI Specification v2.10 12.9.2.1 */
typedef UEFI_STATUS (UEFI_API* UEFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
    IN UEFI_GRAPHICS_OUTPUT_PROTOCOL*           This,
    IN uint32_t                                 ModeNumber,
    OUT uint64_t*                               SizeOfInfo,
    OUT UEFI_GRAPHICS_OUTPUT_MODE_INFORMATION** Info
);

/* UEFI Specification v2.10 12.9.2.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
    IN UEFI_GRAPHICS_OUTPUT_PROTOCOL* This,
    IN uint32_t                       ModeNumber
);

/* UEFI Specification v2.10 12.9.2.3 */
typedef struct {
    uint8_t Blue;
    uint8_t Green;
    uint8_t Red;
    uint8_t Reserved;
} UEFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
    UefiBltVideoFill,
    UefiBltVideoToBltBuffer,
    UefiBltBufferToVideo,
    UefiBltVideoToVideo,
    UefiGraphicsOutputBltOperationMax
} UEFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef UEFI_STATUS (UEFI_API* UEFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
    IN UEFI_GRAPHICS_OUTPUT_PROTOCOL*               This,
    OPTIONAL IN OUT UEFI_GRAPHICS_OUTPUT_BLT_PIXEL* BltBuffer,
    IN UEFI_GRAPHICS_OUTPUT_BLT_OPERATION           BltOperation,
    IN uint64_t                                     SourceX,
    IN uint64_t                                     SourceY,
    IN uint64_t                                     DestinationX,
    IN uint64_t                                     DestinationY,
    IN uint64_t                                     Width,
    IN uint64_t                                     Height,
    OPTIONAL IN uint64_t                            Delta
);

/* UEFI Specification v2.10 12.9.2 */
typedef struct UEFI_GRAPHICS_OUTPUT_PROTOCOL {
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE   SetMode;
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_BLT        Blt;
    UEFI_GRAPHICS_OUTPUT_PROTOCOL_MODE*      Mode;
} UEFI_GRAPHICS_OUTPUT_PROTOCOL;

/******************************************************************************
UEFI LOADED IMAGE PROTOCOL (UEFI Specification v2.10 9.1)
******************************************************************************/

#define UEFI_LOADED_IMAGE_PROTOCOL_GUID \
{0x5B1B31A1, 0x9562, 0x11d2, 0x8E, 0x3F, {0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}}

#define UEFI_LOADED_IMAGE_PROTOCOL_REVISION 0x1000

typedef struct {
    uint32_t                   Revision;
    UEFI_HANDLE                ParentHandle;
    UEFI_SYSTEM_TABLE*         SystemTable;
    UEFI_HANDLE                DeviceHandle;
    UEFI_DEVICE_PATH_PROTOCOL* FilePath;
    void*                      Reserved;
    uint32_t                   LoadOptionsSize;
    void*                      LoadOptions;
    void*                      ImageBase;
    uint64_t                   ImageSize;
    UEFI_MEMORY_TYPE           ImageCodeType;
    UEFI_MEMORY_TYPE           ImageDataType;
    UEFI_IMAGE_UNLOAD          Unload;
} UEFI_LOADED_IMAGE_PROTOCOL;

/******************************************************************************
UEFI SIMPLE FILE SYSTEM AND FILE PROTOCOL (UEFI Specification v2.10 13.4 - 13.5)
******************************************************************************/

// Forward declaration because of circular dependencies.
typedef struct UEFI_FILE_PROTOCOL UEFI_FILE_PROTOCOL;

/* UEFI Specification v2.10 13.5.2 */
// Open Modes
#define UEFI_FILE_MODE_READ   0x0000000000000001
#define UEFI_FILE_MODE_WRITE  0x0000000000000002
#define UEFI_FILE_MODE_CREATE 0x8000000000000000

// File Attributes
#define UEFI_FILE_READ_ONLY  0x0000000000000001
#define UEFI_FILE_HIDDEN     0x0000000000000002
#define UEFI_FILE_SYSTEM     0x0000000000000004
#define UEFI_FILE_RESERVED   0x0000000000000008
#define UEFI_FILE_DIRECTORY  0x0000000000000010
#define UEFI_FILE_ARCHIVE    0x0000000000000020
#define UEFI_FILE_VALID_ATTR 0x0000000000000037

typedef UEFI_STATUS (UEFI_API* UEFI_FILE_OPEN) (
    IN UEFI_FILE_PROTOCOL*   This,
    OUT UEFI_FILE_PROTOCOL** NewHandle,
    IN char16_t*             FileName,
    IN uint64_t              OpenMode,
    IN uint64_t              Attributes
);

/* UEFI Specification v2.10 13.5.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_CLOSE) (
    IN UEFI_FILE_PROTOCOL* This
);

/* UEFI Specification v2.10 13.5.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_DELETE) (
    IN UEFI_FILE_PROTOCOL* This
);

/* UEFI Specification v2.10 13.5.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_READ) (
    IN UEFI_FILE_PROTOCOL* This,
    IN OUT uint64_t*       BufferSize,
    OUT void*              Buffer
);

/* UEFI Specification v2.10 13.5.6 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_WRITE) (
    IN UEFI_FILE_PROTOCOL* This,
    IN OUT uint64_t*       BufferSize,
    IN void*               Buffer
);

/* UEFI Specification v2.10 13.5.7 */
typedef struct {
    UEFI_EVENT  Event;
    UEFI_STATUS Status;
    uint64_t    BufferSize;
    void*       Buffer;
} UEFI_FILE_IO_TOKEN;

typedef UEFI_STATUS (UEFI_API* UEFI_FILE_OPEN_EX) (
    IN UEFI_FILE_PROTOCOL*     This,
    OUT UEFI_FILE_PROTOCOL**   NewHandle,
    IN char16_t*               FileName,
    IN uint64_t                OpenMode,
    IN uint64_t                Attributes,
    IN OUT UEFI_FILE_IO_TOKEN* Token
);

/* UEFI Specification v2.10 13.5.8 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_READ_EX) (
    IN UEFI_FILE_PROTOCOL*     This,
    IN OUT UEFI_FILE_IO_TOKEN* Token
);

/* UEFI Specification v2.10 13.5.9 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_WRITE_EX) (
    IN UEFI_FILE_PROTOCOL*     This,
    IN OUT UEFI_FILE_IO_TOKEN* Token
);

/* UEFI Specification v2.10 13.5.10 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_FLUSH_EX) (
    IN UEFI_FILE_PROTOCOL*     This,
    IN OUT UEFI_FILE_IO_TOKEN* Token
);

/* UEFI Specification v2.10 13.5.11 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_SET_POSITION) (
    IN UEFI_FILE_PROTOCOL* This,
    IN uint64_t            Position
);

/* UEFI Specification v2.10 13.5.12 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_GET_POSITION) (
    IN UEFI_FILE_PROTOCOL* This,
    OUT uint64_t*          Position
);

/* UEFI Specification v2.10 13.5.13 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_GET_INFO) (
    IN UEFI_FILE_PROTOCOL* This,
    IN UEFI_GUID*          InformationType,
    IN OUT uint64_t*       BufferSize,
    OUT void*              Buffer
);

/* UEFI Specification v2.10 13.5.14 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_SET_INFO) (
    IN UEFI_FILE_PROTOCOL* This,
    IN UEFI_GUID*          InformationType,
    IN uint64_t            BufferSize,
    IN void*               Buffer
);

/* UEFI Specification v2.10 13.5.15 */
typedef UEFI_STATUS (UEFI_API* UEFI_FILE_FLUSH) (
    IN UEFI_FILE_PROTOCOL* This
);

/* UEFI Specification v2.10 13.5.16 */
#define UEFI_FILE_INFO_ID \
{0x09576e92, 0x6d3f, 0x11d2, 0x8e, 0x39, {0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}

typedef struct {
    uint64_t  Size;
    uint64_t  FileSize;
    uint64_t  PhysicalSize;
    UEFI_TIME CreateTime;
    UEFI_TIME LastAccessTime;
    UEFI_TIME ModificationTime;
    uint64_t  Attribute;
    char16_t  FileName [256]; // Note: 256 length is not part of spec. Standard maximum length for file names.
} UEFI_FILE_INFO;

/* UEFI Specification v2.10 13.5.17 */
#define EFI_FILE_SYSTEM_INFO_ID \
{0x09576e93, 0x6d3f, 0x11d2, 0x8e39, {0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}

typedef struct {
    uint64_t Size;
    uint8_t  ReadOnly;
    uint64_t VolumeSize;
    uint64_t FreeSpace;
    uint32_t BlockSize;
    char16_t VolumeLabel[256]; // Note: 256 length is not part of spec. Standard maximum length for file names.
} UEFI_FILE_SYSTEM_INFO;

/* UEFI Specification v2.10 13.5.1 */
#define UEFI_FILE_PROTOCOL_REVISION        0x00010000
#define UEFI_FILE_PROTOCOL_REVISION2       0x00020000
#define UEFI_FILE_PROTOCOL_LATEST_REVISION UEFI_FILE_PROTOCOL_REVISION2

typedef struct UEFI_FILE_PROTOCOL {
    uint64_t               Revision;
    UEFI_FILE_OPEN         Open;
    UEFI_FILE_CLOSE        Close;
    UEFI_FILE_DELETE       Delete;
    UEFI_FILE_READ         Read;
    UEFI_FILE_WRITE        Write;
    UEFI_FILE_GET_POSITION GetPosition;
    UEFI_FILE_SET_POSITION SetPosition;
    UEFI_FILE_GET_INFO     GetInfo;
    UEFI_FILE_SET_INFO     SetInfo;
    UEFI_FILE_FLUSH        Flush;
    UEFI_FILE_OPEN_EX      OpenEx;
    UEFI_FILE_READ_EX      ReadEx;
    UEFI_FILE_WRITE_EX     WriteEx;
    UEFI_FILE_FLUSH_EX     FlushEx;
} UEFI_FILE_PROTOCOL;

// Forward declaration because of circular dependencies.
typedef struct UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

/* UEFI Specification v2.10 13.4.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME) (
    IN UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL* This,
    OUT UEFI_FILE_PROTOCOL**             Root
);

/* UEFI Specification v2.10 13.4.1 */
#define UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID \
{0x0964e5b22, 0x6459, 0x11d2, 0x8e, 0x39, {0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}

#define UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REVISION 0x00010000

typedef struct UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL {
    uint64_t                                     Revision;
    UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
} UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL;

/******************************************************************************
UEFI DISK I/O PROTOCOLS (UEFI Specification v2.10 13.7 - 13.8)
******************************************************************************/

// Forward declaration because of circular dependencies.
typedef struct UEFI_DISK_IO_PROTOCOL UEFI_DISK_IO_PROTOCOL;

/* UEFI Specification v2.10 13.7.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_DISK_READ) (
    IN UEFI_DISK_IO_PROTOCOL* This,
    IN uint32_t               MediaId,
    IN uint64_t               Offset,
    IN uint64_t               BufferSize,
    OUT void*                 Buffer
);

/* UEFI Specification v2.10 13.7.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_DISK_WRITE) (
    IN UEFI_DISK_IO_PROTOCOL* This,
    IN uint32_t               MediaId,
    IN uint64_t               Offset,
    IN uint64_t               BufferSize,
    IN void*                  Buffer
);

/* UEFI Specification v2.10 13.7.1 */
#define UEFI_DISK_IO_PROTOCOL_GUID \
{0xCE345171, 0xBA0B, 0x11d2, 0x8e, 0x4F, {0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}

#define UEFI_DISK_IO_PROTOCOL_REVISION 0x00010000

typedef struct UEFI_DISK_IO_PROTOCOL {
    uint64_t        Revision;
    UEFI_DISK_READ  ReadDisk;
    UEFI_DISK_WRITE WriteDisk;
} UEFI_DISK_IO_PROTOCOL;

// Forward declaration because of circular dependencies.
typedef struct UEFI_DISK_IO2_PROTOCOL UEFI_DISK_IO2_PROTOCOL;

/* UEFI Specification v2.10 13.8.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_DISK_CANCEL_EX) (
    IN UEFI_DISK_IO2_PROTOCOL* This
);

/* UEFI Specification v2.10 13.8.3 */
typedef struct {
    UEFI_EVENT  Event;
    UEFI_STATUS TransactionStatus;
} UEFI_DISK_IO2_TOKEN;

typedef UEFI_STATUS (UEFI_API *UEFI_DISK_READ_EX) (
    IN UEFI_DISK_IO2_PROTOCOL*  This,
    IN uint32_t                 MediaId,
    IN uint64_t                 Offset,
    IN OUT UEFI_DISK_IO2_TOKEN* Token,
    IN uint64_t                 BufferSize,
    OUT void*                   Buffer
);

/* UEFI Specification v2.10 13.8.4 */
typedef UEFI_STATUS (UEFI_API *UEFI_DISK_WRITE_EX) (
    IN UEFI_DISK_IO2_PROTOCOL*  This,
    IN uint32_t                 MediaId,
    IN uint64_t                 Offset,
    IN OUT UEFI_DISK_IO2_TOKEN* Token,
    IN uint64_t                 BufferSize,
    IN void*                    Buffer
);

/* UEFI Specification v2.10 13.8.5 */
typedef UEFI_STATUS (UEFI_API *UEFI_DISK_FLUSH_EX) (
    IN UEFI_DISK_IO2_PROTOCOL*  This,
    IN OUT UEFI_DISK_IO2_TOKEN* Token
);

/* UEFI Specification v2.10 13.8.1 */
#define UEFI_DISK_IO2_PROTOCOL_GUID \
{0x151c8eae, 0x7f2c, 0x472c, 0x9e, 0x54, {0x98, 0x28, 0x19, 0x4f, 0x6a, 0x88}}

#define UEFI_DISK_IO2_PROTOCOL_REVISION 0x00020000

typedef struct UEFI_DISK_IO2_PROTOCOL {
    uint64_t            Revision;
    UEFI_DISK_CANCEL_EX Cancel;
    UEFI_DISK_READ_EX   ReadDiskEx;
    UEFI_DISK_WRITE_EX  WriteDiskEx;
    UEFI_DISK_FLUSH_EX  FlushDiskEx;
} UEFI_DISK_IO2_PROTOCOL;

/******************************************************************************
UEFI BLOCK I/O PROTOCOLS (UEFI Specification v2.10 13.9 - 13.10)
******************************************************************************/

// Forward declaration because of circular dependencies.
typedef struct UEFI_BLOCK_IO_PROTOCOL UEFI_BLOCK_IO_PROTOCOL;

/* UEFI Specification v2.10 13.9.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_RESET) (
    IN UEFI_BLOCK_IO_PROTOCOL* This,
    IN uint8_t                 ExtendedVerification
);

/* UEFI Specification v2.10 13.9.3 */
typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_READ) (
    IN UEFI_BLOCK_IO_PROTOCOL* This,
    IN uint32_t                MediaId,
    IN UEFI_LBA                LBA,
    IN uint64_t                BufferSize,
    OUT void*                  Buffer
);

/* UEFI Specification v2.10 13.9.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_WRITE) (
    IN UEFI_BLOCK_IO_PROTOCOL* This,
    IN uint32_t                MediaId,
    IN UEFI_LBA                LBA,
    IN uint64_t                BufferSize,
    IN void*                   Buffer
);

/* UEFI Specification v2.10 13.9.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_FLUSH) (
    IN UEFI_BLOCK_IO_PROTOCOL* This
);

/* UEFI Specification v2.10 13.9.1 */
#define UEFI_BLOCK_IO_PROTOCOL_GUID \
{0x964e5b21, 0x6459, 0x11d2, 0x8e, 0x39, {0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}}

#define UEFI_BLOCK_IO_PROTOCOL_REVISION2 0x00020001
#define UEFI_BLOCK_IO_PROTOCOL_REVISION3 ((2<<16) | (31))

typedef uint64_t UEFI_LBA;

typedef struct {
    uint64_t MediaId;
    uint8_t  RemovableMedia;
    uint8_t  MediaPresent;
    uint8_t  LogicalPartition;
    uint8_t  ReadOnly;
    uint8_t  WriteCaching;
    uint32_t BlockSize;
    uint32_t IoAlign;
    UEFI_LBA LastBlock;
    UEFI_LBA LowestAlignedLba;
    uint32_t LogicalBlocksPerPhysicalBlock;
    uint32_t OptimalTransferLengthGranularity;
} UEFI_BLOCK_IO_MEDIA;

typedef struct UEFI_BLOCK_IO_PROTOCOL {
    uint64_t            Revision;
    UEFI_BLOCK_IO_MEDIA *Media;
    UEFI_BLOCK_RESET    Reset;
    UEFI_BLOCK_READ     ReadBlocks;
    UEFI_BLOCK_WRITE    WriteBlocks;
    UEFI_BLOCK_FLUSH    FlushBlocks;
} UEFI_BLOCK_IO_PROTOCOL;

// Forward declaration because of circular dependencies.
typedef struct UEFI_BLOCK_IO2_PROTOCOL UEFI_BLOCK_IO2_PROTOCOL;

/* UEFI Specification v2.10 13.10.2 */
typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_RESET_EX) (
    IN UEFI_BLOCK_IO2_PROTOCOL* This,
    IN uint8_t                  ExtendedVerification
);

/* UEFI Specification v2.10 13.10.3 */
typedef struct {
    UEFI_EVENT  Event;
    UEFI_STATUS TransactionStatus;
} UEFI_BLOCK_IO2_TOKEN;

typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_READ_EX) (
    IN UEFI_BLOCK_IO2_PROTOCOL*  This,
    IN uint32_t                  MediaID,
    IN UEFI_LBA                  LBA,
    IN OUT UEFI_BLOCK_IO2_TOKEN* Token,
    IN uint64_t                  BufferSize,
    OUT void*                    Buffer
);

/* UEFI Specification v2.10 13.10.4 */
typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_WRITE_EX) (
    IN UEFI_BLOCK_IO2_PROTOCOL*  This,
    IN uint32_t                  MediaID,
    IN UEFI_LBA                  LBA,
    IN OUT UEFI_BLOCK_IO2_TOKEN* Token,
    IN uint64_t                  BufferSize,
    IN void*                     Buffer
);

/* UEFI Specification v2.10 13.10.5 */
typedef UEFI_STATUS (UEFI_API* UEFI_BLOCK_FLUSH_EX) (
    IN UEFI_BLOCK_IO2_PROTOCOL*  This,
    IN OUT UEFI_BLOCK_IO2_TOKEN* Token
);

/* UEFI Specification v2.10 13.10.1 */
#define EFI_BLOCK_IO2_PROTOCOL_GUID \
{0xa77b2472, 0xe282, 0x4e9f, 0xa2, 0x45, {0xc2, 0xc0, 0xe2, 0x7b, 0xbc, 0xc1}}

typedef struct UEFI_BLOCK_IO2_PROTOCOL {
    UEFI_BLOCK_IO_MEDIA* Media;
    UEFI_BLOCK_RESET_EX  Reset;
    UEFI_BLOCK_READ_EX   ReadBlocksEx;
    UEFI_BLOCK_WRITE_EX  WriteBlocksEx;
    UEFI_BLOCK_FLUSH_EX  FlushBlocksEx;
} UEFI_BLOCK_IO2_PROTOCOL;

/******************************************************************************
UEFI GUID PARTITION TABLE (GPT) DISK LAYOUT (UEFI Specification v2.10 5)
******************************************************************************/

typedef struct {
    uint8_t BootIndicator;
    uint8_t StartHead;
    uint8_t StartSector;
    uint8_t StartTrack;
    uint8_t OSIndicator;
    uint8_t EndHead;
    uint8_t EndSector;
    uint8_t EndTrack;
    uint8_t StartingLBA[4];
    uint8_t SizeInLBA[4];
} __attribute__((packed)) MBR_PARTITION_RECORD;

typedef struct {
    uint8_t              BootStrapCode[440];
    uint8_t              UniqueMbrSignature[4];
    uint8_t              Unknown[2];
    MBR_PARTITION_RECORD Partition[4];
    uint16_t             Signature;
} __attribute__((packed)) MASTER_BOOT_RECORD;

typedef struct {
    UEFI_GUID PartitionTypeGUID;
    UEFI_GUID UniquePartitionGUID;
    UEFI_LBA  StartingLBA;
    UEFI_LBA  EndingLBA;
    uint64_t  Attributes;
    char16_t  PartitionName[36];
}  __attribute__((packed)) EFI_PARTITION_ENTRY;

/******************************************************************************
UEFI PARTITION INFORMATION PROTOCOL (UEFI Specification v2.10 13.18)
******************************************************************************/
#define UEFI_PARTITION_INFO_PROTOCOL_GUID \
{0x8cf2f62c, 0xbc9b, 0x4821, 0x80, 0x8d, {0xec, 0x9e, 0xc4, 0x21, 0xa1, 0xa0}}

#define UEFI_PARTITION_INFO_PROTOCOL_REVISION 0x0001000
#define PARTITION_TYPE_OTHER                  0x00
#define PARTITION_TYPE_MBR                    0x01
#define PARTITION_TYPE_GPT                    0x02

typedef struct {
    uint32_t Revision;
    uint32_t Type;
    uint8_t  System;
    uint8_t  Reserved[7];
    union {
        MBR_PARTITION_RECORD Mbr;
        EFI_PARTITION_ENTRY  Gpt;
    } Info;
} __attribute__((packed)) UEFI_PARTITION_INFO_PROTOCOL;
