#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef i32 bool32;

// utility macro stuff
#define array_len(x) (sizeof(x) / sizeof(x[0]))

// tab is 4 spaces.
#define TAB "    "

#endif
