#include "physical_memory_manager.h"

// Input p is a void*.
#define PMM_PHYSICAL_ADDRESS_BYTE_ALIGNMENT_BITS  3
#define PMM_PHYSICAL_ADDRESS_FRAME_ALIGNMENT_BITS 12
#define PMM_BYTE_SIZE                             8    // (2^PMM_PHYSICAL_ADDRESS_BYTE_ALIGNMENT_BITS)
#define PMM_FRAME_SIZE                            4096 // (2^PMM_PHYSICAL_ADDRESS_FRAME_ALIGNMENT_BITS)
#define PMM_IS_ALLOCATED_MEMORY_FLAG(p)           ((physical_memory_size_and_flags*)(p))->is_allocated
#define PMM_RED_BLACK_TREE_MEM_FREE_HEADER(p)     ((physical_memory_free_header*)(p))
#define PMM_RED_BLACK_TREE_KEY_VALUE(p)           (((uint64_t)(*((physical_memory_size_and_flags*)(p))).aligned_size) << PMM_PHYSICAL_ADDRESS_BYTE_ALIGNMENT_BITS)
#define PMM_RED_BLACK_TREE_COLOR(p)               PMM_RED_BLACK_TREE_MEM_FREE_HEADER(p)->size_and_flags.red_black_tree_color
#define PMM_RED_BLACK_TREE_PARENT(p)              (*((void**)(&(PMM_RED_BLACK_TREE_MEM_FREE_HEADER(p)->address_of_parent))))
#define PMM_RED_BLACK_TREE_LEFT_CHILD(p)          (*((void**)(&(PMM_RED_BLACK_TREE_MEM_FREE_HEADER(p)->address_of_left_child))))
#define PMM_RED_BLACK_TREE_RIGHT_CHILD(p)         (*((void**)(&(PMM_RED_BLACK_TREE_MEM_FREE_HEADER(p)->address_of_right_child))))
#define PMM_RED_BLACK_TREE_MEM_START(p)           (*((void**)(((uint8_t*)p) + sizeof(physical_memory_free_header))))
#define PMM_RED_BLACK_TREE_GRANDPARENT(p)         PMM_RED_BLACK_TREE_PARENT(PMM_RED_BLACK_TREE_PARENT(p))
#define PMM_RED_BLACK_TREE_LEFT_UNCLE(p)          PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_GRANDPARENT(p))
#define PMM_RED_BLACK_TREE_RIGHT_UNCLE(p)         PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_GRANDPARENT(p))

/*******************************************************************************
Red-black Tree Rotate Left Function

Rotates a node x and it's right subtree y left. Node y becomes the parent of x 
and x becomes y's left subtree.
*******************************************************************************/
void Physical_Memory_Manager::pmm_red_black_tree_rotate_left (void* x) {

    // y is the right subtree of x.
    void* y = PMM_RED_BLACK_TREE_RIGHT_CHILD(x);

    // y's left subtree becomes x's right subtree.
    PMM_RED_BLACK_TREE_RIGHT_CHILD(x) = PMM_RED_BLACK_TREE_LEFT_CHILD(y);

    // If y's left subtree is not empty then x becomes it's parent.
    if (PMM_RED_BLACK_TREE_LEFT_CHILD(y) != pmm_red_black_tree_null) {
        PMM_RED_BLACK_TREE_PARENT(PMM_RED_BLACK_TREE_LEFT_CHILD(y)) = x;
    }

    // y's parent is x's parent.
    PMM_RED_BLACK_TREE_PARENT(y) = PMM_RED_BLACK_TREE_PARENT(x);

    // If x was the root of the tree, y is now the root.
    if (PMM_RED_BLACK_TREE_PARENT(x) == pmm_red_black_tree_null) {
        pmm_red_black_tree_root = y;
    
    // If x was it's parent's left child then y is now the left child.
    } else if (x == PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(x))) {
        PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(x)) = y;

    // If x was it's parent's right child then y is now the right child.
    } else {
        PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_PARENT(x)) = y;
    }

    // x becomes y's left child.
    PMM_RED_BLACK_TREE_LEFT_CHILD(y) = x;

    // y becomes x's parent.
    PMM_RED_BLACK_TREE_PARENT(x) = y;

}

/*******************************************************************************
Red-black Tree Rotate Right Function

Rotates a node y and it's left subtree x right. Node x becomes the parent of y 
and y becomes x's right subtree.
*******************************************************************************/
void Physical_Memory_Manager::pmm_red_black_tree_rotate_right (void* y) { 

    // x is the left subtree of y.
    void* x = PMM_RED_BLACK_TREE_LEFT_CHILD(y);

    // x's right subtree becomes y's left subtree.
    PMM_RED_BLACK_TREE_LEFT_CHILD(y) = PMM_RED_BLACK_TREE_RIGHT_CHILD(x);

    // If x's right subtree is not empty then y becomes it's parent.
    if (PMM_RED_BLACK_TREE_RIGHT_CHILD(x) != pmm_red_black_tree_null) {
        PMM_RED_BLACK_TREE_PARENT(PMM_RED_BLACK_TREE_RIGHT_CHILD(x)) = y;
    }

    // x's parent is y's parent.
    PMM_RED_BLACK_TREE_PARENT(x) = PMM_RED_BLACK_TREE_PARENT(y);

    // If y was the root of the tree, x is now the root.
    if (PMM_RED_BLACK_TREE_PARENT(y) == pmm_red_black_tree_null) {
        pmm_red_black_tree_root = x;
    
    // If y was it's parent's left child then x is now the left child.
    } else if (y == PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(y))) {
        PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(y)) = x;

    // If y was it's parent's right child then x is now the right child.
    } else {
        PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_PARENT(y)) = x;
    }

    // y becomes x's right child.
    PMM_RED_BLACK_TREE_RIGHT_CHILD(x) = y;

    // x becomes y's parent.
    PMM_RED_BLACK_TREE_PARENT(y) = x;

}

/*******************************************************************************
Red-black Tree Find Parent of Inserted Node Function
*******************************************************************************/
void* Physical_Memory_Manager::pmm_red_black_tree_find_parent_of_inserted_node (uint64_t value) {

    // Initialize x to the root to descend down the tree.
    void* x = pmm_red_black_tree_root;

    // Initialize y which will be the parent of x.
    void* y = pmm_red_black_tree_null;

    /* Start at the root and descend down the tree to find the parent of x like
    an ordinary BST. */
    while (x != pmm_red_black_tree_null) {
        
        // Set y to be the current node x.
        y = x;

        /* If the given value is less than the current node x's key go left 
        otherwise, go right.*/
        if (value < PMM_RED_BLACK_TREE_KEY_VALUE(x)) {
            x = PMM_RED_BLACK_TREE_LEFT_CHILD(x);
        } else {
            x = PMM_RED_BLACK_TREE_RIGHT_CHILD(x);
        }
    }

    // Return the parent of the inserted node.
    return y;

}

/*******************************************************************************
Red-black Tree Find Best Fit Function
*******************************************************************************/
void* Physical_Memory_Manager::pmm_red_black_tree_find_best_fit (uint64_t value) {

    // Initialize x to the root to descend down the tree.
    void* x = pmm_red_black_tree_root;

    /* Initialize best fit which will be the node with the smallest value 
    greater than the given one. */
    void* best_fit = pmm_red_black_tree_null;

    /* Start at the root and descend down the tree to find the parent of x like
    an ordinary BST. */
    while (x != pmm_red_black_tree_null) {
        
        /* Set best fit to be the current node x if x has a value greater than
        the given value. */
        if (value <= PMM_RED_BLACK_TREE_KEY_VALUE(x)) {
            best_fit = x;
        }

        /* If the given value is less than the current node x's key go left 
        otherwise, go right.*/
        if (value < PMM_RED_BLACK_TREE_KEY_VALUE(x)) {
            x = PMM_RED_BLACK_TREE_LEFT_CHILD(x);
        } else {
            x = PMM_RED_BLACK_TREE_RIGHT_CHILD(x);
        }
    }

    // Return the best fit.
    return best_fit;

}

/*******************************************************************************
Red-black Tree Insert Function
*******************************************************************************/
void Physical_Memory_Manager::pmm_red_black_tree_insert (void* z) {

    // y will be the parent of z.
    void* y = pmm_red_black_tree_find_parent_of_inserted_node (PMM_RED_BLACK_TREE_KEY_VALUE(z));

    // Set z's parent to be y after the descent.
    PMM_RED_BLACK_TREE_PARENT(z) = y;

    /* If y is the null node, z is the root of the tree otherwise, compare z's 
    key to it's parent's key and place z as the left or right child.*/
    if (y == pmm_red_black_tree_null) {
        pmm_red_black_tree_root = z;
    } else if (PMM_RED_BLACK_TREE_KEY_VALUE(z) < PMM_RED_BLACK_TREE_KEY_VALUE(y)) {
        PMM_RED_BLACK_TREE_LEFT_CHILD(y) = z;
    } else {
        PMM_RED_BLACK_TREE_RIGHT_CHILD(y) = z;
    }

    /* Color z red and give it null node black children - coloring inserted 
    nodes black would violate the black height property of the tree which 
    produces a global problem while coloring inserted nodes red presents a more 
    localized problem which is fixed via rotations and recoloring. */
    PMM_RED_BLACK_TREE_LEFT_CHILD(z)  = pmm_red_black_tree_null;
    PMM_RED_BLACK_TREE_RIGHT_CHILD(z) = pmm_red_black_tree_null;
    PMM_RED_BLACK_TREE_COLOR(z)       = pmm_red_black_tree_color::red;

    // Handle any red black tree property violations after insertion.
    pmm_red_black_tree_insert_fixup(z);

}

/*******************************************************************************
Red-black Tree Insert Fix-Up Function
*******************************************************************************/
void Physical_Memory_Manager::pmm_red_black_tree_insert_fixup (void* z) {

    /* Only execute loop if z's parent is also red which violates that a red 
    node should have black children. (z is a red node being inserted). */
    while (PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(z)) == pmm_red_black_tree_color::red) {

        /* Is the parent of z the left child of z's grandparent? */
        if (PMM_RED_BLACK_TREE_PARENT(z) == PMM_RED_BLACK_TREE_LEFT_UNCLE(z)) {

            // Establish y as z's uncle.
            void* y = PMM_RED_BLACK_TREE_RIGHT_UNCLE(z);

            // z's uncle is red
            if (PMM_RED_BLACK_TREE_COLOR(y) == pmm_red_black_tree_color::red) {
                
                /* z's parent is red thus z's grandparent is black as there 
                wasn't any violations prior to the insert and since z's uncle is 
                also red, transfer the blackness down the tree and make the 
                grandparent red. */
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_GRANDPARENT(z)) = pmm_red_black_tree_color::red;
                PMM_RED_BLACK_TREE_COLOR(y)                                 = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(z))      = pmm_red_black_tree_color::black;

                /* The red grandparent is now z as we may have propagated the 
                double red problem up the tree. */
                z = PMM_RED_BLACK_TREE_GRANDPARENT(z);

            // z's uncle is black
            } else {

                /* Is z the right child of it's parent? Rotate it so it becomes
                the left child. */
                if (z == PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_PARENT(z))) {

                    /* Perform a left rotation on z's parent. z's parent is now
                    z. */
                    z = PMM_RED_BLACK_TREE_PARENT(z);
                    pmm_red_black_tree_rotate_left(z);

                }

                /* z is the left child of it's parent. Recolor and rotate to fix
                the violation. */
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(z))      = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_GRANDPARENT(z)) = pmm_red_black_tree_color::red;
                pmm_red_black_tree_rotate_right(PMM_RED_BLACK_TREE_GRANDPARENT(z));

            }

        // The parent of z is the right child of z's grandparent (symmetrical to
        // the first case where it is the left child).
        } else { 

            // Establish y as z's uncle.
            void* y = PMM_RED_BLACK_TREE_LEFT_UNCLE(z);

            // z's uncle is red
            if (PMM_RED_BLACK_TREE_COLOR(y) == pmm_red_black_tree_color::red) {

                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_GRANDPARENT(z)) = pmm_red_black_tree_color::red;
                PMM_RED_BLACK_TREE_COLOR(y)                                 = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(z))      = pmm_red_black_tree_color::black;

                z = PMM_RED_BLACK_TREE_GRANDPARENT(z);

            // z's uncle is black
            } else {

                if (z == PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(z))) {

                    z = PMM_RED_BLACK_TREE_PARENT(z);
                    pmm_red_black_tree_rotate_right(z);

                }

                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(z))      = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_GRANDPARENT(z)) = pmm_red_black_tree_color::red;
                pmm_red_black_tree_rotate_left(PMM_RED_BLACK_TREE_GRANDPARENT(z));

            }
        }
    }

    /* Always color the root of the red-black tree black - in the case z becomes
    the root. */
    PMM_RED_BLACK_TREE_COLOR(pmm_red_black_tree_root) = pmm_red_black_tree_color::black;

}

/*******************************************************************************
Red-black Tree Transplant Function
*******************************************************************************/
void Physical_Memory_Manager::pmm_red_black_tree_transplant (void* u, void* v) {

    /* Replace v as the left or right subtree of u's parent instead of u if u 
    has a parent. If u was the root, v is now the root. */
    if (PMM_RED_BLACK_TREE_PARENT(u) == pmm_red_black_tree_null) {
        pmm_red_black_tree_root = v;
    } else if (u == PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(u))) {
        PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(u)) = v;
    } else {
        PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_PARENT(u)) = v;
    }

    // v's parent is u's parent.
    PMM_RED_BLACK_TREE_PARENT(v) = PMM_RED_BLACK_TREE_PARENT(u);

}

/*******************************************************************************
Red-black Tree Minimum Function

Finds the minimum of a subtree rooted at node x.
*******************************************************************************/
void* Physical_Memory_Manager::pmm_red_black_tree_minimum (void* x) {
    while (PMM_RED_BLACK_TREE_LEFT_CHILD(x) != pmm_red_black_tree_null) {
        x = PMM_RED_BLACK_TREE_LEFT_CHILD(x);
    }

    return x;
}

/*******************************************************************************
Red-black Tree Delete Function
*******************************************************************************/
void Physical_Memory_Manager::pmm_red_black_tree_delete (void* z) {

    /* y is the node either being moved within the tree or removed from the tree
    either z's replacement or successor thus, we must track it's color.*/
    void* y = z;
    pmm_red_black_tree_color y_original_color = PMM_RED_BLACK_TREE_COLOR(y);

    /* x is the node that moves into y's original position i.e. where y was when
    delete was called. */
    void* x;

    if (PMM_RED_BLACK_TREE_LEFT_CHILD(z) == pmm_red_black_tree_null) {

        // z only has a right child or no children thus z is replaced by it's 
        // right child or null respectively.
        x = PMM_RED_BLACK_TREE_RIGHT_CHILD(z);
        pmm_red_black_tree_transplant(z, x);

    } else if (PMM_RED_BLACK_TREE_RIGHT_CHILD(z) == pmm_red_black_tree_null) {

        // z only has a left child thus z is replaced by it's left child.
        x = PMM_RED_BLACK_TREE_LEFT_CHILD(z);
        pmm_red_black_tree_transplant(z, x);

    } else { // z has a left and right child.

        // z's inorder successor is z's replacement if z has two children.
        y = pmm_red_black_tree_minimum (PMM_RED_BLACK_TREE_RIGHT_CHILD(z));

        /* Track z's inorder successor's color because a fix-up will need to
        incur on node x if it was black. */
        y_original_color = PMM_RED_BLACK_TREE_COLOR(y);

        /* y only has a right child so it is y's replacement. y cannot have a 
        left child otherwise, it would have been z's inorder successor. */
        x = PMM_RED_BLACK_TREE_RIGHT_CHILD(y);

        // Is z's successor deeper than just it's right child?
        if (y != PMM_RED_BLACK_TREE_RIGHT_CHILD(z)) {
            
            // The right child of y replaces y's position.
            pmm_red_black_tree_transplant (y, x);
            
            // z's successor's right child is z's right child.
            PMM_RED_BLACK_TREE_RIGHT_CHILD(y) = PMM_RED_BLACK_TREE_RIGHT_CHILD(z);
            PMM_RED_BLACK_TREE_PARENT(PMM_RED_BLACK_TREE_RIGHT_CHILD(y)) = y;

        } else {
            
            /* The call to delete fix-up relies on x's parent being y in the 
            case x is the null node. */
            PMM_RED_BLACK_TREE_PARENT(x) = y;
        
        }

        // Replace z with it's in order successor.
        pmm_red_black_tree_transplant (z, y);

        // z's successor's left child is z's left child.
        PMM_RED_BLACK_TREE_LEFT_CHILD(y) = PMM_RED_BLACK_TREE_LEFT_CHILD(z);
        PMM_RED_BLACK_TREE_PARENT(PMM_RED_BLACK_TREE_LEFT_CHILD(y)) = y;

        // Color y z's color.
        PMM_RED_BLACK_TREE_COLOR(y) = PMM_RED_BLACK_TREE_COLOR(z);

    }

    /* If y was black, perform a fixup on node x. Noting that if y was red a 
    fixup is not needed as a path on the tree's black height has not changed
    and y's children must be black, no red nodes are adajacent. */
    if (y_original_color == pmm_red_black_tree_color::black) {
        pmm_red_black_tree_delete_fixup(x);
    }
}

/*******************************************************************************
Red-black Tree Delete Fix-Up Function
*******************************************************************************/
void Physical_Memory_Manager::pmm_red_black_tree_delete_fixup (void* x) {

    /* Iterate until doubly black node x propagates up to the root or x is no
    no longer doubly black. */
    while ((x != pmm_red_black_tree_root) && (PMM_RED_BLACK_TREE_COLOR(x) == pmm_red_black_tree_color::black)) {

        // Is x a left child?
        if (x == PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(x))) {

            // w is x's sibling.
            void* w = PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_PARENT(x));

            // w is red. 
            if (PMM_RED_BLACK_TREE_COLOR(w) == pmm_red_black_tree_color::red) {

                // Transform this case into a case where w is a black sibling.
                PMM_RED_BLACK_TREE_COLOR(w)                            = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(x)) = pmm_red_black_tree_color::red;
                pmm_red_black_tree_rotate_left(PMM_RED_BLACK_TREE_PARENT(x));
                w = PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_PARENT(x));

            }

            // Are both w's children black?
            if ((PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_LEFT_CHILD(w))  == pmm_red_black_tree_color::black) &&
                (PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_RIGHT_CHILD(w)) == pmm_red_black_tree_color::black)) {

                /* Color w red and propagate the blackness of the doubly black x 
                up the tree. */
                PMM_RED_BLACK_TREE_COLOR(w) = pmm_red_black_tree_color::red;
                x = PMM_RED_BLACK_TREE_PARENT(x);

            } else { // At least one of w's children is red. 

                // Is w's right child black? 
                if (PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_RIGHT_CHILD(w)) == pmm_red_black_tree_color::black) {

                    // If so, create the case where w's right child is red.
                    PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_LEFT_CHILD(w)) = pmm_red_black_tree_color::black;
                    PMM_RED_BLACK_TREE_COLOR(w)                                = pmm_red_black_tree_color::red;
                    pmm_red_black_tree_rotate_right(w);
                    w = PMM_RED_BLACK_TREE_RIGHT_CHILD(PMM_RED_BLACK_TREE_PARENT(x));

                }

                /* w's right child is now red, this case eliminates x as a 
                doubly black node. Setting x as the root terminates the loop as
                x is no longer doubly black. */
                PMM_RED_BLACK_TREE_COLOR(w)                                 = PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(x));
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(x))      = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_RIGHT_CHILD(w)) = pmm_red_black_tree_color::black;
                pmm_red_black_tree_rotate_left(PMM_RED_BLACK_TREE_PARENT(x));
                x = pmm_red_black_tree_root;

            }

        // x is a right child. Symmetrical cases.
        } else {

            void* w = PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(x));

            if (PMM_RED_BLACK_TREE_COLOR(w) == pmm_red_black_tree_color::red) {

                // Transform this case into a case where w is a black sibling.
                PMM_RED_BLACK_TREE_COLOR(w)                            = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(x)) = pmm_red_black_tree_color::red;
                pmm_red_black_tree_rotate_right(PMM_RED_BLACK_TREE_PARENT(x));
                w = PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(x));

            }

            if ((PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_LEFT_CHILD(w))  == pmm_red_black_tree_color::black) &&
                (PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_RIGHT_CHILD(w)) == pmm_red_black_tree_color::black)) {

                PMM_RED_BLACK_TREE_COLOR(w) = pmm_red_black_tree_color::red;
                x = PMM_RED_BLACK_TREE_PARENT(x);

            } else { 

                if (PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_LEFT_CHILD(w)) == pmm_red_black_tree_color::black) {

                    PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_RIGHT_CHILD(w)) = pmm_red_black_tree_color::black;
                    PMM_RED_BLACK_TREE_COLOR(w)                                 = pmm_red_black_tree_color::red;
                    pmm_red_black_tree_rotate_left(w);
                    w = PMM_RED_BLACK_TREE_LEFT_CHILD(PMM_RED_BLACK_TREE_PARENT(x));

                }

                PMM_RED_BLACK_TREE_COLOR(w)                                = PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(x));
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_PARENT(x))     = pmm_red_black_tree_color::black;
                PMM_RED_BLACK_TREE_COLOR(PMM_RED_BLACK_TREE_LEFT_CHILD(w)) = pmm_red_black_tree_color::black;
                pmm_red_black_tree_rotate_right(PMM_RED_BLACK_TREE_PARENT(x));
                x = pmm_red_black_tree_root;

            }
        }    
    }

    // Always color x black.
    PMM_RED_BLACK_TREE_COLOR(x) = pmm_red_black_tree_color::black;

}

/*******************************************************************************
Is the Memory Region Type described in the UEFI Memory Map Usable by the OS?
*******************************************************************************/
bool Physical_Memory_Manager::Is_Physical_Memory_Region_Type_Usable (UEFI_MEMORY_TYPE mem_type) {

    /* Do not use UefiLoaderCode or UefiLoaderData as the operating system's 
    code or data maybe allocated to that type of memory. */
    return ((mem_type == UEFI_MEMORY_TYPE::UefiBootServicesCode)   ||
            (mem_type == UEFI_MEMORY_TYPE::UefiBootServicesData)   ||
            (mem_type == UEFI_MEMORY_TYPE::UefiConventionalMemory) ||
            (mem_type == UEFI_MEMORY_TYPE::UefiPersistentMemory)); 

}

/*******************************************************************************
Is the Memory Region described in the UEFI Memory Map Usable by the OS?
*******************************************************************************/
bool Physical_Memory_Manager::Is_Physical_Memory_Region_Usable (Memory_Map_Info* mmap_info, void* addr) {

    // Calculate the number of memory map entries.
    uint64_t num_of_mem_map_entries = mmap_info->size / mmap_info->desc_size;

    // Loop thru the memory map.
    for (uint64_t idx = 0; idx < num_of_mem_map_entries; idx++) {

        /* Pointer arithmetic to point to an entry in the memory map using the
        given size of the memory descriptors. */
        UEFI_MEMORY_DESCRIPTOR* mem_desc = (UEFI_MEMORY_DESCRIPTOR*)(((uint8_t*)(mmap_info->map)) + (idx * mmap_info->desc_size));

        if ((((uint64_t)addr) >= mem_desc->PhysicalStart) && (((uint64_t)addr) < (mem_desc->PhysicalStart + (mem_desc->NumberOfPages * PMM_FRAME_SIZE)))) {

            if (mem_desc->PhysicalStart == 0) {
                return false;
            }

            UEFI_MEMORY_TYPE mem_type = ((UEFI_MEMORY_TYPE)mem_desc->Type);

            return Is_Physical_Memory_Region_Type_Usable(mem_type);

        }
    }

    return false;
}

/*******************************************************************************
Get Expacted First Address of the next Memory Region

Gets the first address of the next region by adding the size in number of pages
to the starting physical address of the current region.
*******************************************************************************/
uint64_t Physical_Memory_Manager::Get_Expected_First_Address_in_Next_Memory_Region (Memory_Map_Info* mmap_info, void* addr) {

    // Calculate the number of memory map entries.
    uint64_t num_of_mem_map_entries = mmap_info->size / mmap_info->desc_size;

    // Loop thru the memory map.
    for (uint64_t idx = 0; idx < num_of_mem_map_entries; idx++) {

        /* Pointer arithmetic to point to an entry in the memory map using the
        given size of the memory descriptors. */
        UEFI_MEMORY_DESCRIPTOR* mem_desc = (UEFI_MEMORY_DESCRIPTOR*)(((uint8_t*)(mmap_info->map)) + (idx * mmap_info->desc_size));

        if ((((uint64_t)addr) >= mem_desc->PhysicalStart) && (((uint64_t)addr) < (mem_desc->PhysicalStart + (mem_desc->NumberOfPages * PMM_FRAME_SIZE)))) {

            return (mem_desc->PhysicalStart + (mem_desc->NumberOfPages * PMM_FRAME_SIZE));

        }
    }

    return 0;
}

/*******************************************************************************
Get Next Memory Region

Gets the first address of the next region by looking for the smallest starting
physical address larger than the supplied address.
*******************************************************************************/
uint64_t Physical_Memory_Manager::Get_Next_Memory_Region (Memory_Map_Info* mmap_info, void* addr) {

    // Calculate the number of memory map entries.
    uint64_t num_of_mem_map_entries = mmap_info->size / mmap_info->desc_size;

    bool set = false;
    uint64_t next_region_address = 0;

    // Loop thru the memory map.
    for (uint64_t idx = 0; idx < num_of_mem_map_entries; idx++) {

        /* Pointer arithmetic to point to an entry in the memory map using the
        given size of the memory descriptors. */
        UEFI_MEMORY_DESCRIPTOR* mem_desc = (UEFI_MEMORY_DESCRIPTOR*)(((uint8_t*)(mmap_info->map)) + (idx * mmap_info->desc_size));

        if ((!set) && (mem_desc->PhysicalStart > ((uint64_t)addr))) {
            next_region_address = mem_desc->PhysicalStart;
            set = true;
        }

        /* Find the smallest memory region starting address greater than the
        current address. */
        if ((set) && (mem_desc->PhysicalStart > ((uint64_t)addr)) && (mem_desc->PhysicalStart < next_region_address)) {
            next_region_address = mem_desc->PhysicalStart;
        }
    }

    return next_region_address;

}

/*******************************************************************************
Get Size of Memory Region
*******************************************************************************/
uint64_t Physical_Memory_Manager::Get_Size_of_Memory_Region (Memory_Map_Info* mmap_info, void* addr) {

    // Calculate the number of memory map entries.
    uint64_t num_of_mem_map_entries = mmap_info->size / mmap_info->desc_size;

    // Loop thru the memory map.
    for (uint64_t idx = 0; idx < num_of_mem_map_entries; idx++) {

        /* Pointer arithmetic to point to an entry in the memory map using the
        given size of the memory descriptors. */
        UEFI_MEMORY_DESCRIPTOR* mem_desc = (UEFI_MEMORY_DESCRIPTOR*)(((uint8_t*)(mmap_info->map)) + (idx * mmap_info->desc_size));

        if ((((uint64_t)addr) >= mem_desc->PhysicalStart) && (((uint64_t)addr) < (mem_desc->PhysicalStart + (mem_desc->NumberOfPages * PMM_FRAME_SIZE)))) {

            return (mem_desc->NumberOfPages * PMM_FRAME_SIZE);

        }
    }

    return 0;
}

/******************************************************************************* 
Initialize Physical Memory Manager Function (Constructor)
Read memory map passed from UEFI bootloader and insert free regions into red-
black tree.
*******************************************************************************/
Physical_Memory_Manager::Physical_Memory_Manager (Memory_Map_Info* mmap_info, void* pmm_null_memory) {

    m_mmap_info = mmap_info;

    /* Initialize pmm_red_black_tree_null as having the color black and it's
    parent and children are pmm_red_black_tree_null as well. */
    pmm_red_black_tree_null = pmm_null_memory;
    PMM_RED_BLACK_TREE_COLOR(pmm_red_black_tree_null)       = pmm_red_black_tree_color::black;
    PMM_RED_BLACK_TREE_PARENT(pmm_red_black_tree_null)      = pmm_red_black_tree_null;
    PMM_RED_BLACK_TREE_LEFT_CHILD(pmm_red_black_tree_null)  = pmm_red_black_tree_null;
    PMM_RED_BLACK_TREE_RIGHT_CHILD(pmm_red_black_tree_null) = pmm_red_black_tree_null;

    // Initialize the root of the tree as being the null construct.
    pmm_red_black_tree_root = pmm_red_black_tree_null;
    PMM_RED_BLACK_TREE_COLOR(pmm_red_black_tree_root)       = pmm_red_black_tree_color::black;
    PMM_RED_BLACK_TREE_PARENT(pmm_red_black_tree_root)      = pmm_red_black_tree_null;
    PMM_RED_BLACK_TREE_LEFT_CHILD(pmm_red_black_tree_root)  = pmm_red_black_tree_null;
    PMM_RED_BLACK_TREE_RIGHT_CHILD(pmm_red_black_tree_root) = pmm_red_black_tree_null;

    // Initialize the current memory being addressed to the start of memory.
    void* current_memory = (void*)0;

    // Loop thru the entirety of memory.
    while (true) {

        /* Is the current memory address in a region of memory that is usable to
        the operating system? */
        if (Is_Physical_Memory_Region_Usable(mmap_info, current_memory)) {

            // Remember the first address of the usable memory region.
            void* first_usable_memory_addr = current_memory;

            /* Traverse forward into memory until an unusable memory region is 
            found or the next memory region's address is zero indicating there 
            is no next memory region. */
            uint64_t accumulated_memory_size = 0;
            while (Is_Physical_Memory_Region_Usable(mmap_info, current_memory)) {

                // Accumulate the sizes of the usable memory regions.
                accumulated_memory_size += Get_Size_of_Memory_Region (mmap_info, current_memory);

                /* If the calculated first address of the next memory region 
                does not match the first address of the next memory region there
                is an address gap; this is the end of this accumulated region 
                and go to the next region. */
                if (Get_Expected_First_Address_in_Next_Memory_Region (mmap_info, current_memory) != 
                    Get_Next_Memory_Region (mmap_info, current_memory)) {
                    current_memory = (void*)(Get_Next_Memory_Region (mmap_info, current_memory));
                    break;
                }
            
                // Move forward to next memory region.
                current_memory = (void*)(Get_Next_Memory_Region (mmap_info, current_memory));

                /* The next memory region's address is zero indicating there is 
                no next memory region. */
                if (current_memory == ((void*)0)) {
                    break;
                }

            }

            /* Coalesce memory by declaring the size of the first usable region
            as the accumulated memory size. The division by 8 is because the 
            aligned size is only the upper 61 bits. It is guaranteed to be a 
            multiple of 8 because the sizes are a multiple of the frame size 
            which is 4096 which is a multiple of 8. */
            physical_memory_size_and_flags size_and_flags;
            size_and_flags.aligned_size = accumulated_memory_size / 8;
            size_and_flags.is_allocated = 0;
            size_and_flags.reserved     = 0;
            *((physical_memory_size_and_flags*)first_usable_memory_addr) = size_and_flags;

            // Copy the size and flags to the boundary tag as well.
            physical_memory_boundary_tag boundary_tag;
            boundary_tag.size_and_flags = size_and_flags;
            *((physical_memory_boundary_tag*)(((uint8_t*)first_usable_memory_addr) + accumulated_memory_size - sizeof(physical_memory_boundary_tag))) = boundary_tag;

            // Insert the coalesced memory region into the red-black tree.
            pmm_red_black_tree_insert(first_usable_memory_addr);

        /* The current memory address is not in a region of memory that is 
        usable to the operating system. */
        } else {

            // Jump to the next memory region. 
            current_memory = (void*)(Get_Next_Memory_Region (mmap_info, current_memory));

        }

        /* The next memory region's address is zero indicating there is no next 
        memory region. */
        if (current_memory == 0) {
            break;
        }

    }
}

/******************************************************************************* 
Allocate Frame(s) of Physical Memory Function
Given a size of memory to allocate, find the smallest free memory region capable 
of fitting the frame-aligned size (best fit allocator), split the memory region 
to the frame-aligned size if necessary, remove the entry from red-black tree, 
and return a pointer to the address space which is the first address after the 
header for allocated memory.
*******************************************************************************/
void* Physical_Memory_Manager::allocate_physical_frames (uint64_t desired_size) {

    /* Add onto the size, the number of bytes the header for allocated memory
    and the boundary tag take. */
    uint64_t desired_size_modified = desired_size + sizeof(physical_memory_allocated_header) + sizeof(physical_memory_boundary_tag);

    // Round the desired size up to the nearest multiple of the size of a frame.
    desired_size_modified = (((uint64_t)(desired_size_modified / PMM_FRAME_SIZE)) * PMM_FRAME_SIZE) + PMM_FRAME_SIZE;

    // Find the free memory region that best fits the size of memory requested.
    void* best_fit_node = pmm_red_black_tree_find_best_fit(desired_size_modified);

    /* If a best fit does not exist i.e. insufficient memory, return the null 
    pointer. */
    if (best_fit_node == pmm_red_black_tree_null) {
        return nullptr;
    }

    /* Remove the best fit node form the red black tree because the memory it
    represents is now allocated. */
    pmm_red_black_tree_delete(best_fit_node);

    // Update header and boundary tag to say this region is now allocated.
    physical_memory_size_and_flags size_and_flags;
    size_and_flags.aligned_size = PMM_RED_BLACK_TREE_KEY_VALUE(best_fit_node) / 8;
    size_and_flags.is_allocated = 1;
    size_and_flags.reserved     = 0;
    *((physical_memory_size_and_flags*)best_fit_node) = size_and_flags;

    physical_memory_boundary_tag boundary_tag;
    boundary_tag.size_and_flags = size_and_flags;
    *((physical_memory_boundary_tag*)(((uint8_t*)best_fit_node) + PMM_RED_BLACK_TREE_KEY_VALUE(best_fit_node) - sizeof(physical_memory_boundary_tag))) = boundary_tag;

    /* If the best fit memory region is bigger than desired, split the region
    into a region of the desired size and a region of size of the remaining 
    memory. */
    if (PMM_RED_BLACK_TREE_KEY_VALUE(best_fit_node) > desired_size_modified) {

        /* Change the best fit node's size in the header and create a new 
        boundary tag. */
        physical_memory_size_and_flags size_and_flags;
        size_and_flags.aligned_size = desired_size_modified / 8;
        size_and_flags.is_allocated = 1;
        size_and_flags.reserved     = 0;
        *((physical_memory_size_and_flags*)best_fit_node) = size_and_flags;

        physical_memory_boundary_tag boundary_tag;
        boundary_tag.size_and_flags = size_and_flags;
        *((physical_memory_boundary_tag*)(((uint8_t*)best_fit_node) + desired_size_modified - sizeof(physical_memory_boundary_tag))) = boundary_tag;

        /* Create the new memory region of size of the remaining memory with a 
        header and boundary tag. */
        uint64_t size_of_new_node = PMM_RED_BLACK_TREE_KEY_VALUE(best_fit_node) - desired_size_modified;
        void* new_memory_region = (void*)(((uint8_t*)best_fit_node) + desired_size_modified);

        size_and_flags.aligned_size = size_of_new_node / 8;
        size_and_flags.is_allocated = 0;
        size_and_flags.reserved     = 0;
        *((physical_memory_size_and_flags*)new_memory_region) = size_and_flags;

        boundary_tag.size_and_flags = size_and_flags;
        *((physical_memory_boundary_tag*)(((uint8_t*)new_memory_region) + size_of_new_node - sizeof(physical_memory_boundary_tag))) = boundary_tag;

        // Add the new node to the red black tree.
        pmm_red_black_tree_insert(new_memory_region);

    }

    /* Return a pointer to the address after the header of the best fit memory 
    region. */ 
    return ((void *)(((uint8_t*)best_fit_node) + sizeof(physical_memory_allocated_header)));

}

/*******************************************************************************
Free Frame(s) of Physical Memory Function
Given a pointer to the address space free the memory allocated by updating the 
header and boundary tag, add the region into the red-black tree after coalescing
the freed space with other contigious free memory.
*******************************************************************************/
void Physical_Memory_Manager::free_physical_frames (void* memory_to_free) {

    /* The given void pointer is the pointer to the start of allocated memory,
    go back to a pointer to the header. */
    void* memory_to_free_modified = (void*)(((uint8_t*)(memory_to_free)) - sizeof(physical_memory_allocated_header));

    // Jump to the region on left.
    void* left_memory_address = (void*)(((uint8_t*)(memory_to_free_modified)) - sizeof(physical_memory_boundary_tag));
    bool left_is_free_and_usable = false;

    // Check if we can coalesce to the left. Check if memory region is usable.
    if (Is_Physical_Memory_Region_Usable(m_mmap_info, left_memory_address)) {

        // Is the memory region free?
        if (PMM_IS_ALLOCATED_MEMORY_FLAG(left_memory_address) == 0) {
            left_is_free_and_usable = true;
        }
    }

    /* Jump to beginning of the left memory region. */
    left_memory_address = (void*)(((uint8_t*)(left_memory_address)) - (PMM_RED_BLACK_TREE_KEY_VALUE(left_memory_address) - sizeof(physical_memory_boundary_tag)));

    // Jump to the region on right.
    void* right_memory_address = (void*)(((uint8_t*)(memory_to_free_modified)) + PMM_RED_BLACK_TREE_KEY_VALUE(memory_to_free_modified));
    bool right_is_free_and_usable = false;

    // Check if we can coalesce to the right. Check if memory region is usable.
    if (Is_Physical_Memory_Region_Usable(m_mmap_info, right_memory_address)) {

        // Is the memory region free?
        if (PMM_IS_ALLOCATED_MEMORY_FLAG(right_memory_address) == 0) {
            right_is_free_and_usable = true;
        }
    }

    // Coalesce with the left and right memory regions. 
    if (left_is_free_and_usable && right_is_free_and_usable) {

        // Calculate size of the coalesced region.
        uint64_t coalesced_size = PMM_RED_BLACK_TREE_KEY_VALUE(memory_to_free_modified) + PMM_RED_BLACK_TREE_KEY_VALUE(left_memory_address) + PMM_RED_BLACK_TREE_KEY_VALUE(right_memory_address);

        // Start from the left region and form the newly coalesced region.
        physical_memory_size_and_flags size_and_flags;
        size_and_flags.aligned_size = coalesced_size / 8;
        size_and_flags.is_allocated = 0;
        size_and_flags.reserved     = 0;
        *((physical_memory_size_and_flags*)left_memory_address) = size_and_flags;

        physical_memory_boundary_tag boundary_tag;
        boundary_tag.size_and_flags = size_and_flags;
        *((physical_memory_boundary_tag*)(((uint8_t*)left_memory_address) + coalesced_size - sizeof(physical_memory_boundary_tag))) = boundary_tag;

        /* Don't need to add the recently freed memory node to the red black 
        tree because it was coalesced into the left memory region. However, we 
        must remove the right region that was coalesced from the tree. The left
        memory region should already be in tree as it was free and usable. */ 
        pmm_red_black_tree_delete(right_memory_address);

    // Coalesce with the right memory region.
    } else if ((!left_is_free_and_usable) && right_is_free_and_usable) {
        
        // Calculate size of the coalesced region.
        uint64_t coalesced_size = PMM_RED_BLACK_TREE_KEY_VALUE(memory_to_free_modified) + PMM_RED_BLACK_TREE_KEY_VALUE(right_memory_address);

        /* Start from the recently freed memory region and form the newly 
        coalesced region. */
        physical_memory_size_and_flags size_and_flags;
        size_and_flags.aligned_size = coalesced_size / 8;
        size_and_flags.is_allocated = 0;
        size_and_flags.reserved     = 0;
        *((physical_memory_size_and_flags*)memory_to_free_modified) = size_and_flags;

        physical_memory_boundary_tag boundary_tag;
        boundary_tag.size_and_flags = size_and_flags;
        *((physical_memory_boundary_tag*)(((uint8_t*)memory_to_free_modified) + coalesced_size - sizeof(physical_memory_boundary_tag))) = boundary_tag;

        /* Insert the newly coalesced region into the tree that starts from the
        recently freed memory region. Remove the right region from the tree that 
        was coalesced.*/
        pmm_red_black_tree_insert(memory_to_free_modified);
        pmm_red_black_tree_delete(right_memory_address);

    // Coalesce with the left memory region.
    } else if (left_is_free_and_usable && (!right_is_free_and_usable)) {

        // Calculate size of the coalesced region.
        uint64_t coalesced_size = PMM_RED_BLACK_TREE_KEY_VALUE(memory_to_free_modified) + PMM_RED_BLACK_TREE_KEY_VALUE(left_memory_address);

        // Start from the left region and form the newly coalesced region.
        physical_memory_size_and_flags size_and_flags;
        size_and_flags.aligned_size = coalesced_size / 8;
        size_and_flags.is_allocated = 0;
        size_and_flags.reserved     = 0;
        *((physical_memory_size_and_flags*)left_memory_address) = size_and_flags;

        physical_memory_boundary_tag boundary_tag;
        boundary_tag.size_and_flags = size_and_flags;
        *((physical_memory_boundary_tag*)(((uint8_t*)left_memory_address) + coalesced_size - sizeof(physical_memory_boundary_tag))) = boundary_tag;

        /* Don't need to add the recently freed memory node to the red black 
        tree because it was coalesced into the left memory region. The left
        memory region should already be in tree as it was free and usable. The 
        right regon is not free and usable thus it does not belong in the tree. 
        */ 

    // No coalescing.
    } else {

        // The memory is no longer allocated.
        physical_memory_size_and_flags size_and_flags;
        size_and_flags.aligned_size = PMM_RED_BLACK_TREE_KEY_VALUE(memory_to_free_modified) / 8;
        size_and_flags.is_allocated = 0;
        size_and_flags.reserved     = 0;
        *((physical_memory_size_and_flags*)memory_to_free_modified) = size_and_flags;

        physical_memory_boundary_tag boundary_tag;
        boundary_tag.size_and_flags = size_and_flags;
        *((physical_memory_boundary_tag*)(((uint8_t*)memory_to_free_modified) + PMM_RED_BLACK_TREE_KEY_VALUE(memory_to_free_modified) - sizeof(physical_memory_boundary_tag))) = boundary_tag;

        /* Insert the recently freed memory region into the tree. The left and 
        right regions are not free and usable thus don't belong in the tree. */
        pmm_red_black_tree_insert(memory_to_free_modified);

    }
}
