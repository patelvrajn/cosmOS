#include "uefi.h"

template <typename T> void uefi_print_number (UEFI_SYSTEM_TABLE* SystemTable, T number, uint64_t base);
void uefi_printf (UEFI_SYSTEM_TABLE* SystemTable, char16_t* unformatted_string, ...);
UEFI_INPUT_KEY uefi_wait_for_keystroke (UEFI_SYSTEM_TABLE* SystemTable);
