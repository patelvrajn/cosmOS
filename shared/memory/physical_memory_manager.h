#pragma once
#include <stdint.h>
#include "../uefi/uefi_memory_map.h"
#include "../graphics/fonts/pc_screen_font_v1_renderer.h"

enum pmm_red_black_tree_color {
    black = 0,
    red   = 1
};

typedef struct {
    pmm_red_black_tree_color red_black_tree_color : 1;
    uint64_t                 is_allocated         : 1;
    uint64_t                 reserved             : 1;                       
    uint64_t                 aligned_size         : 61;
} physical_memory_size_and_flags;

typedef struct {
    physical_memory_size_and_flags size_and_flags;
    uint64_t                       address_of_parent;
    uint64_t                       address_of_left_child;
    uint64_t                       address_of_right_child;
} physical_memory_free_header;

typedef struct {
    physical_memory_size_and_flags size_and_flags;
} physical_memory_allocated_header;

typedef struct {
    physical_memory_size_and_flags size_and_flags;
} physical_memory_boundary_tag;

class Physical_Memory_Manager {

    public:

        Physical_Memory_Manager (Memory_Map_Info* mmap_info, void* pmm_null_memory, PC_Screen_Font_v1_Renderer* font_renderer);
        void* allocate_physical_frames (uint64_t desired_size);
        void  free_physical_frames (void* memory_to_free);
    
    private:

        // Pointer to the root of the red-black tree.
        void* pmm_red_black_tree_root;
        void* pmm_red_black_tree_null;

        Memory_Map_Info* m_mmap_info = nullptr;

        void  pmm_red_black_tree_rotate_left                  (void* x);
        void  pmm_red_black_tree_rotate_right                 (void* y);
        void* pmm_red_black_tree_find_parent_of_inserted_node (uint64_t value);
        void* pmm_red_black_tree_find_best_fit                (uint64_t value);
        void  pmm_red_black_tree_insert                       (void* z);
        void  pmm_red_black_tree_insert_fixup                 (void* z);
        void  pmm_red_black_tree_transplant                   (void* u, void* v);
        void* pmm_red_black_tree_minimum                      (void* x);
        void  pmm_red_black_tree_delete                       (void* z);
        void  pmm_red_black_tree_delete_fixup                 (void* x);

        bool Is_Physical_Memory_Region_Type_Usable (UEFI_MEMORY_TYPE mem_type);
        bool Is_Physical_Memory_Region_Usable (Memory_Map_Info* mmap_info, void* addr);
        uint64_t Get_First_Address_in_Next_Memory_Region (Memory_Map_Info* mmap_info, void* addr);
        uint64_t Get_Size_of_Memory_Region (Memory_Map_Info* mmap_info, void* addr);
        uint64_t Get_Next_Memory_Region (Memory_Map_Info* mmap_info, void* addr);

};
