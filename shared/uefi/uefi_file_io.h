#pragma once
#include "uefi.h"

UEFI_FILE_PROTOCOL* get_esp_root (UEFI_HANDLE ImageHandle, UEFI_SYSTEM_TABLE* SystemTable);