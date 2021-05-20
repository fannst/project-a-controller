#include "print.h"

/// Printf version which is meant for socket connections.
void ip_printf (ip_addr_t addr, const char *fmt, ...) {
    // Prints the address prefix.
    printf ("[%lu : %u.%u.%u.%u] -> ",
        HAL_GetTick (),
        ip4_addr_get_byte (&addr, 0),
        ip4_addr_get_byte (&addr, 1),
        ip4_addr_get_byte (&addr, 2),
        ip4_addr_get_byte (&addr, 3));

    // Calls the printf method with the specified __va_args__.
    va_list args;
    va_start (args, fmt);
    vprintf (fmt, args);
    va_end (args);

    // Prints the newline (\r required for screen command).
    printf ("\r\n");
}
