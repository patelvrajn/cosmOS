#include <stdint.h>

#define DEFINE_CONTROL_REGISTER_RW(index)       \
    /* Read value of control register into */   \
    /* cr-variable and return the value. */     \     
    uint64_t read_cr##index() {                 \
                                                \
        uint64_t cr##index;                     \
                                                \
        __asm__ __volatile__ (                  \
            "mov %%cr" #index ", %%rax\n\t"     \
            "mov %%rax, %0\n\t"                 \
            : "=rm" (cr##index)                 \
            : /* No input. */                   \
            : "%rax"                            \
        );                                      \
                                                \
    return cr##index;                           \
                                                \
    }                                           \
    /* Write value into control register */     \
    /* while preserving the value of rax by */  \
    /* pushing and popping to/from the */       \
    /* stack. */                                \
    void write_cr##index (uint64_t cr##index) { \
                                                \
        __asm__ __volatile__ ("pushq %rax");    \                    
        __asm__ __volatile__ (                  \
            "mov %%rax, %%cr" #index            \
            : /* No output. */                  \
            :"a"(cr##index)                     \
            :/* No clobbered. */                \
        );                                      \
        __asm__ __volatile__ ("popq %rax");     \
                                                \
    }

DEFINE_CONTROL_REGISTER_RW(0)
// DEFINE_CONTROL_REGISTER_RW(1); Reserved.
DEFINE_CONTROL_REGISTER_RW(2)
DEFINE_CONTROL_REGISTER_RW(3)
DEFINE_CONTROL_REGISTER_RW(4)
// DEFINE_CONTROL_REGISTER_RW(5); Reserved.
// DEFINE_CONTROL_REGISTER_RW(6); Reserved.
// DEFINE_CONTROL_REGISTER_RW(7); Reserved.
DEFINE_CONTROL_REGISTER_RW(8)
