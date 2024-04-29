#include "uefi.h"

void uefi_print_number (UEFI_SYSTEM_TABLE* SystemTable, int32_t number, int32_t base);
void uefi_printf (UEFI_SYSTEM_TABLE* SystemTable, char16_t* unformatted_string, ...);
