#ifndef _SCOTTZ0R_DEBUG_SERIAL_H_INCLUDE_GUARD
#define _SCOTTZ0R_DEBUG_SERIAL_H_INCLUDE_GUARD

#ifndef NDEBUG
#define DEBUG_PRINT(x) (Serial.print(x))
#define DEBUG_PRINTLN(x) (Serial.println(x))
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

#endif // _SCOTTZ0R_DEBUG_SERIAL_H_INCLUDE_GUARD
