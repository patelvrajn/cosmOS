#include <stdarg.h>
#include "uefi.h"

// TODO: Can generalize these functions?

void uefi_print_number (UEFI_SYSTEM_TABLE* SystemTable, int32_t number, int32_t base) {
    
    /*Digits for bases 1 to 16*/
    char16_t* digits = u"0123456789ABCDEF";

    /*INT32 ranges from -2,147,483,648 to 2,147,483,647 so 10 digits + sign + null character */
    const uint32_t BUFFER_SIZE = 12; // TODO: May not be large enough for all use cases!!!
    char16_t charbuffer[BUFFER_SIZE];

    uint32_t i;
    bool isNegative = (number < 0);

    // Flip sign if negative for digit logic to work.
    if (isNegative) number = -1 * number;

    // Get digits from number in reverse order.
    i = 0;
    do {
        charbuffer[i++] = digits[number % base];
        number /= base;
    } while (number > 0);

    // Append the negative sign if negative.
    if (isNegative) charbuffer[i++] = u'-';

    // Null terminate buffer and go to index of last digit/sign.
    charbuffer[i--] = u'\0';

    // Reverse digits in buffer before printing.
    for (uint32_t j = 0; j < i; j++, i--) {
        char16_t temp = charbuffer[i];
        charbuffer[i] = charbuffer[j];
        charbuffer[j] = temp;
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, charbuffer);

}

void uefi_printf (UEFI_SYSTEM_TABLE* SystemTable, char16_t* unformatted_string, ...) {

    char16_t charbuffer[2] = {'\0', '\0'};

    va_list v_args;
    va_start(v_args, unformatted_string);

    /* Loop thru unformatted string until you hit the terminating null
    character.*/
    for (uint64_t idx = 0; unformatted_string[idx] != '\0'; idx++) {

        /*Found % in unformatted string.*/
        if (unformatted_string[idx] == '%') {

            /* Get next character to determine type of argument.*/
            idx++;

            /*Format specifier switch statement.*/
            switch (unformatted_string[idx]) {

                /*Case %s : Char16 string*/
                case 's': {

                    SystemTable->ConOut->OutputString(SystemTable->ConOut, va_arg(v_args, char16_t*));

                } break;

                /*Case %d : Int32*/
                case 'i': {

                    int32_t number = va_arg(v_args, int32_t);
                    uefi_print_number(SystemTable, number, 10);

                } break;

                /*If none of the format specifiers, print and move on.*/
                default: {
                    charbuffer[0] = unformatted_string[idx];
                    SystemTable->ConOut->OutputString(SystemTable->ConOut, charbuffer);
                }
            }

        /*No formatting needed, print character.*/
        } else {
            charbuffer[0] = unformatted_string[idx];
            SystemTable->ConOut->OutputString(SystemTable->ConOut, charbuffer);
        }
    }

    va_end (v_args);

}

UEFI_INPUT_KEY uefi_wait_for_keystroke (UEFI_SYSTEM_TABLE* SystemTable) {
    
    const uint64_t NUM_OF_EVENTS = 1;
    UEFI_EVENT event[NUM_OF_EVENTS];
    UEFI_INPUT_KEY k;
    uint64_t idx;

    /*Add wait for key to event array.*/
    event[0] = SystemTable->ConIn->WaitForKey;

    /*Halts execution and waits for the singular keystroke event.*/
    SystemTable->BootServices->WaitForEvent(NUM_OF_EVENTS, event, &idx);

    /*Read keystroke.*/
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &k);

    return k;
}
