//
// Created by cheesema on 08.01.18.
//

#ifndef PARTPLAY_LOCAL_PARTICLE_SET_HPP
#define PARTPLAY_LOCAL_PARTICLE_SET_HPP

#define EMPTY 0

/*
 * Declerations
 */

template< typename T>
void compute_level_for_array(Mesh_data<T>& input,float k_factor,float rel_error);


static inline uint32_t asmlog_2(const uint32_t x){
    if(x == 0) return 0;
    return (31 - __builtin_clz (x));
}

/*
 * Definitions
 */

template< typename T>
void compute_level_for_array(Mesh_data<T>& input,float k_factor,float rel_error){
    //
    //  Takes the sqrt of the grad vector to caluclate the magnitude
    //
    //  Bevan Cheeseman 2016
    //

    float mult_const = k_factor/rel_error;

    const int z_num = input.z_num;
    const int x_num = input.x_num;
    const int y_num = input.y_num;

    int i,k;

#pragma omp parallel for default(shared) private (i,k) if(z_num*x_num*y_num > 100000)
    for(int j = 0;j < z_num;j++){

        for(i = 0;i < x_num;i++){

#pragma omp simd
            for (k = 0; k < (y_num);k++){
                input.mesh[j*x_num*y_num + i*y_num + k] = asmlog_2(input.mesh[j*x_num*y_num + i*y_num + k]*mult_const);
            }

        }
    }

}

template<typename T>
void initialize_particle_cell_tree(std::vector<Mesh_data<uint8_t>>& particle_cell_tree,APR<T>& apr)
{   //
    //  Initializes the particle cell tree structure
    //
    //  Contains pc up to l_max - 1,
    //

    unsigned int l_max = apr.depth_max() - 1;
    unsigned int l_min = apr.depth_min();
    //make so you can reference the array as l
    particle_cell_tree.resize(l_max + 1);

    for(int l = l_min; l < (l_max + 1) ;l ++){
        particle_cell_tree[l].initialize(ceil(1.0*apr.pc_data.org_dims[0]/pow(2.0,1.0*l_max - l + 1)),
                              ceil(1.0*apr.pc_data.org_dims[1]/pow(2.0,1.0*l_max - l + 1)),
                              ceil(1.0*apr.pc_data.org_dims[2]/pow(2.0,1.0*l_max - l + 1)), EMPTY);
    }
}



#endif //PARTPLAY_LOCAL_PARTICLE_SET_HPP
