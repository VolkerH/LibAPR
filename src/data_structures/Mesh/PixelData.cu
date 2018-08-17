#include "PixelDataCuda.h"
#include <iostream>
#include <memory>

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <device_functions.h>

#include "misc/CudaTools.cuh"

#include "downsample.cuh"

// explicit instantiation of handled types
template void downsampleMeanCuda(const PixelData<float>&, PixelData<float>&);
template void downsampleMaxCuda(const PixelData<float>&, PixelData<float>&);

template <typename T, typename S>
void downsampleMeanCuda(const PixelData<T> &input, PixelData<S> &output) {
    APRTimer timer(true);

    timer.start_timer("cuda: memory alloc + data transfer to device");

    size_t inputSize = input.mesh.size() * sizeof(T);
    T *cudaInput;
    cudaMalloc(&cudaInput, inputSize);
    cudaMemcpy(cudaInput, input.mesh.get(), inputSize, cudaMemcpyHostToDevice);

    size_t outputSize = output.mesh.size() * sizeof(float);
    float *cudaOutput;
    cudaMalloc(&cudaOutput, outputSize);
    cudaMemcpy(cudaOutput, output.mesh.get(), outputSize, cudaMemcpyHostToDevice);
    timer.stop_timer();

    runDownsampleMean(cudaInput, cudaOutput, input.x_num, input.y_num, input.z_num, 0);

    timer.start_timer("cuda: transfer data from device and freeing memory");
    cudaFree(cudaInput);
    cudaMemcpy((void*)output.mesh.get(), cudaOutput, outputSize, cudaMemcpyDeviceToHost);
    cudaFree(cudaOutput);
    timer.stop_timer();
};

template <typename T, typename S>
void downsampleMaxCuda(const PixelData<T> &input, PixelData<S> &output) {
    APRTimer timer(true);

    timer.start_timer("cuda: memory alloc + data transfer to device");

    size_t inputSize = input.mesh.size() * sizeof(T);
    T *cudaInput;
    cudaMalloc(&cudaInput, inputSize);
    cudaMemcpy(cudaInput, input.mesh.get(), inputSize, cudaMemcpyHostToDevice);

    size_t outputSize = output.mesh.size() * sizeof(float);
    float *cudaOutput;
    cudaMalloc(&cudaOutput, outputSize);
    cudaMemcpy(cudaOutput, output.mesh.get(), outputSize, cudaMemcpyHostToDevice);
    timer.stop_timer();

    runDownsampleMax(cudaInput, cudaOutput, input.x_num, input.y_num, input.z_num, 0);

    timer.start_timer("cuda: transfer data from device and freeing memory");
    cudaFree(cudaInput);
    cudaMemcpy((void*)output.mesh.get(), cudaOutput, outputSize, cudaMemcpyDeviceToHost);
    cudaFree(cudaOutput);
    timer.stop_timer();
};
