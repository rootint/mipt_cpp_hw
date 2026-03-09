#include <iostream>

int log2_int(int value)
{
    unsigned int n = static_cast<unsigned int>(value);
    int result = 0;
    n >>= 1;
    while (n != 0)
    {
        result++;
        n >>= 1;
    }
    return result;
}

int log2_float(float value)
{
    union FloatBits
    {
        float f;
        unsigned int u;
    };

    FloatBits converter;
    converter.f = value;
    unsigned int bits = converter.u;

    unsigned int exponent = (bits >> 23) & 0xFFu;
    unsigned int fraction = bits & 0x7FFFFFu;

    if (exponent == 0xFFu)
    {
        return -1;
    }

    if (exponent != 0)
    {
        return static_cast<int>(exponent) - 127;
    }

    unsigned int f = fraction;
    int shift = 0;
    while ((f & 0x400000u) == 0)
    {
        f <<= 1;
        shift++;
    }
    return -126 - shift - 1;
}

int main()
{
    std::cout << "=== log2 for positive int ===" << std::endl;
    int test_ints[] = {1, 2, 3, 4, 5, 7, 8, 15, 16, 100, 1023, 1024, 65536};
    for (int v : test_ints)
    {
        std::cout << "  log2(" << v << ") = " << log2_int(v) << std::endl;
    }

    std::cout << std::endl
              << "=== log2 for positive float ===" << std::endl;

    std::cout << "Normalized:" << std::endl;
    float test_floats[] = {1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 0.5f, 0.25f, 0.1f,
                           37.625f, 67.125f, 1024.0f, 1e10f, 1e-10f};
    for (float v : test_floats)
    {
        std::cout << "  log2(" << v << ") = " << log2_float(v) << std::endl;
    }

    std::cout << "Denormalized:" << std::endl;
    union
    {
        float f;
        unsigned int u;
    } small;

    small.u = 0x00000001u;
    std::cout << "  log2(" << small.f << ") = " << log2_float(small.f) << "  (min denorm)" << std::endl;

    small.u = 0x007FFFFFu;
    std::cout << "  log2(" << small.f << ") = " << log2_float(small.f) << "  (max denorm)" << std::endl;

    small.u = 0x00400000u;
    std::cout << "  log2(" << small.f << ") = " << log2_float(small.f) << std::endl;

    std::cout << std::endl
              << "Special values:" << std::endl;
    float inf = 1.0f / 0.0f;
    float nan_val = 0.0f / 0.0f;
    std::cout << "  log2(inf) = " << log2_float(inf) << "  (error: -1)" << std::endl;
    std::cout << "  log2(nan) = " << log2_float(nan_val) << "  (error: -1)" << std::endl;

    return 0;
}

/*
=== log2 for positive int ===
  log2(1) = 0
  log2(2) = 1
  log2(3) = 1
  log2(4) = 2
  log2(5) = 2
  log2(7) = 2
  log2(8) = 3
  log2(15) = 3
  log2(16) = 4
  log2(100) = 6
  log2(1023) = 9
  log2(1024) = 10
  log2(65536) = 16

=== log2 for positive float ===
Normalized:
  log2(1) = 0
  log2(1.5) = 0
  log2(2) = 1
  log2(3) = 1
  log2(4) = 2
  log2(0.5) = -1
  log2(0.25) = -2
  log2(0.1) = -4
  log2(37.625) = 5
  log2(67.125) = 6
  log2(1024) = 10
  log2(1e+10) = 33
  log2(1e-10) = -34
Denormalized:
  log2(1.4013e-45) = -149  (min denorm)
  log2(1.17549e-38) = -127  (max denorm)
  log2(5.87747e-39) = -127

Special values:
  log2(inf) = -1  (error: -1)
  log2(nan) = -1  (error: -1)
*/