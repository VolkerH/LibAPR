////////////////////////
//
//  Mateusz Susik 2016
//
//  Utility functions for the tests
//
////////////////////////

#ifndef PARTPLAY_UTILS_H
#define PARTPLAY_UTILS_H

#define K_BENCHMARK_REL_ERROR 1000

#include <cmath>
#include <random>

#include "tiffio.h"

#include "../src/data_structures/meshclass.h"
#include "../src/algorithm/level.hpp"

bool compare_two_images(const Mesh_data<uint16_t>& in_memory, std::string filename);
bool compare_two_ks(const Particle_map<float>& in_memory, std::string filename);
bool compare_part_rep_with_particle_map(const Particle_map<float>& in_memory, std::string filename);

Mesh_data<uint16_t> create_random_test_example(unsigned int size_y, unsigned int size_x,
                                               unsigned int size_z, unsigned int seed);


Mesh_data<uint16_t> generate_random_ktest_example(unsigned int size_y, unsigned int size_x,
                                                  unsigned int size_z, unsigned int seed,
                                                  float mean_fraction, float sd_fraction);

uint16_t get_random_number(std::ranlux48& generator, std::normal_distribution<float>& distribution);
uint16_t get_random_number_k(std::ranlux48& generator,
                             std::normal_distribution<float>& distribution, float k_max);

std::string get_source_directory();

#endif //PARTPLAY_UTILS_H