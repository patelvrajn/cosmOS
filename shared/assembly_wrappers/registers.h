#pragma once
#include <stdint.h>

#define DEFINE_CONTROL_REGISTER_RW_PROTO(index) \
    uint64_t read_cr##index();                  \
    void write_cr##index (uint64_t cr##index);

DEFINE_CONTROL_REGISTER_RW_PROTO(0);
// DEFINE_CONTROL_REGISTER_RW_PROTO(1); Reserved.
DEFINE_CONTROL_REGISTER_RW_PROTO(2);
DEFINE_CONTROL_REGISTER_RW_PROTO(3);
DEFINE_CONTROL_REGISTER_RW_PROTO(4);
// DEFINE_CONTROL_REGISTER_RW_PROTO(5); Reserved.
// DEFINE_CONTROL_REGISTER_RW_PROTO(6); Reserved.
// DEFINE_CONTROL_REGISTER_RW_PROTO(7); Reserved.
DEFINE_CONTROL_REGISTER_RW_PROTO(8);
