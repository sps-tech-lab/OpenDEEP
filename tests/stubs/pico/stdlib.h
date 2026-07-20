//
// Host test stub for <pico/stdlib.h>.
// Provides just enough of the Pico SDK surface used by the modules
// under test so they can be compiled and linked on the host.
//
#ifndef OPENDEEP_TEST_STUB_PICO_STDLIB_H
#define OPENDEEP_TEST_STUB_PICO_STDLIB_H

#include <cstddef>
#include <cstdint>

#ifndef PICO_OK
#define PICO_OK 0
#endif

#ifndef PICO_ERROR_GENERIC
#define PICO_ERROR_GENERIC (-1)
#endif

// No-op on the host (busy-wait would only slow the tests down).
void sleep_ms(uint32_t ms);

#endif // OPENDEEP_TEST_STUB_PICO_STDLIB_H
