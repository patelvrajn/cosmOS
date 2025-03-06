#pragma once
#include <stdint.h>
#include "../uefi/uefi.h"
#include "../uefi/uefi_memory_map.h"

#define PAGE_TABLES_SIZE                             4096
#define PAGE_TABLES_ENTRY_VIRTUAL_ADDRESS_RANGE_SIZE 4096
#define PAGE_TABLES_NUM_OF_ENTRIES                   512
#define PAGE_TABLES_VIRTUAL_ADDRESS_RANGE_SIZE       (PAGE_TABLES_ENTRY_VIRTUAL_ADDRESS_RANGE_SIZE * PAGE_TABLES_NUM_OF_ENTRIES)         

typedef struct {
    uint64_t present                  : 1;
    uint64_t read_write               : 1;
    uint64_t user_supervisor          : 1;
    uint64_t page_level_write_through : 1;
    uint64_t page_level_cache_disable : 1;
    uint64_t accessed                 : 1;
    uint64_t ignored_0                : 1;
    uint64_t reserved_0               : 1;
    uint64_t ignored_1                : 4;
    uint64_t PDPT_memory_address      : 40;
    uint64_t ignored_2                : 11;
    uint64_t execute_disable          : 1;
} page_map_level_4_entry;

typedef struct {
    page_map_level_4_entry entries[PAGE_TABLES_NUM_OF_ENTRIES];
} page_map_level_4;

typedef struct {
    uint64_t present                  : 1;
    uint64_t read_write               : 1;
    uint64_t user_supervisor          : 1;
    uint64_t page_level_write_through : 1;
    uint64_t page_level_cache_disable : 1;
    uint64_t accessed                 : 1;
    uint64_t ignored_0                : 1;
    uint64_t page_size                : 1;
    uint64_t ignored_1                : 4;
    uint64_t PD_memory_address        : 40;
    uint64_t ignored_2                : 11;
    uint64_t execute_disable          : 1;
} page_directory_pointer_table_entry;

typedef struct {
    page_directory_pointer_table_entry entries[PAGE_TABLES_NUM_OF_ENTRIES];
} page_directory_pointer_table;

typedef struct {
    uint64_t present                  : 1;
    uint64_t read_write               : 1;
    uint64_t user_supervisor          : 1;
    uint64_t page_level_write_through : 1;
    uint64_t page_level_cache_disable : 1;
    uint64_t accessed                 : 1;
    uint64_t ignored_0                : 1;
    uint64_t page_size                : 1;
    uint64_t ignored_1                : 4;
    uint64_t PT_memory_address        : 40;
    uint64_t ignored_2                : 11;
    uint64_t execute_disable          : 1;
} page_directory_entry;

typedef struct {
    page_directory_entry entries[PAGE_TABLES_NUM_OF_ENTRIES];
} page_directory;

typedef struct {
    uint64_t present                  : 1;
    uint64_t read_write               : 1;
    uint64_t user_supervisor          : 1;
    uint64_t page_level_write_through : 1;
    uint64_t page_level_cache_disable : 1;
    uint64_t accessed                 : 1;
    uint64_t dirty                    : 1;
    uint64_t memory_type              : 1;
    uint64_t global                   : 1;
    uint64_t ignored_0                : 3;
    uint64_t frame_memory_address     : 40;
    uint64_t ignored_1                : 7;
    uint64_t protection_key           : 4;
    uint64_t execute_disable          : 1;
} page_table_entry;

typedef struct {
    page_table_entry entries[PAGE_TABLES_NUM_OF_ENTRIES];
} page_table;

UEFI_STATUS UEFI_API Setup_Kernel_Page_Tables (UEFI_SYSTEM_TABLE* SystemTable, uint64_t& PML4Address, Memory_Map_Info* mmap_info);
