#include <stdint.h>

/* UEFI Data Types UEFI Specification v2.10 2.3.1 */
/* 
typedef uint8_t  UEFI_BOOLEAN;
typedef int64_t  UEFI_INTN;
typedef uint64_t UEFI_UINTN;
typedef int8_t   UEFI_INT8;
typedef uint8_t  UEFI_UINT8;
typedef int16_t  UEFI_INT16;
typedef uint16_t UEFI_UINT16;
typedef int32_t  UEFI_INT32;
typedef uint32_t UEFI_UINT32;
typedef int64_t  UEFI_INT64;
typedef uint64_t UEFI_UINT64;
typedef char     UEFI_CHAR8;
typedef char16_t UEFI_CHAR16;
typedef void     UEFI_VOID;
typedEf const    UEFI_CONST;
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

// Not yet implemented from the specifications.
typedef void* UEFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef void* UEFI_RUNTIME_SERVICES;
typedef void* UEFI_BOOT_SERVICES;
typedef void* UEFI_CONFIGURATION_TABLE;

// Forward declaration because of circular dependency.
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

#define UEFI_BACKGROUND_BLACK 0x00
#define UEFI_BACKGROUND_BLUE  0x10
#define UEFI_BACKGROUND_GREEN 0x20
#define UEFI_BACKGROUND_CYAN  0x30

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

#define UEFI_SUCCESS 0
