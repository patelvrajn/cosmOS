#include "paging.h"
#include "../uefi/uefi_console.h"

/*******************************************************************************
Setup Kernel Page Tables Function
*******************************************************************************/
UEFI_STATUS UEFI_API Setup_Kernel_Page_Tables (UEFI_SYSTEM_TABLE* SystemTable, uint64_t& PML4Address, Memory_Map_Info* mmap_info) {

    // Get the size of valid physical memory from the memory map.
    const uint64_t SIZE_OF_PHYSICAL_MEMORY = Get_Maximum_Memory_Address (mmap_info);

    // Calculate number of page tables needed (minimum 1).
    uint64_t num_of_page_tables = 1;
    if ((SIZE_OF_PHYSICAL_MEMORY / PAGE_TABLES_VIRTUAL_ADDRESS_RANGE_SIZE) > 1) {
        num_of_page_tables = (SIZE_OF_PHYSICAL_MEMORY / PAGE_TABLES_VIRTUAL_ADDRESS_RANGE_SIZE);
        if ((SIZE_OF_PHYSICAL_MEMORY % PAGE_TABLES_VIRTUAL_ADDRESS_RANGE_SIZE) != 0) {
            num_of_page_tables = num_of_page_tables + 1;
        }
    }

    // Calculate number of page table entries.
    uint64_t num_of_page_table_entries = (SIZE_OF_PHYSICAL_MEMORY / PAGE_TABLES_ENTRY_VIRTUAL_ADDRESS_RANGE_SIZE);
    if ((SIZE_OF_PHYSICAL_MEMORY % PAGE_TABLES_ENTRY_VIRTUAL_ADDRESS_RANGE_SIZE) != 0) {
        num_of_page_table_entries += 1;
    }

    // Calculate number of page directories needed (minimum 1).
    uint64_t num_of_pd_tables = 1; ;
    if ((num_of_page_tables / PAGE_TABLES_NUM_OF_ENTRIES) > 1) {
        num_of_pd_tables = (num_of_page_tables / PAGE_TABLES_NUM_OF_ENTRIES);
        if ((num_of_page_tables % PAGE_TABLES_NUM_OF_ENTRIES) != 0) {
            num_of_pd_tables = num_of_pd_tables + 1;
        }
    }

    // Calculate number of page directory pointer tables needed (minimum 1).
    uint64_t num_of_pdpt_tables = 1;
    if ((num_of_pd_tables / PAGE_TABLES_NUM_OF_ENTRIES) > 1) {
        num_of_pdpt_tables = (num_of_pd_tables / PAGE_TABLES_NUM_OF_ENTRIES);
        if ((num_of_pd_tables % PAGE_TABLES_NUM_OF_ENTRIES) != 0) {
            num_of_pdpt_tables = num_of_pdpt_tables + 1;
        }
    }

    // Only 1 PML4 table exists in 4-level paging.
    const uint64_t NUM_OF_PML4_TABLES = 1;

    // Calculate memory/number of pages needed for all levels of paging tables.
    const uint64_t MEMORY_NEEDED_FOR_PAGE_TABLES  = num_of_page_tables * sizeof(page_table);
    const uint64_t MEMORY_NEEDED_FOR_PD_TABLES    = num_of_pd_tables   * sizeof(page_directory);
    const uint64_t MEMORY_NEEDED_FOR_PDPT_TABLES  = num_of_pdpt_tables * sizeof(page_directory_pointer_table);
    const uint64_t MEMORY_NEEDED_FOR_PML4_TABLES  = NUM_OF_PML4_TABLES * sizeof(page_map_level_4);
    const uint64_t TOTAL_MEMORY_NEEDED_FOR_TABLES = MEMORY_NEEDED_FOR_PML4_TABLES + MEMORY_NEEDED_FOR_PDPT_TABLES + MEMORY_NEEDED_FOR_PD_TABLES + MEMORY_NEEDED_FOR_PAGE_TABLES;

    uint64_t num_of_pages_needed_for_tables = TOTAL_MEMORY_NEEDED_FOR_TABLES / 4096;
    if ((TOTAL_MEMORY_NEEDED_FOR_TABLES % 4096) != 0) {
        num_of_pages_needed_for_tables += 1;
    }

    /* Allocate the number of pages needed for all levels of paging. Note the 
    usage of AllocatePages vs AllocatePool, we need to guarantee 4KB aligned 
    address to write into the CR3 register. */
    void* paging_memory = nullptr;
    UEFI_STATUS status = SystemTable->BootServices->AllocatePages (
        UEFI_ALLOCATE_TYPE::AllocateAnyPages,
        UEFI_MEMORY_TYPE::UefiLoaderData, 
        num_of_pages_needed_for_tables,
        (UEFI_PHYSICAL_ADDRESS*)&paging_memory
    );

    UEFI_PRINT_ERROR (SystemTable, status, u"Could not allocate memory for Kernel Page Tables");

    const uint64_t PDPT_PTR_STARTING_ADDR = (uint64_t)(((uint8_t*)paging_memory) + MEMORY_NEEDED_FOR_PML4_TABLES);
    const uint64_t PD_PTR_STARTING_ADDR   = (uint64_t)(((uint8_t*)paging_memory) + MEMORY_NEEDED_FOR_PML4_TABLES + MEMORY_NEEDED_FOR_PDPT_TABLES);
    const uint64_t PT_PTR_STARTING_ADDR   = (uint64_t)(((uint8_t*)paging_memory) + MEMORY_NEEDED_FOR_PML4_TABLES + MEMORY_NEEDED_FOR_PDPT_TABLES + MEMORY_NEEDED_FOR_PD_TABLES);

    PML4Address    = (uint64_t)paging_memory;
    void* pml4_ptr = paging_memory;
    void* pdpt_ptr = (void*) PDPT_PTR_STARTING_ADDR;
    void* pd_ptr   = (void*) PD_PTR_STARTING_ADDR;
    void* pt_ptr   = (void*) PT_PTR_STARTING_ADDR;

    // Page tables
    uint64_t pte_frame_address = 0;
    uint64_t pte_count         = 0;
    for (int i = 0; i < num_of_page_tables; i++) {
        
        page_table pt = {};

        for (int i = 0; i < PAGE_TABLES_NUM_OF_ENTRIES; i++) {
            
            page_table_entry pte = {};

            pte.present                  = 1;
            pte.read_write               = 1;
            pte.user_supervisor          = 0;
            pte.page_level_write_through = 0;
            pte.page_level_cache_disable = 0;
            pte.accessed                 = 0;
            pte.dirty                    = 0;
            pte.memory_type              = 0;
            pte.global                   = 0;
            pte.ignored_0                = 0;
            pte.frame_memory_address     = (pte_frame_address >> 12);
            pte.ignored_1                = 0;
            pte.protection_key           = 0;
            pte.execute_disable          = 0;

            pt.entries[i] = pte;
            pte_frame_address += PAGE_TABLES_ENTRY_VIRTUAL_ADDRESS_RANGE_SIZE;
            pte_count++;

            if (pte_count == num_of_page_table_entries) {
                break;
            }
        }

        *((page_table*)pt_ptr) = pt;
        pt_ptr = (void*)(((uint8_t*)pt_ptr) + sizeof(page_table));       

        if (pte_count == num_of_page_table_entries) {
            break;
        }
    }

    pt_ptr = (void*) PT_PTR_STARTING_ADDR;

    // Page directories
    uint64_t pde_count = 0;
    for (int i = 0; i < num_of_pd_tables; i++) {
        
        page_directory pd = {};

        for (int i = 0; i < PAGE_TABLES_NUM_OF_ENTRIES; i++) { 

            page_directory_entry pde = {};

            pde.present                  = 1;
            pde.read_write               = 1;
            pde.user_supervisor          = 0;
            pde.page_level_write_through = 0;
            pde.page_level_cache_disable = 0;
            pde.accessed                 = 0;
            pde.ignored_0                = 0;
            pde.page_size                = 0;
            pde.ignored_1                = 0;
            pde.PT_memory_address        = (((uint64_t) pt_ptr) >> 12);
            pde.ignored_2                = 0;
            pde.execute_disable          = 0;

            pd.entries[i] = pde;
            pt_ptr = (void*)(((uint8_t*)pt_ptr) + sizeof(page_table));
            pde_count++;
            
            if (pde_count == num_of_page_tables) {
                break;
            }
        }
    
        *((page_directory*)pd_ptr) = pd;
        pd_ptr = (void*)(((uint8_t*)pd_ptr) + sizeof(page_directory));

        if (pde_count == num_of_page_tables) {
            break;
        }
    }

    pd_ptr = (void*) PD_PTR_STARTING_ADDR;

    // Page directory pointer tables
    uint64_t pdpte_count = 0;
    for (int i = 0; i < num_of_pdpt_tables; i++) {
        
        page_directory_pointer_table pdpt = {};

        for (int i = 0; i < PAGE_TABLES_NUM_OF_ENTRIES; i++) { 

            page_directory_pointer_table_entry pdpte = {};

            pdpte.present                  = 1;
            pdpte.read_write               = 1;
            pdpte.user_supervisor          = 0;
            pdpte.page_level_write_through = 0;
            pdpte.page_level_cache_disable = 0;
            pdpte.accessed                 = 0;
            pdpte.ignored_0                = 0;
            pdpte.page_size                = 0;
            pdpte.ignored_1                = 0;
            pdpte.PD_memory_address        = (((uint64_t) pd_ptr) >> 12);
            pdpte.ignored_2                = 0;
            pdpte.execute_disable          = 0;

            pdpt.entries[i] = pdpte;
            pd_ptr = (void*)(((uint8_t*)pd_ptr) + sizeof(page_directory));  
            pdpte_count++;

            if (pdpte_count == num_of_pd_tables) {
                break;
            }
        }
    
        *((page_directory_pointer_table*)pdpt_ptr) = pdpt;
        pdpt_ptr = (void*)(((uint8_t*)pdpt_ptr) + sizeof(page_directory_pointer_table));

        if (pdpte_count == num_of_pd_tables) {
            break;
        }
    }

    pdpt_ptr = (void*) PDPT_PTR_STARTING_ADDR;

    // PML4
    page_map_level_4 pml4 = {};
    for (int i = 0; i < num_of_pdpt_tables; i++) { 

        page_map_level_4_entry pml4e = {};

        pml4e.present                  = 1;
        pml4e.read_write               = 1;
        pml4e.user_supervisor          = 0;
        pml4e.page_level_write_through = 0;
        pml4e.page_level_cache_disable = 0;
        pml4e.accessed                 = 0;
        pml4e.ignored_0                = 0;
        pml4e.reserved_0               = 0;
        pml4e.ignored_1                = 0;
        pml4e.PDPT_memory_address      = (((uint64_t) pdpt_ptr) >> 12);;
        pml4e.ignored_2                = 0;
        pml4e.execute_disable          = 0;

        pml4.entries[i] = pml4e;
        pdpt_ptr = (void*)(((uint8_t*)pdpt_ptr) + sizeof(page_directory_pointer_table));

    }

    *((page_map_level_4*)pml4_ptr) = pml4;

    return UEFI_SUCCESS;

}
