#ifndef __khrplatform_h_
#define __khrplatform_h_

/* Minimal subset of KHR/khrplatform.h sufficient for our hand-rolled GL loader. */

#include <stdint.h>
#include <stddef.h>

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#   define KHRONOS_APIENTRY __stdcall
#else
#   define KHRONOS_APIENTRY
#endif

#define KHRONOS_APIATTRIBUTES

typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
typedef float                   khronos_float_t;
typedef signed   char           khronos_int8_t;
typedef unsigned char           khronos_uint8_t;
typedef signed   short int      khronos_int16_t;
typedef unsigned short int      khronos_uint16_t;
typedef intptr_t                khronos_intptr_t;
typedef uintptr_t               khronos_uintptr_t;
typedef ptrdiff_t               khronos_ssize_t;
typedef size_t                  khronos_usize_t;

typedef enum {
    KHRONOS_FALSE = 0,
    KHRONOS_TRUE  = 1,
    KHRONOS_BOOLEAN_ENUM_FORCE_SIZE = 0x7FFFFFFF
} khronos_boolean_enum_t;

#endif /* __khrplatform_h_ */
