//////////////////////////////////////////////////////////////////////////////
//
//  CudeCheck.h
//
//  CUDA_CHECK() is a macro for wrapping calls to CUDA runtime functions
//    to verify their proper execution
//
//  CUDA_CHECK_KERNEL() is a similar macro for verifying that a CUDA kernel
//    executed as expected
//

#ifndef __CUDACHECK_H__
#define __CUDACHECK_H__

#ifdef DEBUG
  #define CUDA_CHECK_CALL(f) do { \
      auto err = (f); \
      if (err != cudaSuccess) { \
          printf("[%s:%d] '%s' failed: %s\n", __FILE__, __LINE__, #f, \
              cudaGetErrorString(err)); \
          exit(EXIT_FAILURE); \
      } \
  } while (0)

  #define CUDA_CHECK_KERNEL(k) do { \
    k; \
    auto err = cudaGetLastError(); \
    if (err != cudaSuccess) { \
        printf("[%s:%d] '%s' failed: %s\n", __FILE__, __LINE__, #k, \
            cudaGetErrorString(err)); \
        exit(EXIT_FAILURE); \
    } \
  } while (0)
#else
  #define CUDA_CHECK_CALL(f) f
  #define CUDA_CHECK_KERNEL(k) k
#endif // DEBUG

#endif // __CUDACHECK_H__