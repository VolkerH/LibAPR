//
// Created by cheesema on 09.03.18.
//
#include <algorithm>
#include <vector>
#include <array>
#include <iostream>
#include <cassert>
#include <limits>
#include <chrono>
#include <iomanip>

#include "data_structures/APR/APR.hpp"
#include "data_structures/APR/APRTreeIterator.hpp"
#include "data_structures/APR/ExtraParticleData.hpp"
#include "data_structures/Mesh/MeshData.hpp"
#include "io/TiffUtils.hpp"

#include "thrust/device_vector.h"
#include "thrust/tuple.h"
#include "thrust/copy.h"
#include "../src/misc/APRTimer.hpp"
#include "../src/data_structures/APR/ExtraParticleData.hpp"

#include "GPUAPRAccess.hpp"
#include "../src/data_structures/APR/APR.hpp"

struct cmdLineOptions{
    std::string output = "output";
    std::string stats = "";
    std::string directory = "";
    std::string input = "";
    int num_rep = 100;
};

bool command_option_exists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

char* get_command_option(char **begin, char **end, const std::string &option) {
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    }
    return 0;
}

cmdLineOptions read_command_line_options(int argc, char **argv) {
    cmdLineOptions result;

    if(argc == 1) {
        std::cerr << "Usage: \"Example_apr_neighbour_access -i input_apr_file -d directory\"" << std::endl;
        exit(1);
    }
    if(command_option_exists(argv, argv + argc, "-i")) {
        result.input = std::string(get_command_option(argv, argv + argc, "-i"));
    } else {
        std::cout << "Input file required" << std::endl;
        exit(2);
    }
    if(command_option_exists(argv, argv + argc, "-d")) {
        result.directory = std::string(get_command_option(argv, argv + argc, "-d"));
    }
    if(command_option_exists(argv, argv + argc, "-o")) {
        result.output = std::string(get_command_option(argv, argv + argc, "-o"));
    }

    if(command_option_exists(argv, argv + argc, "-numrep"))
    {
        result.num_rep = std::stoi(std::string(get_command_option(argv, argv + argc, "-numrep")));
    }

    return result;
}


__global__ void down_sample_avg_mid(const std::size_t *row_info,
                                const std::uint16_t *particle_y,
                                const std::size_t* level_offset,
                                const std::uint16_t *particle_data_input,
                                const std::size_t *row_info_child,
                                const std::uint16_t *particle_y_child,
                                const std::size_t* level_offset_child,
                                std::float_t *particle_data_output,
                                const std::uint16_t* level_x_num,
                                const std::uint16_t* level_z_num,
                                const std::uint16_t* level_y_num,
                                const std::size_t level);

__global__ void down_sample_avg(const std::size_t *row_info,
                                const std::uint16_t *particle_y,
                                const std::size_t* level_offset,
                                const std::uint16_t *particle_data_input,
                                const std::size_t *row_info_child,
                                const std::uint16_t *particle_y_child,
                                const std::size_t* level_offset_child,
                                std::float_t *particle_data_output,
                                const std::uint16_t* level_x_num,
                                const std::uint16_t* level_z_num,
                                const std::uint16_t* level_y_num,
                                const std::size_t level);

__global__ void shared_update(const std::size_t *row_info,
                              const std::uint16_t *particle_y,
                              const std::uint16_t *particle_data_input,
                              std::uint16_t *particle_data_output,
                              const std::size_t offset,
                              const std::size_t x_num,
                              const std::size_t z_num,
                              const std::size_t y_num,
                              const std::size_t level);

__global__ void down_sample_y_update(const std::size_t *row_info,
                                     const std::uint16_t *particle_y,
                                     const std::size_t* level_offset,
                                     const std::uint16_t *particle_data_input,
                                     const std::size_t *row_info_child,
                                     const std::uint16_t *particle_y_child,
                                     const std::size_t* level_offset_child,
                                     std::float_t *particle_data_output,
                                     const std::uint16_t* level_x_num,
                                     const std::uint16_t* level_z_num,
                                     const std::uint16_t* level_y_num,
                                     const std::size_t level);

__global__ void down_sample_avg_interior(const std::size_t *row_info,
                                         const std::uint16_t *particle_y,
                                         const std::size_t* level_offset,
                                         const std::uint16_t *particle_data_input,
                                         const std::size_t *row_info_child,
                                         const std::uint16_t *particle_y_child,
                                         const std::size_t* level_offset_child,
                                         std::float_t *particle_data_output,
                                         const std::uint16_t* level_x_num,
                                         const std::uint16_t* level_z_num,
                                         const std::uint16_t* level_y_num,
                                         const std::size_t level);


ExtraParticleData<float> meanDownsamplingOld(APR<uint16_t> &aInputApr, APRTree<uint16_t> &aprTree) {
    APRIterator<uint16_t> aprIt(aInputApr);
    APRTreeIterator<uint16_t> treeIt(aprTree);
    APRTreeIterator<uint16_t> parentTreeIt(aprTree);
    ExtraParticleData<float> outputTree(aprTree);
    ExtraParticleData<uint8_t> childCnt(aprTree);
    auto &intensities = aInputApr.particles_intensities;

    for (unsigned int level = aprIt.level_max(); level >= aprIt.level_min(); --level) {
        for (size_t particle_number = aprIt.particles_level_begin(level);
             particle_number < aprIt.particles_level_end(level);
             ++particle_number)
        {
            aprIt.set_iterator_to_particle_by_number(particle_number);
            parentTreeIt.set_iterator_to_parent(aprIt);

            auto val = intensities[aprIt];
            outputTree[parentTreeIt] += val;
            childCnt[parentTreeIt]++;
        }
    }

    //then do the rest of the tree where order matters (it goes to level_min since we need to eventually average data there).
    for (unsigned int level = treeIt.level_max(); level >= treeIt.level_min(); --level) {
        // average intensities first
        for (size_t particleNumber = treeIt.particles_level_begin(level);
             particleNumber < treeIt.particles_level_end(level);
             ++particleNumber)
        {
            treeIt.set_iterator_to_particle_by_number(particleNumber);
            outputTree[treeIt] /= (1.0*childCnt[treeIt]);
        }

        // push changes
        if (level > treeIt.level_min())
            for (uint64_t parentNumber = treeIt.particles_level_begin(level);
                 parentNumber < treeIt.particles_level_end(level);
                 ++parentNumber)
            {
                treeIt.set_iterator_to_particle_by_number(parentNumber);
                if (parentTreeIt.set_iterator_to_parent(treeIt)) {
                    outputTree[parentTreeIt] += outputTree[treeIt];
                    childCnt[parentTreeIt]++;
                }
            }
    }
    return outputTree;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
    // Read provided APR file
    cmdLineOptions options = read_command_line_options(argc, argv);

    std::string fileName = options.directory + options.input;
    APR<uint16_t> apr;
    apr.read_apr(fileName);

    // Get dense representation of APR
    APRIterator<uint16_t> aprIt(apr);

    APRTimer timer;
    timer.verbose_flag = false;


    /*
     * Set up the GPU Access data structures
     *
     */
    GPUAPRAccess gpuaprAccess(aprIt);

    // Get dense representation of APR tree
    timer.start_timer("gen tree");
    APRTree<uint16_t> aprTree(apr);
    timer.stop_timer();

    APRTreeIterator<uint16_t> treeIt(aprTree);

    GPUAPRAccess gpuaprAccessTree(treeIt);

    /*
     *
     *  Calculate the down-sampled Particle Values
     *
     *
     */

    timer.start_timer("generate ds particles");
    ExtraParticleData<float> ds_parts =  meanDownsamplingOld(apr, aprTree);
    timer.stop_timer();

    std::cout << "Number parts: " << aprIt.total_number_particles() << " number in interior tree: " << ds_parts.data.size() << std::endl;


    //ds_parts.copy_data_to_gpu();


    int number_reps = options.num_rep;




    /*
    *  Test the x,y,z,level information is correct
    *
    */


    apr.particles_intensities.copy_data_to_gpu();


    //copy data back from gpu

    bool success = true;

    uint64_t c_fail= 0;
    uint64_t c_pass= 0;


    ExtraParticleData<float> tree_mean_gpu(aprTree);
    tree_mean_gpu.init_gpu(aprTree.total_number_parent_cells());



    for (int i = 0; i < 3; ++i) {

        timer.start_timer("summing the sptial informatino for each partilce on the GPU");
        for (int rep = 0; rep < number_reps; ++rep) {

            for (int level = apr.level_max(); level > aprIt.level_min(); --level) {

                std::size_t number_rows_l = apr.spatial_index_x_max(level) * apr.spatial_index_z_max(level);
                std::size_t offset = gpuaprAccess.h_level_offset[level];

                std::size_t x_num = apr.spatial_index_x_max(level);
                std::size_t z_num = apr.spatial_index_z_max(level);
                std::size_t y_num = apr.spatial_index_y_max(level);

                dim3 threads_l(128, 1, 1);

                int x_blocks = (x_num + 2 - 1) / 2;
                int z_blocks = (z_num + 2 - 1) / 2;

                dim3 blocks_l(x_blocks, 1, z_blocks);

                if(level==apr.level_max()) {

                    down_sample_avg << < blocks_l, threads_l >> >
                                                   (gpuaprAccess.gpu_access.row_global_index,
                                                           gpuaprAccess.gpu_access.y_part_coord,
                                                           gpuaprAccess.gpu_access.level_offsets,
                                                           apr.particles_intensities.gpu_pointer,
                                                           gpuaprAccessTree.gpu_access.row_global_index,
                                                           gpuaprAccessTree.gpu_access.y_part_coord,
                                                           gpuaprAccessTree.gpu_access.level_offsets,
                                                           tree_mean_gpu.gpu_pointer,
                                                           gpuaprAccess.gpu_access.level_x_num,
                                                           gpuaprAccess.gpu_access.level_z_num,
                                                           gpuaprAccess.gpu_access.level_y_num,
                                                           level);


                } else {

                    down_sample_avg_interior<< < blocks_l, threads_l >> >
                                                           (gpuaprAccess.gpu_access.row_global_index,
                                                                   gpuaprAccess.gpu_access.y_part_coord,
                                                                   gpuaprAccess.gpu_access.level_offsets,
                                                                   apr.particles_intensities.gpu_pointer,
                                                                   gpuaprAccessTree.gpu_access.row_global_index,
                                                                   gpuaprAccessTree.gpu_access.y_part_coord,
                                                                   gpuaprAccessTree.gpu_access.level_offsets,
                                                                   tree_mean_gpu.gpu_pointer,
                                                                   gpuaprAccess.gpu_access.level_x_num,
                                                                   gpuaprAccess.gpu_access.level_z_num,
                                                                   gpuaprAccess.gpu_access.level_y_num,
                                                                   level);
                }
                cudaDeviceSynchronize();
            }
        }

        timer.stop_timer();
    }

    float gpu_iterate_time_batch = timer.timings.back();
    std::cout << "Average time NEWt for loop insert max: " << (gpu_iterate_time_batch/(number_reps*1.0f))*1000 << " ms" << std::endl;
    std::cout << "Average time NEWt for loop insert max per million: " << (gpu_iterate_time_batch/(number_reps*1.0f*apr.total_number_particles()))*1000.0*1000000.0f << " ms" << std::endl;

    //copy data back from gpu
    tree_mean_gpu.copy_data_to_host();

    tree_mean_gpu.gpu_data.clear();
    tree_mean_gpu.gpu_data.shrink_to_fit();



    cudaDeviceSynchronize();

    ExtraParticleData<float> dummy(apr);
    dummy.init_gpu(apr.total_number_particles());


    for (int i = 0; i < 2; ++i) {

        timer.start_timer("summing the sptial informatino for each partilce on the GPU");
        for (int rep = 0; rep < number_reps; ++rep) {

            for (int level = apr.level_max(); level >= aprIt.level_min(); --level) {

                std::size_t number_rows_l = apr.spatial_index_x_max(level) * apr.spatial_index_z_max(level);
                std::size_t offset = gpuaprAccess.h_level_offset[level];

                std::size_t x_num = apr.spatial_index_x_max(level);
                std::size_t z_num = apr.spatial_index_z_max(level);
                std::size_t y_num = apr.spatial_index_y_max(level);

                dim3 threads_l(128, 1, 1);

                int x_blocks = (x_num + 2 - 1) / 2;
                int z_blocks = (z_num + 2 - 1) / 2;

                dim3 blocks_l(x_blocks, 1, z_blocks);


                down_sample_y_update << < blocks_l, threads_l >> >
                                               (gpuaprAccess.gpu_access.row_global_index,
                                                       gpuaprAccess.gpu_access.y_part_coord,
                                                       gpuaprAccess.gpu_access.level_offsets,
                                                       apr.particles_intensities.gpu_pointer,
                                                       gpuaprAccessTree.gpu_access.row_global_index,
                                                       gpuaprAccessTree.gpu_access.y_part_coord,
                                                       gpuaprAccessTree.gpu_access.level_offsets,
                                                       dummy.gpu_pointer,
                                                       gpuaprAccess.gpu_access.level_x_num,
                                                       gpuaprAccess.gpu_access.level_z_num,
                                                       gpuaprAccess.gpu_access.level_y_num,
                                                       level);


                cudaDeviceSynchronize();
            }
        }

        timer.stop_timer();
    }

    float gpu_iterate_time_batcht = timer.timings.back();
    std::cout << "Average time NEW for loop insert max: " << (gpu_iterate_time_batcht/(number_reps*1.0f))*1000 << " ms" << std::endl;
    std::cout << "Average time NEW for loop insert max per million: " << (gpu_iterate_time_batcht/(number_reps*1.0f*apr.total_number_particles()))*1000.0*1000000.0f << " ms" << std::endl;


    //copy data back from gpu
    dummy.copy_data_to_host();

    dummy.gpu_data.clear();
    dummy.gpu_data.shrink_to_fit();


    //////////////////////////
    ///
    /// Now check the data
    ///
    ////////////////////////////

    c_pass = 0;
    c_fail = 0;
    success=true;
    uint64_t output_c=0;

    for (uint64_t particle_number = 0; particle_number < aprTree.total_number_parent_cells(); ++particle_number) {
        //This step is required for all loops to set the iterator by the particle number
        treeIt.set_iterator_to_particle_by_number(particle_number);

        //if(tree_mean_gpu[treeIt]==8){
        //if(tree_mean_gpu[treeIt]==treeIt.y()){
        if(abs(tree_mean_gpu[treeIt]-ds_parts[treeIt])<0.1){
        //if(tree_mean_gpu[treeIt]==2*number_reps){
            c_pass++;
        } else {
            //c_fail++;

            if(treeIt.level() == (treeIt.level_max())) {
                if (output_c < 80) {
                    std::cout << "Expected: " << ds_parts[treeIt] << " Recieved: " << tree_mean_gpu[treeIt] << " Level: " << treeIt.level() << " x: " << treeIt.x()
                              << " z: " << treeIt.z() << " y: " << treeIt.y() << " global index: " << (int) treeIt.global_index() << std::endl;
                    output_c++;

                }
                c_fail++;
                success = false;
            }

        }
    }

    if(success){
        std::cout << "Direct ds, PASS" << std::endl;
    } else {
        std::cout << "Direct ds Check, FAIL Total: " << c_fail << " Pass Total:  " << c_pass << std::endl;
    }

    c_pass = 0;
    c_fail = 0;
    success=true;
    output_c=0;


    for (uint64_t particle_number = 0; particle_number < apr.total_number_particles(); ++particle_number) {
        //This step is required for all loops to set the iterator by the particle number
        aprIt.set_iterator_to_particle_by_number(particle_number);
        //if(spatial_info_test[aprIt]==(aprIt.x() + aprIt.y() + aprIt.z() + aprIt.level())){
        if(dummy[aprIt]==apr.particles_intensities[aprIt]){
            c_pass++;
        } else {
            c_fail++;
            success = false;
            if(aprIt.level() >= aprIt.level_min()) {
                if (output_c < 5) {
                    std::cout << "Expected: " << apr.particles_intensities[aprIt] << " Recieved: " << dummy[aprIt] << " Level: " << aprIt.level() << " x: " << aprIt.x()
                              << " z: " << aprIt.z() << " y: " << aprIt.y() << std::endl;
                    output_c++;
                }
                //spatial_info_test3[aprIt] = 0;
            }

        }
    }


    if(success){
        std::cout << "Direct insert, PASS " << c_pass << std::endl;
    } else {
        std::cout << "Direct insert Check, FAIL Total: " << c_fail << " Pass Total:  " << c_pass << std::endl;
    }



}




__device__ void get_row_begin_end(std::size_t* index_begin,
                                  std::size_t* index_end,
                                  std::size_t current_row,
                                  const std::size_t *row_info){

    *index_end = (row_info[current_row]);

    if (current_row == 0) {
        *index_begin = 0;
    } else {
        *index_begin =(row_info[current_row-1]);
    }


};
__global__ void down_sample_y_update(const std::size_t *row_info,
                                const std::uint16_t *particle_y,
                                const std::size_t* level_offset,
                                const std::uint16_t *particle_data_input,
                                const std::size_t *row_info_child,
                                const std::uint16_t *particle_y_child,
                                const std::size_t* level_offset_child,
                                std::float_t *particle_data_output,
                                const std::uint16_t* level_x_num,
                                const std::uint16_t* level_z_num,
                                const std::uint16_t* level_y_num,
                                const std::size_t level) {

    const int x_num = level_x_num[level];
    const int y_num = level_y_num[level];
    const int z_num = level_z_num[level];

    const int x_num_p = level_x_num[level-1];
    const int y_num_p = level_y_num[level-1];
    const int z_num_p = level_z_num[level-1];

    int x_index = (2 * blockIdx.x + threadIdx.x/64);
    int z_index = (2 * blockIdx.z + ((threadIdx.x+31)/32)%2);

    int block = threadIdx.x/32;
    int local_th = (threadIdx.x%32);

    if(x_index >= x_num ){

        return; //out of bounds
    }

    if(z_index >= z_num){

        return; //out of bounds
    }

    std::size_t row_index =x_index + z_index*x_num + level_offset[level];

    std::size_t global_index_begin_0;
    std::size_t global_index_end_0;

    __shared__ std::float_t f_cache[4][32];
    __shared__ int y_cache[4][32];

    //initialization to zero
    f_cache[block][local_th]=0;
    y_cache[block][local_th]=0;


    uint16_t current_y=0;
    //ying printf("hello begin %d end %d chunks %d number parts %d \n",(int) global_index_begin_0,(int) global_index_end_f, (int) number_chunk, (int) number_parts);


    get_row_begin_end(&global_index_begin_0, &global_index_end_0, row_index, row_info);
    std::size_t number_parts = global_index_end_0 - global_index_begin_0;
    std::uint16_t number_chunk = ((number_parts+31)/32);

    std::uint16_t number_y_chunk = (y_num+31)/32;

    //initialize (i=0)
    if (global_index_begin_0 + local_th < global_index_end_0) {
        f_cache[block][local_th] = particle_data_input[global_index_begin_0 + local_th];
    }

    if ( global_index_begin_0 + local_th < global_index_end_0) {
        y_cache[block][local_th] = particle_y[ global_index_begin_0 + local_th];
    }

    current_y = y_cache[block][local_th ];

    uint16_t sparse_block = 0;

    for (int y_block = 0; y_block < (number_y_chunk); ++y_block) {

        __syncthreads();
        //value less then current chunk then update.
        if (current_y < y_block * 32) {
            sparse_block++;
            if (sparse_block * 32 + global_index_begin_0 + local_th < global_index_end_0) {
                f_cache[block][local_th] = particle_data_input[sparse_block * 32 + global_index_begin_0 +
                        local_th];
            }

            if (sparse_block * 32 + global_index_begin_0 + local_th < global_index_end_0) {
                y_cache[block][local_th] = particle_y[sparse_block * 32 + global_index_begin_0 + local_th];
            }

        }

        current_y = y_cache[block][local_th];

        //do something
        if (current_y < (y_block + 1) * 32) {


        }

        __syncthreads();
        //output
        if (current_y < (y_block + 1) * 32) {
            if (sparse_block * 32 + global_index_begin_0 + local_th < global_index_end_0) {

                particle_data_output[sparse_block * 32 + global_index_begin_0 +
                        local_th] = f_cache[block][local_th];

            }
        }
    }







}


__global__ void down_sample_avg(const std::size_t *row_info,
                                const std::uint16_t *particle_y,
                                const std::size_t* level_offset,
                                const std::uint16_t *particle_data_input,
                                const std::size_t *row_info_child,
                                const std::uint16_t *particle_y_child,
                                const std::size_t* level_offset_child,
                                std::float_t *particle_data_output,
                                const std::uint16_t* level_x_num,
                                const std::uint16_t* level_z_num,
                                const std::uint16_t* level_y_num,
                                const std::size_t level) {

    const int x_num = level_x_num[level];
    const int y_num = level_y_num[level];
    const int z_num = level_z_num[level];

    const int x_num_p = level_x_num[level-1];
    const int y_num_p = level_y_num[level-1];
    const int z_num_p = level_z_num[level-1];

    int x_index = (2 * blockIdx.x + threadIdx.x/64);
    int z_index = (2 * blockIdx.z + ((threadIdx.x+31)/32)%2);

    int x_index_p = blockIdx.x;
    int z_index_p = blockIdx.z;

    int block = threadIdx.x/32;
    int local_th = (threadIdx.x%32);


    std::size_t row_index_p =x_index_p + z_index_p*x_num_p + level_offset_child[level-1];


    std::size_t row_index =x_index + z_index*x_num + level_offset[level];

    std::size_t global_index_begin_0;
    std::size_t global_index_end_0;

    std::size_t global_index_begin_p;
    std::size_t global_index_end_p;


    __shared__ float f_cache[5][32];
    __shared__ int y_cache[5][32];

    __shared__ float parent_cache[8][16];

    //initialization to zero
    f_cache[block][local_th]=0;
    y_cache[block][local_th]=0;

    if(block==0){
        f_cache[4][local_th]=0;
        y_cache[4][local_th]=0;
        parent_cache[0][local_th/2]=1;
    }

    int current_y=0;
    int current_y_p=0;
    //ying printf("hello begin %d end %d chunks %d number parts %d \n",(int) global_index_begin_0,(int) global_index_end_f, (int) number_chunk, (int) number_parts);


    if((x_index >= x_num) || (z_index >= z_num) ){

        global_index_begin_0 = 1;
        global_index_end_0 = 0;

       // return; //out of bounds
    } else {
        get_row_begin_end(&global_index_begin_0, &global_index_end_0, row_index, row_info);


    }

    get_row_begin_end(&global_index_begin_p, &global_index_end_p, row_index_p, row_info_child);



    std::size_t number_parts = global_index_end_0 - global_index_begin_0;
    std::uint16_t number_chunk = ((number_parts+31)/32);

    std::uint16_t number_y_chunk = (y_num+31)/32;


    //initialize (i=0)
    if (global_index_begin_0 + local_th < global_index_end_0) {
        f_cache[block][local_th] = particle_data_input[global_index_begin_0 + local_th];
    }

    if ( global_index_begin_0 + local_th < global_index_end_0) {
        y_cache[block][local_th] = particle_y[ global_index_begin_0 + local_th];
    }


    if (block==2){

        if ((  global_index_begin_p + local_th) < global_index_end_p) {
            f_cache[4][local_th] = particle_data_output[ global_index_begin_p + local_th];

        }

    } else if (block == 3) {

        if (( global_index_begin_p + local_th) < global_index_end_p) {

            y_cache[4][local_th] = particle_y_child[ global_index_begin_p + local_th];

        }

    }

    current_y = y_cache[block][local_th ];
    current_y_p = y_cache[4][local_th ];


    uint16_t sparse_block = 0;
    int sparse_block_p =0;

    for (int y_block = 0; y_block < (number_y_chunk); ++y_block) {

        //__syncthreads();
        //value less then current chunk then update.
        if (current_y < y_block * 32) {
            sparse_block++;
            if (sparse_block * 32 + global_index_begin_0 + local_th < global_index_end_0) {
                f_cache[block][local_th] = particle_data_input[sparse_block * 32 + global_index_begin_0 +
                                                               local_th];

                y_cache[block][local_th] = particle_y[sparse_block * 32 + global_index_begin_0 + local_th];
            }

        }

        current_y = y_cache[block][local_th];
        __syncthreads();

//        if(block < 2){
//            //block 0 or 1
//            uint16_t local_y =  y_cache[2*block][local_th];
//
//            if(local_th%2==0) {
//                //this here needs to be dealt with..
//                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {
//
//                    local_y = (local_y/2);
//
//                    parent_cache[block][(local_y) % 16] = (1.0/8.0f)*f_cache[2*block][local_th];
//
//
//                }
//
//                //next threads work
//                local_y =  y_cache[2*block][local_th+1];
//
//                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {
//
//                    local_y = (local_y/2);
//
//                    parent_cache[block][(local_y) % 16] += (1.0/8.0f)*f_cache[2*block][local_th+1];
//
//
//                }
//
//                //next threads work
//                local_y =  y_cache[2*block+1][local_th];
//
//                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {
//
//                    local_y = (local_y/2);
//
//                    parent_cache[block][(local_y) % 16] += (1.0/8.0f)*f_cache[2*block+1][local_th];
//
//                }
//
//                //next threads work
//                local_y =  y_cache[2*block+1][local_th+1];
//
//                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {
//
//                    local_y = (local_y/2);
//
//                    parent_cache[block][(local_y) % 16] += (1.0/8.0f)*f_cache[2*block+1][local_th+1];
//
//                }
//            }
//
//
//        }


        if ((current_y < (y_block + 1) * 32) && (current_y >= (y_block) * 32)) {

            parent_cache[2*block+local_th%2][(current_y/2) % 16] = (1.0/8.0f)*f_cache[block][local_th];

        }




        if (block==2){

            if (current_y_p < ((y_block * 32)/2)) {
                sparse_block_p++;

                if ((sparse_block_p * 32 + global_index_begin_p + local_th) < global_index_end_p) {
                    f_cache[4][local_th] = particle_data_output[sparse_block_p * 32 + global_index_begin_p + local_th];

                }


            }
        } else if (block == 3) {
            if (current_y_p < ((y_block * 32)/2)) {
                sparse_block_p++;


                if ((sparse_block_p * 32 + global_index_begin_p + local_th) < global_index_end_p) {

                    y_cache[4][local_th] = particle_y_child[sparse_block_p * 32 + global_index_begin_p + local_th];

                }

            }


        }
        __syncthreads();
        current_y_p = y_cache[4][local_th];

        if(block ==3) {
            //output

            if (current_y_p < ((y_block+1) * 32)/2) {
                if (sparse_block_p * 32 + global_index_begin_p + local_th < global_index_end_p) {

                    particle_data_output[sparse_block_p * 32 + global_index_begin_p + local_th] = parent_cache[0][current_y_p%16] + parent_cache[1][current_y_p%16] +  parent_cache[2][current_y_p%16]
                                                                                                  + parent_cache[3][current_y_p%16]  + parent_cache[4][current_y_p%16] + parent_cache[5][current_y_p%16] + parent_cache[6][current_y_p%16] + parent_cache[7][current_y_p%16];

                }
            }

        }
    }



}

__global__ void down_sample_avg_interior(const std::size_t *row_info,
                                const std::uint16_t *particle_y,
                                const std::size_t* level_offset,
                                const std::uint16_t *particle_data_input,
                                const std::size_t *row_info_child,
                                const std::uint16_t *particle_y_child,
                                const std::size_t* level_offset_child,
                                std::float_t *particle_data_output,
                                const std::uint16_t* level_x_num,
                                const std::uint16_t* level_z_num,
                                const std::uint16_t* level_y_num,
                                const std::size_t level) {
    //
    //  This step is required for the interior down-sampling
    //

    const int x_num = level_x_num[level];
    const int y_num = level_y_num[level];
    const int z_num = level_z_num[level];

    const int x_num_p = level_x_num[level-1];
    const int y_num_p = level_y_num[level-1];
    const int z_num_p = level_z_num[level-1];

    int x_index = (2 * blockIdx.x + threadIdx.x/64);
    int z_index = (2 * blockIdx.z + ((threadIdx.x+31)/32)%2);

    int x_index_p = blockIdx.x;
    int z_index_p = blockIdx.z;

    int block = threadIdx.x/32;
    int local_th = (threadIdx.x%32);

    std::size_t row_index_p =x_index_p + z_index_p*x_num_p + level_offset_child[level-1];

    std::size_t row_index =x_index + z_index*x_num + level_offset[level];

    //Particles
    std::size_t global_index_begin_0;
    std::size_t global_index_end_0;

    //Parent Tree Particle Cells
    std::size_t global_index_begin_p;
    std::size_t global_index_end_p;

    //Interior Tree Particle Cells
    std::size_t global_index_begin_t;
    std::size_t global_index_end_t;

    //shared memory caches
    __shared__ float f_cache[5][32];
    __shared__ int y_cache[5][32];

    __shared__ float f_cache_t[4][32];
    __shared__ int y_cache_t[4][32];

    __shared__ float parent_cache[8][16];

    //initialization to zero
    f_cache[block][local_th]=0;
    y_cache[block][local_th]=0;

    if(block==0){
        f_cache[4][local_th]=0;
        y_cache[4][local_th]=0;
        parent_cache[0][local_th/2]=1;
    }

    int current_y=0;
    int current_y_p=0;
    int current_y_t=0;

    if((x_index >= x_num) || (z_index >= z_num) ){

        global_index_begin_0 = 1;
        global_index_end_0 = 0;

        global_index_begin_t = 1;
        global_index_end_t = 0;

        // return; //out of bounds
    } else {
        get_row_begin_end(&global_index_begin_t, &global_index_end_t, row_index, row_info_child);
        get_row_begin_end(&global_index_begin_0, &global_index_end_0, row_index, row_info);

    }


    std::uint16_t number_y_chunk = (y_num+31)/32;


    //initialize (i=0)
    if (global_index_begin_0 + local_th < global_index_end_0) {

        y_cache[block][local_th] = particle_y[global_index_begin_0 + local_th];

        f_cache[block][local_th] = particle_data_input[global_index_begin_0 + local_th];

    }

    //tree interior
    if (global_index_begin_t + local_th < global_index_end_t) {

        y_cache_t[block][local_th] = particle_y_child[global_index_begin_t + local_th];

        f_cache_t[block][local_th] = particle_data_output[global_index_begin_t + local_th];

    }


    if (block==2){

        if ((  global_index_begin_p + local_th) < global_index_end_p) {
            f_cache[4][local_th] = particle_data_output[ global_index_begin_p + local_th];
        }

    } else if (block == 3) {

        if (( global_index_begin_p + local_th) < global_index_end_p) {

            y_cache[4][local_th] = particle_y_child[ global_index_begin_p + local_th];

        }

    }

    current_y = y_cache[block][local_th ];
    current_y_p = y_cache[4][local_th ];
    current_y_t = y_cache_t[block][local_th ];

    uint16_t sparse_block = 0;
    int sparse_block_p =0;
    int sparse_block_t =0;

    for (int y_block = 0; y_block < (number_y_chunk); ++y_block) {

        //__syncthreads();
        //value less then current chunk then update.
        if (current_y < y_block * 32) {
            sparse_block++;
            if (sparse_block * 32 + global_index_begin_0 + local_th < global_index_end_0) {
                f_cache[block][local_th] = particle_data_input[sparse_block * 32 + global_index_begin_0 +
                                                               local_th];

                y_cache[block][local_th] = particle_y[sparse_block * 32 + global_index_begin_0 + local_th];
            }

        }
        current_y = y_cache[block][local_th];

        //interior tree update
        if (current_y_t < y_block * 32) {
            sparse_block_t++;
            if (sparse_block_t * 32 + global_index_begin_t + local_th < global_index_end_t) {
                f_cache_t[block][local_th] = particle_data_output[sparse_block_t * 32 + global_index_begin_t +
                                                               local_th];

                y_cache_t[block][local_th] = particle_y_child[sparse_block_t * 32 + global_index_begin_t + local_th];
            }

        }
        current_y_t = y_cache_t[block][local_th];


        __syncthreads();

        if(block < 2){
            //block 0 or 1
            uint16_t local_y =  y_cache[2*block][local_th];

            if(local_th%2==0) {
                //this here needs to be dealt with..
                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {

                    local_y = (local_y/2);

                    parent_cache[block][(local_y) % 16] = (1.0/8.0f)*f_cache[2*block][local_th];

                }

                //next threads work
                local_y =  y_cache[2*block][local_th+1];

                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {

                    local_y = (local_y/2);

                    parent_cache[block][(local_y) % 16] += (1.0/8.0f)*f_cache[2*block][local_th+1];


                }

                //next threads work
                local_y =  y_cache[2*block+1][local_th];

                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {

                    local_y = (local_y/2);

                    parent_cache[block][(local_y) % 16] += (1.0/8.0f)*f_cache[2*block+1][local_th];

                }

                //next threads work
                local_y =  y_cache[2*block+1][local_th+1];

                if ((local_y < (y_block + 1) * 32) && (local_y >= (y_block) * 32)) {

                    local_y = (local_y/2);

                    parent_cache[block][(local_y) % 16] += (1.0/8.0f)*f_cache[2*block+1][local_th+1];

                }
            }



        } else if (block==2){

            if (current_y_p < ((y_block * 32)/2)) {
                sparse_block_p++;

                if ((sparse_block_p * 32 + global_index_begin_p + local_th) < global_index_end_p) {
                    f_cache[4][local_th] = particle_data_output[sparse_block_p * 32 + global_index_begin_p + local_th];

                }


            }
        } else if (block == 3) {
            if (current_y_p < ((y_block * 32)/2)) {
                sparse_block_p++;


                if ((sparse_block_p * 32 + global_index_begin_p + local_th) < global_index_end_p) {

                    y_cache[4][local_th] = particle_y_child[sparse_block_p * 32 + global_index_begin_p + local_th];

                }

            }


        }
        __syncthreads();
        current_y_p = y_cache[4][local_th];

        if(block ==3) {
            //output

            if (current_y_p < ((y_block+1) * 32)/2) {
                if (sparse_block_p * 32 + global_index_begin_p + local_th < global_index_end_p) {

                    particle_data_output[sparse_block_p * 32 + global_index_begin_p + local_th] = parent_cache[0][current_y_p%16] + parent_cache[1][current_y_p%16];

                    parent_cache[0][current_y_p%16]=0;
                    parent_cache[1][current_y_p%16]=0;
                }
            }

        }
    }



}



