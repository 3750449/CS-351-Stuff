//////////////////////////////////////////////////////////////////////////////
//
//  julia.cu - A CUDA-based Julia set generator
//

#include <complex>
#include <fstream>
#include <iostream>
#include <vector>

#define DEBUG
#include "CudaCheck.h"

//----------------------------------------------------------------------------
//
//  Global configuration parameters
//

const size_t Width = 1024;
const size_t Height = 1024;
const size_t MaxIterations = 1000;

//----------------------------------------------------------------------------
//
//  struct Color
//
//  A simple struture for storing a color as an RGB triple.  In this case,
//    the color components are stored as unsigned chars, which is what's
//    used in the PPM format.
//
//  This structure is used both on the CPU and the GPU, and so the
//    constructor is tagged to have a version of the function generated for
//    both the CPU (signified by the __host__ decoration), and for the
//    GPU (__device__).
//

struct Color {
    using T = unsigned char;
    T r = 0;
    T g = 0;
    T b = 0;
    
    __host__ __device__
    Color() { /* Empty */ }

    __host__ __device__ 
    Color(T r, T g, T b) : r(r), g(g), b(b) { /* Empty */}
};

//----------------------------------------------------------------------------
//
//  function setColor()
//
//  This function returns a color based on an iteration value.
//
//  As this function is called from an executing GPU kernel (julia), it's
//    tagged as being a device-only function (using the __device__ decoration)
//

__device__
Color setColor(int iterations) {
    constexpr size_t NumColors = 16;
    const Color colors[NumColors] = {
        Color(66, 30, 15),
        Color(25, 7, 26),
        Color(9, 1, 47),
        Color(4, 4, 73),
        Color(0, 7, 100),
        Color(12, 44, 138),
        Color(24, 82, 177),
        Color(57, 125, 209),
        Color(134, 181, 229),
        Color(211, 236, 248),
        Color(241, 233, 191),
        Color(248, 201, 95),
        Color(255, 170, 0),
        Color(204, 128, 0),
        Color(153, 87, 0),
        Color(106, 52, 3)
    };

    const Color black;

    return iterations < MaxIterations ? colors[iterations % NumColors] : black;
}

//----------------------------------------------------------------------------
//
//  template struct TComplex (for complex numbers)
//
//  A proper C++ class (a struct is merely a C++ class with the default
//    member set to "public", where a class is "private" by default) for
//    storing a complex value.  In this case, we use 'x' as the real
//    component, and 'y' for the imaginary component.
//
//  For the purposes of the program, we use a typedef (using a modern
//    C++ using statement) to define a float-based complex value.
//
//  More interesting is that a number of the class methods are decorated
//    with __host__ and __device__ depending on their use in the program.
//    For example, the constructor is used both on the CPU, and in the GPU
//    kernel.  As such, that function needs to be decorated with both options.
//    By way of comparison, the function magnitude() is only used (in this
//    case) in the julia device kernel, so it's tagged as __device__.
//
//  Any function that isn't decorated is compiled for the CPU only by default.
//

template <typename T>
struct TComplex {
    T x = T(0);
    T y = T(0);

    TComplex() = default;

    __host__ __device__
    TComplex(T x, T y) : x(x), y(y) {}

    __device__
    T magnitude() const 
        { return sqrt(x*x + y*y); }

    __device__
    TComplex& operator -= (const TComplex& a)
        { x -= a.x; y -= a.y; return *this; }

    __device__ __host__
    friend TComplex operator + (const TComplex& a, const TComplex& b)
        { return TComplex(a.x + b.x, a.y + b.y); }

    __device__
    friend TComplex operator * (const TComplex& a, const TComplex& b)
        { return TComplex(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x); }

    friend TComplex operator - (const TComplex& a, const TComplex& b)
        { return TComplex(a.x - b.x, a.y - b.y); }

    friend TComplex operator * (const float s, const TComplex& a)
        { return TComplex(s * a.x, s * a.y); }

    friend std::ostream& operator << (std::ostream& os, const TComplex& p)
        { return os << "(" << p.x << ", " << p.y << ")"; }
};

using Complex = TComplex<float>;

//----------------------------------------------------------------------------
//
//  compute kernel julia
//
//  A Julia set generator kernel, which iterates complex values to determine
//    if they converge, or not.
//
//  The parameters for the kernel are:
//
//  - d, which is declared as a complex number is really just conviently
//      storing the "pixel size" in its value.  The values in 'd' are used
//      to determine which coordinate in the complex plane we're working
//      with.
//
//  - center, which is the complex coordinate of the center of our Julia
//      set.  The points we use in our iteration computations are centered
//      around this center value.
//
//  - pixels, which is the "image", a collection of Color structures that
//      will be written out to form our PPM image.
//
//  This CUDA kernel is tagged __global__, as it's executed on the GPU, but
//    launched (dispatched) from the CPU.
//

inline __device__ float magnitude(const Complex& z) { return z.magnitude(); }

__global__
void julia(Complex d, Complex center, Color* pixels) {
    // Add your CUDA implementation of the Julia program here.
    //
    // Hint: this function should basically be the same thing as the body
    //   of the two for loops in the C++ version.  If you're clever, which
    //   means you choose your variable names well (just like Phil mentions)
    //   you can pretty much drop in the CPU code, and then do the extra
    //   CUDA bits

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= Width || y >= Height) return;

    // Map screen space to complex plane
    Complex z(x * d.x, y * d.y);
    z -= center;

    // Use a fixed Julia constant to guarantee visible fractal
    Complex c(-0.8f, 0.156f);

    int iter = 0;
    while (iter < MaxIterations && magnitude(z) < 2.0f) {
        z = z * z + c;
        ++iter;
    }

    pixels[x + y * Width] = setColor(iter);

}

//----------------------------------------------------------------------------
//
//  function main()
//
//  Nothing particularly special here.  We specify the domain in the Complex
//    plane we're interested in looking at using its lower-left, and upper-
//    right corners (the variables "ll", and "ur", respectively).
//
//  From there, we determine the size of the domain, its center point, and
//    the size of a pixel in the complex plane.
//
//  Then we allocate some GPU memory, specify the number of threads we
//    want to execute in parallel (which we set in the "blockDim" variable).
//    Using the block's dimensions, we specify how many blocks are required
//    in each of the dimensions to populate our image, and then we dispatch
//    our kernel to do its work.
//
//  For our CUDA-specific operations, we verify their proper operation
//    using some modifications of our CUDA checking macros.  Of particular
//    note is for the CUDA_CHECK_KERNEL() macro, the kernel dispatch needs
//    to be wrapped in an extra set of parentheses to trick the C preprocessor
//    that there's only a single parameter (the comma in the kernel's 
//    dispatch messes things up).
//
//  Once the kernel's done, we copy the results from the GPU back to the
//    CPU, and output the results in an image named "julia.ppm".
//

int main() {
    Complex ll(-2.1, -2.1);
    Complex ur( 2.1,  2.1);
    Complex domain = ur - ll;
    Complex center = 0.5 * domain;
    Complex d(domain.x/Width, domain.y/Height);

    Color* gpuPixels;
    size_t numBytes = Width * Height * sizeof(Color);
    CUDA_CHECK_CALL(cudaMalloc(&gpuPixels, numBytes));

    dim3 blockDim(32, 32);
    dim3 numBlocks(Width/blockDim.x, Height/blockDim.y);
    CUDA_CHECK_KERNEL( (julia<<<numBlocks, blockDim>>>(d, center, gpuPixels)) );

    Color* pixels = new Color[Width * Height];
    CUDA_CHECK_CALL(cudaMemcpy(pixels, gpuPixels, numBytes, cudaMemcpyDeviceToHost));

    std::ofstream ppm("julia.ppm", std::ios::binary);
    ppm << "P6 " << Width << " " << Height << " " << 255 << "\n";
    ppm.write(reinterpret_cast<const char*>(&pixels[0]), numBytes);
}
