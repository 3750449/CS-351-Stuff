
#include <complex>
#include <fstream>
#include <vector>
#include <iostream>

//----------------------------------------------------------------------------
//
//  Global configuration parameters
//

const size_t MaxIterations = 1000;
using Complex = std::complex<float>;

Complex operator*(Complex::value_type s, const Complex& z) {
    return Complex(s * z.real(), s * z.imag());
}

struct Color {
    using T = unsigned char;
    T r = 0;
    T g = 0;
    T b = 0;

    Color() = default;
    Color(T r, T g, T b) : r(r), g(g), b(b) {}
};

//----------------------------------------------------------------------------
//
//  function setColor()
//

Color setColor(int iterations) {
    constexpr size_t NumColors = 16;
    const Color colors[NumColors] = {
        Color(66, 30, 15),   Color(25, 7, 26),   Color(9, 1, 47),     Color(4, 4, 73),
        Color(0, 7, 100),    Color(12, 44, 138), Color(24, 82, 177),  Color(57, 125, 209),
        Color(134, 181, 229),Color(211, 236, 248),Color(241, 233, 191),Color(248, 201, 95),
        Color(255, 170, 0),  Color(204, 128, 0),  Color(153, 87, 0),   Color(106, 52, 3)
    };

    const Color black;
    return iterations < MaxIterations ? colors[iterations % NumColors] : black;
}

inline float magnitude(const Complex& z) { return std::abs(z); }

//----------------------------------------------------------------------------
//
//  function main()
//
//  Now accepts an optional command-line argument for image size.
//

int main(int argc, char* argv[]) {
    // --- NEW: Accept size argument from command line
    size_t size = (argc > 1) ? std::stoul(argv[1]) : 1024;
    size_t width = size;
    size_t height = size;

    Complex ll(-2.1, -2.1);
    Complex ur(2.1, 2.1);
    Complex domain = ur - ll;
    Complex center = 0.5f * domain;
    Complex d(domain.real() / width, domain.imag() / height);  // --- CHANGED: use variable dimensions

    std::vector<Color> pixels(width * height);  // --- CHANGED: use dynamic size

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            Complex c(x * d.real(), y * d.imag());
            c -= center;
            Complex z;

            int iter = 0;
            while (iter < MaxIterations && magnitude(z) < 2.0f) {
                z = z * z + c;
                ++iter;
            }

            pixels[x + y * width] = setColor(iter);  // --- CHANGED: use variable width
        }
    }

    std::ofstream ppm("julia.ppm", std::ios::binary);
    ppm << "P6 " << width << " " << height << " " << 255 << "\n";  // --- CHANGED: use variable dimensions
    ppm.write(reinterpret_cast<const char*>(&pixels[0]), pixels.size() * sizeof(Color));  // --- CHANGED: write all data
}
