#include <iostream>

typedef uint8_t kh_ubyte, kh_uint8;
typedef int8_t kh_byte, kh_int8;
typedef uint16_t kh_ushort, kh_uint16;
typedef int16_t kh_short, kh_int16;
typedef uint32_t kh_uint, kh_uint32;
typedef int32_t kh_int, kh_int32;
typedef uint64_t kh_ulong, kh_uint64;
typedef int64_t kh_long, kh_int64;

typedef float kh_float, kh_float32;
typedef double kh_double, kh_float64;

typedef char32_t kh_char;


class kh_std_mod {
public:
    void print(std::string str) {
        std::cout << str;
    }
    void println(std::string str) {
        std::cout << str << "\n";
    }
};