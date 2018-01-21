//
// Created by cheesema on 23/01/17.
//

#ifndef PARTPLAY_APR_ANALYSIS_H
#define PARTPLAY_APR_ANALYSIS_H

#include "benchmarks/development/old_algorithm/apr_pipeline.hpp"
#include "AnalysisData.hpp"
#include "MeshDataAF.h"
#include "benchmarks/development/old_io/parameters.h"
#include "SynImageClasses.hpp"
#include "numerics_benchmarks.hpp"
#include "benchmarks/development/old_numerics/misc_numerics.hpp"
#include "../../src/data_structures/APR/APR.hpp"
#include <assert.h>

#include "test/utils.h"

void calc_information_content(SynImage syn_image,AnalysisData& analysis_data);
void calc_information_content_new(SynImage syn_image,AnalysisData& analysis_data);

template<typename S>
void copy_mesh_data_structures(MeshDataAF<S>& input_syn,MeshData<S>& input_img){
    //copy across metadata
    input_img.y_num = input_syn.y_num;
    input_img.x_num = input_syn.x_num;
    input_img.z_num = input_syn.z_num;

    std::swap(input_img.mesh,input_syn.mesh);

}

void get_apr_ground_thruth(SynImage& syn_image,Part_rep& part_rep,PartCellStructure<float,uint64_t>& pc_struct,AnalysisData& analysis_data){
    //
    //  Bevan Cheeseman 2017
    //
    //  Calculates Ground Truth APR, using synthetic image generator
    //
    //

//    int interp_type = part_rep.pars.interp_type;
//
//    // COMPUTATIONS
//
//    MeshData<float> input_image_float;
//    MeshData<float> gradient, variance;
//    MeshData<float> interp_img;
//
//    gradient.initialize(input_image.y_num, input_image.x_num, input_image.z_num, 0);
//    part_rep.initialize(input_image.y_num, input_image.x_num, input_image.z_num);
//
//    input_image_float = input_image.to_type<float>();
//    interp_img = input_image.to_type<float>();
//    // After this block, input_image will be freed.
//
//    Part_timer t;
//    t.verbose_flag = false;
//
//    // preallocate_memory
//    Particle_map<float> part_map(part_rep);
//    preallocate(part_map.layers, gradient.y_num, gradient.x_num, gradient.z_num, part_rep);
//    variance.preallocate(gradient.y_num, gradient.x_num, gradient.z_num, 0);
//    std::vector<MeshData<float>> down_sampled_images;
//
//    MeshData<float> temp;
//    temp.preallocate(gradient.y_num, gradient.x_num, gradient.z_num, 0);
//
//
//    generate_gt_image(MeshData<T>& gt_image,SynImage& syn_image)
//
//
//    t.start_timer("whole");
//
//
//    part_rep.timer.start_timer("get_level_3D");
//    get_level_3D(variance, gradient, part_rep, part_map, temp);
//    part_rep.timer.stop_timer();
//
//    // free memory (not used anymore)
//    std::vector<float>().swap(gradient.mesh);
//    std::vector<float>().swap(variance.mesh);
//
//    part_rep.timer.start_timer("pushing_scheme");
//    part_map.pushing_scheme(part_rep);
//    part_rep.timer.stop_timer();
//
//
//    part_rep.timer.start_timer("sample");
//
//    if (interp_type == 0) {
//        part_map.downsample(interp_img);
//        calc_median_filter(part_map.downsampled[part_map.k_max+1]);
//    }
//
//    if (interp_type == 1) {
//        part_map.downsample(input_image_float);
//    } else if (interp_type == 2) {
//        part_map.downsample(interp_img);
//    } else if (interp_type ==3){
//        part_map.downsample(interp_img);
//        calc_median_filter_n(part_map.downsampled[part_map.k_max+1],input_image_float);
//    }
//
//    part_rep.timer.stop_timer();
//
//    part_rep.timer.start_timer("construct_pcstruct");
//
//    pc_struct.initialize_structure(part_map);
//
//    part_rep.timer.stop_timer();
//
//    t.stop_timer();



}




template <typename T>
void bench_get_apr(MeshData<T>& input_image,Part_rep& p_rep,PartCellStructure<float,uint64_t>& pc_struct,AnalysisData& analysis_data){
    //
    //
    //  Calculates the APR from image
    //
    //


    std::vector<float> lambda_sch = {0.1,0.25,0.5000,0.7500,1.0000,1.5000,2.0000,3.0000,4.0000,5.0000,10.0000,15.0000,20.0000,30.0000,50.0000,100.0000};

    std::vector<std::vector<float>> scaling_var_th ={{0.99139,1.0009,0.99767,1.0425,1.0136,1.0349},{0.92411,0.98263,1.0262,1.0338,1.002,0.98295},{0.9218,0.96032,1.0173,1.0441,0.97428,0.97304},{0.92265,0.95773,0.99296,1.0266,1.0477,0.96948},{0.87994,0.91564,1.052,1.0682,0.99079,0.98891},{0.84401,0.93945,0.98487,1.0085,1.0116,0.97628},{0.78323,0.91706,0.96895,0.99856,0.98813,0.95153},{0.74016,0.88093,1.0083,0.98473,1.0043,0.91803},{0.71953,0.85958,0.96862,0.99867,0.98421,0.97756},{0.69742,0.84674,0.99477,0.97982,1.0493,0.96122},{0.60312,0.78776,0.95586,0.96647,0.9883,0.9427},{ 0.55227,0.73624,0.88747,0.94319,0.9723,0.94098},{0.53228,0.72638,0.84264,0.90814,0.94163,0.92053},{0.46973,0.66264,0.83219,0.88602,0.90963,0.88453},{0.4265,0.61347,0.75971,0.8483,0.85824,0.90162},{0.32835,0.52536,0.65846,0.77149,0.79807,0.80074}};

    p_rep.pars.tol = 0.0005f;

    int psf_ind = std::max(((float) (round(p_rep.pars.psfx/p_rep.pars.dx) - 1)),((float)0.0f));

    psf_ind = std::min(psf_ind,5);

    //p_rep.pars.var_scale = 2;

    float k_diff = -3.0f;

    //set lambda

    if(p_rep.pars.lambda == 0) {

        float lambda = expf((-1.0f/0.6161f) * logf((p_rep.pars.var_th/p_rep.pars.noise_sigma) *
                                                   powf(2.0f,k_diff + log2f(p_rep.pars.rel_error))/0.12531f));
        std::cout << lambda << std::endl;

        p_rep.pars.lambda = expf((-1.0f/0.6161f) * logf((p_rep.pars.var_th/p_rep.pars.noise_sigma) *
                                                        powf(2.0f,k_diff + log2f(.05))/0.12531f));

        std::cout << p_rep.pars.lambda << std::endl;

        float lambda_min = 0.05f;
        float lambda_max = 5000;

        p_rep.pars.lambda = std::max(lambda_min,p_rep.pars.lambda);
        p_rep.pars.lambda = std::min(p_rep.pars.lambda,lambda_max);

        float max_var_th = 1.2f * p_rep.pars.noise_sigma * expf(-0.5138f * logf(p_rep.pars.lambda)) *
                           (0.1821f * logf(p_rep.pars.lambda) + 1.522f);
        if (max_var_th > .25*p_rep.pars.var_th){
            float desired_th = 0.1*p_rep.pars.var_th;
            p_rep.pars.lambda = std::max((float)exp((-1.0/0.5138)*log(desired_th/p_rep.pars.noise_sigma)),p_rep.pars.lambda);
            p_rep.pars.var_th_max = .25*p_rep.pars.var_th;

        } else {
            p_rep.pars.var_th_max = .5*p_rep.pars.var_th;

        }
    } else {
        p_rep.pars.var_th_max =  .25*p_rep.pars.var_th;
    }

    if(p_rep.pars.lambda == -1){
        p_rep.pars.lambda = 0;
        p_rep.pars.var_th_max = 1;
    }


    float scales_down = 0;
    int index_scale = 0;

    for (int i = 0; i < lambda_sch.size(); ++i) {
        if(p_rep.pars.lambda > lambda_sch[i]){
            index_scale = i;
        }
    }


    p_rep.pars.var_th = scaling_var_th[index_scale][psf_ind]*p_rep.pars.var_th;
    p_rep.pars.var_th_max = scaling_var_th[index_scale][psf_ind]*p_rep.pars.var_th_max;

    std::cout << "Lamda: " << p_rep.pars.lambda << std::endl;

    get_apr(input_image,p_rep,pc_struct,analysis_data);

    p_rep.pars.mean_scale = scaling_var_th[index_scale][psf_ind];

}

template <typename T>
void bench_get_apr_part_time(MeshData<T>& input_image,Part_rep& p_rep,PartCellStructure<float,uint64_t>& pc_struct,AnalysisData& analysis_data){
    //
    //
    //  Calculates the APR from image
    //
    //

    p_rep.pars.tol = 0.0005f;

    p_rep.pars.var_scale = 2;

    float k_diff = -3.0f;

    //set lambda


    if(p_rep.pars.lambda == 0) {

        float lambda = expf((-1.0f/0.6161f) * logf((p_rep.pars.var_th/p_rep.pars.noise_sigma) *
                                                   powf(2.0f,k_diff + log2f(p_rep.pars.rel_error))/0.12531f));
        std::cout << lambda << std::endl;

        p_rep.pars.lambda = expf((-1.0f/0.6161f) * logf((p_rep.pars.var_th/p_rep.pars.noise_sigma) *
                                                        powf(2.0f,k_diff + log2f(.05))/0.12531f));

        std::cout << p_rep.pars.lambda << std::endl;

        float lambda_min = 0.05f;
        float lambda_max = 5000;

        p_rep.pars.lambda = std::max(lambda_min,p_rep.pars.lambda);
        p_rep.pars.lambda = std::min(p_rep.pars.lambda,lambda_max);

        float max_var_th = 1.2f * p_rep.pars.noise_sigma * expf(-0.5138f * logf(p_rep.pars.lambda)) *
                           (0.1821f * logf(p_rep.pars.lambda) + 1.522f);
        if (max_var_th > .25*p_rep.pars.var_th){
            float desired_th = 0.1*p_rep.pars.var_th;
            p_rep.pars.lambda = std::max((float)exp((-1.0/0.5138)*log(desired_th/p_rep.pars.noise_sigma)),p_rep.pars.lambda);
            p_rep.pars.var_th_max = .25*p_rep.pars.var_th;

        } else {
            p_rep.pars.var_th_max = .25*p_rep.pars.var_th;

        }
    } else {
        p_rep.pars.var_th_max =  .25*p_rep.pars.var_th;
    }

    if(p_rep.pars.lambda == -1){
        p_rep.pars.lambda = 0;
    }

    std::cout << "Lamda: " << p_rep.pars.lambda << std::endl;

    get_apr_part_timing(input_image,p_rep,pc_struct,analysis_data);


}

template<typename T>
void flip_lr(MeshData<T>& input) {
    //
    //  Bevan Cheeseman 2017: Flips LR
    //

    int y_num = input.y_num;
    int x_num = input.x_num;
    int z_num = input.z_num;


    MeshData<T> temp;
    temp.initialize(input.y_num, input.x_num, input.z_num, 0);

    for (int j = 0; j < temp.z_num; j++) {
        for (int i = 0; i < temp.x_num; i++) {

            for (int k = 0; k < temp.y_num; k++) {
                int k_lr = temp.y_num - 1 - k;
                temp.mesh[j * x_num * y_num + i * y_num + k] = input.mesh[j * x_num * y_num + i * y_num + k_lr];
            }
        }
    }



    std::swap(input.mesh, temp.mesh);

}

void test_local_scale(MeshData<uint16_t >& input_image,Part_rep& part_rep,PartCellStructure<float,uint64_t>& pc_struct,AnalysisData& analysis_data){

    int interp_type = part_rep.pars.interp_type;

    // COMPUTATIONS

    MeshData<float> input_image_float;
    MeshData<float> gradient, variance;
    MeshData<float> interp_img;

    gradient.initialize(input_image.y_num, input_image.x_num, input_image.z_num, 0);
    part_rep.initialize(input_image.y_num, input_image.x_num, input_image.z_num);

    input_image_float = input_image.to_type<float>();
    interp_img = input_image.to_type<float>();
    // After this block, input_image will be freed.

    Part_timer t;
    t.verbose_flag = false;

    // preallocate_memory
    Particle_map<float> part_map(part_rep);
    preallocate(part_map.layers, gradient.y_num, gradient.x_num, gradient.z_num, part_rep);
    variance.preallocate(gradient.y_num, gradient.x_num, gradient.z_num, 0);
    std::vector<MeshData<float>> down_sampled_images;

    MeshData<float> temp;
    temp.preallocate(gradient.y_num, gradient.x_num, gradient.z_num, 0);

    t.start_timer("whole");

    //    std::swap(part_map.downsampled[part_map.k_max+1],input_image_float);

    part_rep.timer.start_timer("get_gradient_3D");
    get_gradient_3D(part_rep, input_image_float, gradient);
    part_rep.timer.stop_timer();

    part_rep.timer.start_timer("get_variance_3D");
    get_variance_3D(part_rep, input_image_float, variance);
    part_rep.timer.stop_timer();






    down_sample(gradient,temp,
                [](float x, float y) { return std::max(x,y); },
                [](float x) { return x; });



    /////////////////////////////////////////////
    //
    //  Now we want to loop over and calculate the maximum in the area given by the apr
    //
    //////////////////////////////////////////////


    MeshData<float> local_max;
    local_max.initialize(temp.y_num, temp.x_num, temp.z_num, 0);

    MeshData<uint8_t> k_img;

    MeshData<uint8_t> k_img_ds;


    interp_depth_to_mesh(k_img,pc_struct);

    k_img_ds.preallocate(local_max.y_num, local_max.x_num, local_max.z_num, 0);

    down_sample(k_img,k_img_ds,
                [](uint8_t x, uint8_t y) { return std::max(x,y); },
                [](uint8_t x) { return x; },true);


    int a = 1;


    int x_num = temp.x_num;
    int y_num = temp.y_num;
    int z_num = temp.z_num;

    int k_max = part_rep.pl_map.k_max;





    //
    //
    //  FLIP LR BOTH OF THEM< SEE IF THE ERROR FLIPS SIDES
    //
    //
    //




    for(float j = 0; j < temp.z_num;j++){
        for(float i = 0; i < temp.x_num;i++){

            for(float k = 0;k < temp.y_num;k++){

                float curr_l = k_img_ds.mesh[j*x_num*y_num + i*y_num + k];

                int step_size = floor(pow(2,k_max - curr_l));


                int offset_max_y = std::min((int)(k + step_size),(int)(y_num-1));
                int offset_min_y = std::max((int)(k - step_size),(int)0);

                int offset_max_x = std::min((int)(i + step_size),(int)(x_num-1));
                int offset_min_x = std::max((int)(i - step_size),(int)0);

                int offset_max_z = std::min((int)(j + step_size),(int)(z_num-1));
                int offset_min_z = std::max((int)(j - step_size),(int)0);

                for(float a = offset_min_z;a <= offset_max_z;a++){
                    for(float b = offset_min_x;b <= offset_max_x;b++){
                        for(float c = offset_min_y;c <= offset_max_y;c++){

                            float dist = sqrt(pow(round(a-j),2.0)+ pow(round(b-i),2.0)+ pow(round(c-k),2.0));
                            //float dist = sqrt(pow(a-j,2)+ pow(b-i,2)+ pow(c-k,2));


                            if (dist <= step_size) {

                                local_max.mesh[j * x_num * y_num + i * y_num + k] = std::max(
                                        local_max.mesh[j * x_num * y_num + i * y_num + k],
                                        temp.mesh[a * x_num * y_num + b * y_num + c]);
                            }

                        }
                    }
                }

            }
        }
    }




#pragma omp parallel for default(shared)
    for(int i = 0; i < temp.mesh.size(); i++)
    {
        local_max.mesh[i] /= variance.mesh[i];
        temp.mesh[i] /= variance.mesh[i];
    }


    float k_factor;

    float min_dim = std::min(part_rep.pars.dy,std::min(part_rep.pars.dx,part_rep.pars.dz));

    k_factor = pow(2,part_rep.pl_map.k_max+1)*min_dim;

    compute_k_for_array(temp,k_factor,part_rep.pars.rel_error);

    compute_k_for_array(local_max,k_factor,part_rep.pars.rel_error);



    for(int i = 0; i < temp.mesh.size(); i++)
    {
        local_max.mesh[i] = std::min(local_max.mesh[i],(float)part_rep.pl_map.k_max);
        temp.mesh[i] = std::min(temp.mesh[i],(float)part_rep.pl_map.k_max);
    }




    MeshData<float> test_l;
    test_l.initialize(temp.y_num, temp.x_num, temp.z_num, 0);


    for(int j = 0; j < temp.z_num;j++){
        for(int i = 0; i < temp.x_num;i++){

            for(int k = 0;k < temp.y_num;k++){

                float curr_l = k_img_ds.mesh[j*x_num*y_num + i*y_num + k];

                float step_size = floor(pow(2,k_max - curr_l));

                int offset_max_y = std::min((int)(k + step_size),(int)(y_num-1));
                int offset_min_y = std::max((int)(k - step_size),(int)0);

                int offset_max_x = std::min((int)(i + step_size),(int)(x_num-1));
                int offset_min_x = std::max((int)(i - step_size),(int)0);

                int offset_max_z = std::min((int)(j + step_size),(int)(z_num-1));
                int offset_min_z = std::max((int)(j - step_size),(int)0);

                for(uint64_t a = offset_min_z;a <= offset_max_z;a++){
                    for(uint64_t b = offset_min_x;b <= offset_max_x;b++){
                        for(uint64_t c = offset_min_y;c <= offset_max_y;c++){

                            float dist = sqrt(pow(a-j,2)+ pow(b-i,2)+ pow(c-k,2));

                            if (dist <= step_size) {

                                test_l.mesh[j * x_num * y_num + i * y_num + k] = std::max(
                                        test_l.mesh[j * x_num * y_num + i * y_num + k],
                                        (float) temp.mesh[a * x_num * y_num + b * y_num + c]);
                            }
                        }
                    }
                }

            }
        }
    }






    MeshData<float> compare_org;
    compare_org.initialize(temp.y_num, temp.x_num, temp.z_num, 0);

    int counter_c = 0;

    int counter_d = 0;

    int counter_h = 0;

    for(int i = 0; i < compare_org.mesh.size(); i++)
    {

        compare_org.mesh[i] = std::max(local_max.mesh[i]-k_img_ds.mesh[i], (float) 0);

        if(compare_org.mesh[i] > 0){
            counter_c++;

            if(k_img_ds.mesh[i] > (k_max -2)){
                counter_h++;
            }

        }

        if(compare_org.mesh[i] > 1){
            counter_d++;

        }


    }

    std::cout << "max_c: " << counter_c << std::endl;

    std::cout << "max_c_d: " << counter_d << std::endl;

    std::cout << "max_c_h: " << counter_h << std::endl;

    analysis_data.add_float_data("max_c:",counter_c);
    analysis_data.add_float_data("max_c_d:",counter_h);
    analysis_data.add_float_data("max_c_h:",counter_d);

    //debug_write(compare_org,"compare");

//    MeshData<float> compare_l;
//    compare_l.initialize(temp.y_num, temp.x_num, temp.z_num, 0);
//
//    int counter_l = 0;
//
//    for(int i = 0; i < compare_l.mesh.size(); i++)
//    {
//
//        compare_l.mesh[i] = std::max(test_l.mesh[i]-k_img_ds.mesh[i], (float) 0);
//
//        if(compare_l.mesh[i] > 0){
//            counter_l++;
//        }
//
//    }
//
//
//    std::cout << "lc: " << counter_l << std::endl;

}
void cont_solution(MeshData<uint16_t >& input_image,Part_rep& part_rep,PartCellStructure<float,uint64_t>& pc_struct,AnalysisData& analysis_data){

    int interp_type = part_rep.pars.interp_type;

    // COMPUTATIONS

    MeshData<float> input_image_float;
    MeshData<float> gradient, variance;
    MeshData<float> interp_img;

    gradient.initialize(input_image.y_num, input_image.x_num, input_image.z_num, 0);
    part_rep.initialize(input_image.y_num, input_image.x_num, input_image.z_num);

    input_image_float = input_image.to_type<float>();
    interp_img = input_image.to_type<float>();
    // After this block, input_image will be freed.

    Part_timer t;
    t.verbose_flag = false;

    // preallocate_memory
    Particle_map<float> part_map(part_rep);
    preallocate(part_map.layers, gradient.y_num, gradient.x_num, gradient.z_num, part_rep);
    variance.preallocate(gradient.y_num, gradient.x_num, gradient.z_num, 0);
    std::vector<MeshData<float>> down_sampled_images;

    MeshData<float> temp;
    temp.preallocate(gradient.y_num, gradient.x_num, gradient.z_num, 0);

    t.start_timer("whole");

    //    std::swap(part_map.downsampled[part_map.k_max+1],input_image_float);

    part_rep.timer.start_timer("get_gradient_3D");
    get_gradient_3D(part_rep, input_image_float, gradient);
    part_rep.timer.stop_timer();

    part_rep.timer.start_timer("get_variance_3D");
    get_variance_3D(part_rep, input_image_float, variance);
    part_rep.timer.stop_timer();


    MeshData<float> variance_u;

    int x_dim = ceil(gradient.x_num/2.0)*2;
    int z_dim = ceil(gradient.z_num/2.0)*2;
    int y_dim = ceil(gradient.y_num/2.0)*2;

    variance_u.mesh.resize(x_dim*z_dim*y_dim,0);

    std::vector<unsigned int> dims = {(unsigned int)gradient.y_num,(unsigned int)gradient.x_num,(unsigned int)gradient.z_num};

    const_upsample_img(variance_u,variance,dims);

    float k_factor;

    float min_dim = std::min(part_rep.pars.dy,std::min(part_rep.pars.dx,part_rep.pars.dz));

    k_factor = pow(2,part_rep.pl_map.k_max+1)*min_dim;

    compute_k_for_array(temp,k_factor,part_rep.pars.rel_error);


    for (int l = 0; l < gradient.mesh.size(); ++l) {

        if(gradient.mesh[l] > 0.0001) {
            gradient.mesh[l] = (k_factor * variance_u.mesh[l] * part_rep.pars.rel_error) / abs(gradient.mesh[l]);
        } else {
            gradient.mesh[l] = k_factor;
        }
    }


    /////////////////////////////////////////////
    //
    //  Now we want to loop over and calculate the maximum in the area given by the apr
    //
    //////////////////////////////////////////////


    MeshData<float> resolution;
    resolution.initialize(gradient.y_num, gradient.x_num, gradient.z_num, 0);


    int a = 1;


    int x_num = gradient.x_num;
    int y_num = gradient.y_num;
    int z_num = gradient.z_num;

    int k_max = part_rep.pl_map.k_max;

    Part_timer timer;

    timer.verbose_flag = true;


    timer.start_timer("brute force loop");

    float cumsum = 0;

    int i,j,k;

#pragma omp parallel for default(shared) private(j,i,k) reduction(+: cumsum)
    for(j = 0; j < gradient.z_num;j++){
        for(i = 0; i < gradient.x_num;i++){

            for(k = 0;k < gradient.y_num;k++){


                bool bound = true;
                int R = 1;

                while(bound) {

                    int step_size = R;


                    int offset_max_y = std::min((int) (k + step_size), (int) (y_num - 1));
                    int offset_min_y = std::max((int) (k - step_size), (int) 0);

                    int offset_max_x = std::min((int) (i + step_size), (int) (x_num - 1));
                    int offset_min_x = std::max((int) (i - step_size), (int) 0);

                    int offset_max_z = std::min((int) (j + step_size), (int) (z_num - 1));
                    int offset_min_z = std::max((int) (j - step_size), (int) 0);

                    for (float a = offset_min_z; a <= offset_max_z; a++) {

                        if(a == offset_min_z || a == offset_max_z) {
                            for (float b = offset_min_x; b <= offset_max_x; b++) {

                                if (b == offset_min_z || b == offset_max_z) {

                                    for (float c = offset_min_y; c <= offset_max_y; c++) {

                                        float dist = sqrt(
                                                pow(round(a - j), 2.0) + pow(round(b - i), 2.0) +
                                                pow(round(c - k), 2.0));


                                        if (dist <= step_size) {

                                            float curr_L = gradient.mesh[a * x_num * y_num + b * y_num + c];

                                            if (R * part_rep.pars.dx > curr_L) {
                                                bound = false;
                                                goto escape2;

                                            }


                                        }

                                    }

                                } else {

                                    float c = offset_min_y;

                                    float dist = sqrt(
                                            pow(round(a - j), 2.0) + pow(round(b - i), 2.0) +
                                            pow(round(c - k), 2.0));


                                    if (dist <= step_size) {

                                        float curr_L = gradient.mesh[a * x_num * y_num + b * y_num + c];

                                        if (R * part_rep.pars.dx > curr_L) {
                                            bound = false;
                                            goto escape2;

                                        }


                                    }

                                    c = offset_max_y;

                                    dist = sqrt(
                                            pow(round(a - j), 2.0) + pow(round(b - i), 2.0) +
                                            pow(round(c - k), 2.0));


                                    if (dist <= step_size) {

                                        float curr_L = gradient.mesh[a * x_num * y_num + b * y_num + c];

                                        if (R * part_rep.pars.dx > curr_L) {
                                            bound = false;
                                            goto escape2;

                                        }


                                    }


                                }


                            }

                        }else {
                            for (float b = offset_min_x; b <= offset_max_x; b++) {
                                for (float c = offset_min_y; c <= offset_max_y; c++) {

                                    float dist = sqrt(
                                            pow(round(a - j), 2.0) + pow(round(b - i), 2.0) + pow(round(c - k), 2.0));



                                    if (dist <= step_size) {

                                        float curr_L = gradient.mesh[a * x_num * y_num + b * y_num + c];

                                        if (R * part_rep.pars.dx > curr_L) {
                                            bound = false;
                                            goto escape2;

                                        }


                                    }

                                }
                            }
                        }



                    }


                    escape2:

                    R++;

                }

                resolution.mesh[j * x_num * y_num + i * y_num + k] = (R-1);
                //cumsum += (R-1)*part_rep.pars.dx*part_rep.pars.dx;
                float R_ = std::max(R-1,1);
                cumsum += 1.0f/(R_);
            }
        }
    }


    timer.stop_timer();


    analysis_data.add_timer(timer);

    analysis_data.add_float_data("R_c",cumsum);

   // debug_write(resolution,"resolution2");

    std::cout << "sum: " << cumsum << std::endl;

}


void gen_parameter_pars(SynImage& syn_image,Proc_par& pars,std::string image_name){
    //
    //
    //  Takes in the SynImage model parameters and outputs them to the APR parameter class
    //
    //
    //

    pars.name = image_name;

    pars.dy = syn_image.sampling_properties.voxel_real_dims[0];
    pars.dx = syn_image.sampling_properties.voxel_real_dims[1];
    pars.dz = syn_image.sampling_properties.voxel_real_dims[2];

    pars.psfy = syn_image.PSF_properties.real_sigmas[0];
    pars.psfx = syn_image.PSF_properties.real_sigmas[1];
    pars.psfz = syn_image.PSF_properties.real_sigmas[2];

    pars.ydim = syn_image.real_domain.size[0];
    pars.xdim = syn_image.real_domain.size[1];
    pars.zdim = syn_image.real_domain.size[2];

    pars.noise_sigma = sqrt(syn_image.noise_properties.gauss_var);
    pars.background = syn_image.global_trans.const_shift;

}


template<typename S,typename U>
void compare_reconstruction_to_original(MeshData<S>& org_img,PartCellStructure<float,U>& pc_struct,cmdLineOptions& options){
    //
    //  Bevan Cheeseman 2017
    //
    //  Compares an APR pc reconstruction with original image.
    //
    //

    AnalysisData analysis_data;

    MeshData<S> rec_img;
    pc_struct.interp_parts_to_pc(rec_img,pc_struct.part_data.particle_data);

    std::string name = "input";
    //get the MSE
    //calc_mse(org_img,rec_img,name,analysis_data);

    debug_write(rec_img,name +"rec_img");
    //compare_E(org_img,rec_img,options,name,analysis_data);

}

template<typename S>
void check_var(MeshData<S>& org_img,Proc_par& pars,std::string name,AnalysisData& analysis_data,SynImage& syn_image){
    //
    //
    //  Bevan Cheeseman 2017
    //
    //  Rescaling check
    //


    MeshData<float> variance;

    get_variance(org_img,variance,pars);

    Map_calc_cpu calc_map(0,pars);

    calc_map.set_up_var_filters_3D();

    analysis_data.add_float_data("re_scale",calc_map.var_rescale);

    analysis_data.add_float_data("window_ref",calc_map.window_ref);

    analysis_data.add_float_data("window_size",calc_map.var_window_size1);

    float cen_offset = round(1.5/syn_image.sampling_properties.sampling_delta[0]);

    float mid = round(variance.y_num/2);

    float var_val = variance(mid + cen_offset,mid + cen_offset,mid + cen_offset);
    float img_val = org_img(mid ,mid ,mid ) - syn_image.global_trans.const_shift;


    analysis_data.add_float_data("img_val",img_val);
    analysis_data.add_float_data("var_val",var_val);

    std::cout << var_val << " " << img_val << std::endl;


};

template<typename S,typename T>
void compare_E(MeshData<S>& org_img,MeshData<T>& rec_img,Proc_par& pars,std::string name,AnalysisData& analysis_data) {
    MeshData<float> variance;

    get_variance(org_img, variance, pars);

    compare_E(org_img,rec_img,pars,name,analysis_data,variance);

}

template<typename S,typename T>
void compare_E(MeshData<S>& org_img,MeshData<T>& rec_img,Proc_par& pars,std::string name,AnalysisData& analysis_data,MeshData<float>& variance){


    uint64_t z_num_o = org_img.z_num;
    uint64_t x_num_o = org_img.x_num;
    uint64_t y_num_o = org_img.y_num;

    uint64_t z_num_r = rec_img.z_num;
    uint64_t x_num_r = rec_img.x_num;
    uint64_t y_num_r = rec_img.y_num;

    variance.x_num = x_num_r;
    variance.y_num = y_num_r;
    variance.z_num = z_num_r;

    uint64_t j = 0;
    uint64_t k = 0;
    uint64_t i = 0;

    MeshData<float> SE;
    //SE.initialize(y_num_o,x_num_o,z_num_o,0);

    double mean = 0;
    double inf_norm = 0;
    uint64_t counter = 0;
    double MSE = 0;
    double L1 = 0;

    //ignored boundary layer
    int b = 0;

#pragma omp parallel for default(shared) private(j,i,k) reduction(+: MSE) reduction(+: counter) reduction(+: mean) reduction(max: inf_norm)
    for(j = b; j < (z_num_o-b);j++){
        for(i = b; i < (x_num_o-b);i++){

            for(k = b;k < (y_num_o-b);k++){
                double val = abs(org_img.mesh[j*x_num_o*y_num_o + i*y_num_o + k] - rec_img.mesh[j*x_num_r*y_num_r + i*y_num_r + k])/(1.0*variance.mesh[j*x_num_r*y_num_r + i*y_num_r + k]);
                //SE.mesh[j*x_num_o*y_num_o + i*y_num_o + k] = 1000*val;

                if(variance.mesh[j*x_num_r*y_num_r + i*y_num_r + k] < 60000) {
                    MSE += pow(org_img.mesh[j*x_num_o*y_num_o + i*y_num_o + k] - rec_img.mesh[j*x_num_r*y_num_r + i*y_num_r + k],2);
                    L1 += abs(org_img.mesh[j*x_num_o*y_num_o + i*y_num_o + k] - rec_img.mesh[j*x_num_r*y_num_r + i*y_num_r + k]);

                    mean += val;
                    inf_norm = std::max(inf_norm, val);
                    counter++;
                }
            }
        }
    }

    mean = mean/(1.0*counter);
    MSE = MSE/(1*counter);
    L1 = L1/(1*counter);

    //debug_write(SE,name + "E_diff");

    double MSE_var = 0;
    //calculate the variance
    double var = 0;
    counter = 0;

#pragma omp parallel for default(shared) private(j,i,k) reduction(+: var) reduction(+: counter) reduction(+: MSE_var)
    for(j = b; j < (z_num_o-b);j++){
        for(i = b; i < (x_num_o-b);i++){

            for(k = b;k < (y_num_o-b);k++){

                if(variance.mesh[j*x_num_r*y_num_r + i*y_num_r + k] < 60000) {
                    var += pow(pow(org_img.mesh[j*x_num_o*y_num_o + i*y_num_o + k] - rec_img.mesh[j*x_num_r*y_num_r + i*y_num_r + k],2) - MSE,2);
                    MSE_var += pow(pow(org_img.mesh[j*x_num_o*y_num_o + i*y_num_o + k] - rec_img.mesh[j*x_num_o*y_num_r + i*y_num_r + k],2) - MSE,2);

                    counter++;
                }
            }
        }
    }


    //debug_write(variance,name + "var");
    //debug_write(rec_img,name +"rec_img");
    //debug_write(org_img,name +"org_img");

    //get variance
    var = var/(1.0*counter);
    double se = 1.96*sqrt(var);
    MSE_var = MSE_var/(1.0*counter);

    analysis_data.add_float_data(name+"_Ediff",mean);
    analysis_data.add_float_data(name+"_Ediff_sd",sqrt(var));
    analysis_data.add_float_data(name+"_Ediff_inf",inf_norm);
    analysis_data.add_float_data(name+"_Ediff_se",se);

    double PSNR = 10*log10(64000.0/MSE);

    analysis_data.add_float_data(name+"_vMSE",MSE);
    analysis_data.add_float_data(name+"_vL1",L1);
    analysis_data.add_float_data(name+"_vMSE_sd",sqrt(MSE_var));
    analysis_data.add_float_data(name+"_vPSNR",PSNR);

    float rel_error = pars.rel_error;

    if(pars.lambda == 0) {
        if (inf_norm > rel_error) {
            int stop = 1;
            std::cout << "*********Out of bounds!*********" << std::endl;

            //debug_write(org_img,"org_img_out_bounds");
            //debug_write(rec_img,"rec_img_out_bounds");
            //debug_write(variance,"var_img_out_bounds");

            // assert(inf_norm < rel_error);
        }
    }

    std::cout << name << " E: " << pars.rel_error << " inf_norm: " << inf_norm << std::endl;

}

std::vector<double> get_cell_types(PartCellStructure<float,uint64_t>& pc_struct) {


    std::vector<double> status_vec = {0,0,0}; //(Seed,Boundary,Filler)

    uint64_t z_,x_,j_,status,node_val_pc;

    for (int i = pc_struct.pc_data.depth_min; i <= pc_struct.pc_data.depth_max; i++) {
//loop over the resolutions of the structure
        const unsigned int x_num_ = pc_struct.pc_data.x_num[i];
        const unsigned int z_num_ = pc_struct.pc_data.z_num[i];

        for (z_ = 0; z_ < z_num_; z_++) {
//both z and x are explicitly accessed in the structure

            for (x_ = 0; x_ < x_num_; x_++) {


                const size_t offset_pc_data = x_num_ * z_ + x_;

                const size_t j_num = pc_struct.pc_data.data[i][offset_pc_data].size();


                for (j_ = 0; j_ < j_num; j_++) {

                    node_val_pc = pc_struct.pc_data.data[i][offset_pc_data][j_];

                    if (!(node_val_pc & 1)) {


                        status = pc_struct.pc_data.get_status(node_val_pc);

                        status_vec[status-1]++;

                    } else {

                    }

                }

            }

        }
    }


    return status_vec;

}


uint64_t get_size_of_pc_struct(PartCellData<uint64_t>& pc_data){
    //
    //  Bevan Cheeseman 2017
    //
    //  Get the number of elements in the pc struct data structure
    //

    uint64_t counter = 0;


    for(uint64_t depth = (pc_data.depth_min);depth <= pc_data.depth_max;depth++) {
        //loop over the resolutions of the structure
        for(int i = 0;i < pc_data.data[depth].size();i++){

            counter += pc_data.data[depth][i].size();
        }

    }

    return counter;

}
void compare_depth_rep(PartCellStructure<float,uint64_t>& pc_struct,PartCellStructure<float,uint64_t>& pc_struct_perfect,AnalysisData& analysis_data){
    //
    //  Bevan Cheeseman 2017
    //
    //  Compare the depth of the two APRs
    //
    //


    MeshData<uint8_t> k_img;

    interp_depth_to_mesh(k_img,pc_struct,0);

    MeshData<uint8_t> k_img_perfect;

    interp_depth_to_mesh(k_img_perfect,pc_struct_perfect,0);

    MeshData<float> diff;

    diff.initialize(k_img.x_num,k_img.y_num,k_img.z_num,0);

    float counter_p = 0;
    float counter_m = 0;
    float sum_p = 0;
    float sum_m  = 0;
    float max_m = 0;

    float norm_total = 0;


    for (int i = 0; i < k_img.mesh.size(); ++i) {
        diff.mesh[i] = 10 + k_img.mesh[i] - k_img_perfect.mesh[i];

        if(diff.mesh[i] > 10){
            counter_p+=1.0/pow(2,pc_struct_perfect.depth_max + 1 - k_img_perfect.mesh[i]);
            sum_p += diff.mesh[i] - 10;
        }

        if(diff.mesh[i] < 10){
            counter_m+=1.0/pow(2,pc_struct_perfect.depth_max + 1 - k_img_perfect.mesh[i]);
            sum_m += -(diff.mesh[i] - 10);
            max_m = std::max(-(diff.mesh[i] - 10),max_m);
        }

        norm_total +=1.0/pow(2,pc_struct_perfect.depth_max + 1 - k_img_perfect.mesh[i]);

    }


    //debug_write(k_img_perfect,"k_perfect");

    debug_write(diff,"diff_img");

    analysis_data.add_float_data("k_plus",counter_p/norm_total);
    analysis_data.add_float_data("k_minus",counter_m/norm_total);
    analysis_data.add_float_data("k_minus_avg",sum_m/counter_m);
    analysis_data.add_float_data("k_plus_avg",sum_p/counter_m);

}

void compare_var_func(PartCellStructure<float,uint64_t>& pc_struct_perfect,MeshData<float> var_gt,MeshData<float> var_comp,AnalysisData& analysis_data,float factor = 1){
    //
    //  Evaluates and compares the variance at high resolution areas between the ground truth and the computed var
    //
    //  Bevan Cheeseman 2017
    //
    //

    MeshData<uint8_t> k_img_perfect;

    interp_depth_to_mesh(k_img_perfect,pc_struct_perfect);

    float max_ratio = 0;
    float min_ratio = 99999999;
    float sum_ratio = 0;
    float counter_ratio = 0 ;




    float val =pc_struct_perfect.pc_data.depth_max + 1;

    for (int i = 0; i < k_img_perfect.mesh.size(); ++i) {
        if(k_img_perfect.mesh[i] == val){
            counter_ratio++;
            sum_ratio += var_comp.mesh[i]/(factor*var_gt.mesh[i]);
            max_ratio = std::max(var_comp.mesh[i]/(factor*var_gt.mesh[i]),max_ratio);
            min_ratio = std::min(var_comp.mesh[i]/(factor*var_gt.mesh[i]),min_ratio);
        }

    }


    float mean = sum_ratio/counter_ratio;

    float var = 0;

    for (int i = 0; i < k_img_perfect.mesh.size(); ++i) {
        if(k_img_perfect.mesh[i] == val){

            var += pow((var_comp.mesh[i]/(factor*var_gt.mesh[i]) - mean),2);

        }

    }

    var = var/counter_ratio;

    analysis_data.add_float_data("var_ratio",sum_ratio/counter_ratio);

    analysis_data.add_float_data("max_ratio",max_ratio);

    analysis_data.add_float_data("min_ratio",min_ratio);

    analysis_data.add_float_data("std_ratio",sqrt(var));

}
template<typename T>
void true_int(MeshData<T>& input_image,AnalysisData& analysis_data,PartCellStructure<float,uint64_t>& pc_struct,SynImage& syn_image,Proc_par& pars,std::string name,MeshData<float>& var) {
//Generate clean gt image&
//    MeshData<float> gt_imaged;
//    generate_gt_image(gt_imaged, syn_image);
//
//    Part_rep p_rep(input_image.y_num, input_image.x_num, input_image.z_num);
//
//    p_rep.pars = pars;
//
//    APR<float> t_apr;
//
//    create_pc_data_new(t_apr,pc_struct);
//
//    ExtraPartCellData<float> true_parts(t_apr);
//
//
//    Particle_map<float> part_map(p_rep);
//
//    part_map.downsample(gt_imaged);
//
//
//    t_apr.get_parts_from_img(part_map.downsampled,true_parts);

//
//
//    int z_, x_, j_, y_, i, k;
//
//    for (uint64_t depth = (t_apr.y_vec.depth_min); depth <= t_apr.y_vec.depth_max; depth++) {
////loop over the resolutions of the structure
//        const unsigned int x_num_ = t_apr.y_vec.x_num[depth];
//        const unsigned int z_num_ = t_apr.y_vec.z_num[depth];
//
//        const float step_size_x = pow(2, t_apr.y_vec.depth_max - depth);
//        const float step_size_y = pow(2, t_apr.y_vec.depth_max - depth);
//        const float step_size_z = pow(2, t_apr.y_vec.depth_max - depth);
//
//        for (z_ = 0; z_ < z_num_; z_++) {
////both z and x are explicitly accessed in the structure
//
//            for (x_ = 0; x_ < x_num_; x_++) {
//
//                const unsigned int pc_offset = x_num_ * z_ + x_;
//
//                for (j_ = 0; j_ < t_apr.y_vec.data[depth][pc_offset].size(); j_++) {
//
//
//                    const int y = t_apr.y_vec.data[depth][pc_offset][j_];
//
//                    const unsigned int y_actual = ((y));
//                    const unsigned int x_actual = ((x_));
//                    const unsigned int z_actual = ((z_));
//
//                    true_parts.data[depth][pc_offset][j_] = part_map.downsampled[depth](y_actual, x_actual, z_actual);
//
//                }
//            }
//        }
//    }

//    MeshData<float> true_int_m;
//
//    MeshData<uint16_t> gt_image;
//
//    t_apr.interp_img(true_int_m,true_parts);
//
//    generate_gt_image(gt_image, syn_image);
//
//    name = name + "_true";
//
//    compare_E(gt_image, true_int_m, pars, name, analysis_data,var);
//
//    calc_mse(gt_image, true_int_m, name, analysis_data);

}


template<typename T>
void produce_apr_analysis(MeshData<T>& input_image,AnalysisData& analysis_data,PartCellStructure<float,uint64_t>& pc_struct,SynImage& syn_image,Proc_par& pars) {
    //
    //  Computes anslysis of part rep dataset
    //
    //  Bevan Cheeseman 2017
    //

    Part_timer timer;

    analysis_data.push_proc_par(pars);


    //////////////////////////////////////
    //
    //  Initialize - data anslysis fields
    //
    /////////////////////////////////////////

    //first check if its first run and the variables need to be set up
    Part_data<int> *check_ref = analysis_data.get_data_ref<int>("num_parts");

    if (check_ref == nullptr) {
        // First experiment need to set up the variables

        //pipeline parameters
        analysis_data.create_int_dataset("num_parts", 0);
        analysis_data.create_int_dataset("num_cells", 0);
        analysis_data.create_int_dataset("num_seed_cells", 0);
        analysis_data.create_int_dataset("num_boundary_cells", 0);
        analysis_data.create_int_dataset("num_filler_cells", 0);
        analysis_data.create_int_dataset("num_ghost_cells", 0);

        analysis_data.create_int_dataset("num_pixels", 0);

        analysis_data.create_float_dataset("apr_comp_size", 0);
        analysis_data.create_float_dataset("apr_full_size", 0);
        analysis_data.create_float_dataset("image_size", 0);
        analysis_data.create_float_dataset("comp_image_size", 0);

        analysis_data.create_float_dataset("information_content", 0);
        analysis_data.create_float_dataset("relerror", 0);

        //set up timing variables

        for (int i = 0; i < timer.timings.size(); i++) {
            analysis_data.create_float_dataset(timer.timing_names[i], 0);
        }


    }

    /////////////////////////////////////////////
    //
    //  APR Size Info
    //
    //////////////////////////////////////////////

    analysis_data.get_data_ref<int>("num_parts")->data.push_back(pc_struct.get_number_parts());
    analysis_data.part_data_list["num_parts"].print_flag = true;

    std::vector<double> status_vec = get_cell_types(pc_struct);

    //calc number of active cells
    int num_seed_cells = status_vec[0];
    int num_boundary_cells = status_vec[1];
    int num_filler_cells = status_vec[2];
    int num_ghost_cells = 0;
    int num_cells = 0;

    int full_sampling = num_filler_cells + 8*(num_seed_cells + num_boundary_cells);


    analysis_data.add_float_data("num_parts_full",full_sampling);

    analysis_data.get_data_ref<int>("num_cells")->data.push_back(num_seed_cells + num_boundary_cells + num_filler_cells);
    analysis_data.part_data_list["num_cells"].print_flag = true;

    analysis_data.get_data_ref<int>("num_boundary_cells")->data.push_back(num_boundary_cells);
    analysis_data.part_data_list["num_boundary_cells"].print_flag = true;

    analysis_data.get_data_ref<int>("num_filler_cells")->data.push_back(num_filler_cells);
    analysis_data.part_data_list["num_filler_cells"].print_flag = true;

    analysis_data.get_data_ref<int>("num_seed_cells")->data.push_back(num_seed_cells);
    analysis_data.part_data_list["num_seed_cells"].print_flag = true;

    analysis_data.get_data_ref<int>("num_ghost_cells")->data.push_back(num_ghost_cells);
    analysis_data.part_data_list["num_ghost_cells"].print_flag = true;

    analysis_data.get_data_ref<int>("num_pixels")->data.push_back(
            input_image.y_num * input_image.x_num * input_image.z_num);
    analysis_data.part_data_list["num_pixels"].print_flag = true;

    std::cout << (1.0*input_image.y_num * input_image.x_num * input_image.z_num)/(1.0*pc_struct.get_number_parts()) << std::endl;

    ////////////////////////////////////////////////////////////////////
    //
    //  Timing Information
    //
    ////////////////////////////////////////////////////////////////////

    for (int i = 0; i < timer.timings.size(); i++) {
        analysis_data.get_data_ref<float>(timer.timing_names[i])->data.push_back(timer.timings[i]);
        analysis_data.part_data_list[timer.timing_names[i]].print_flag = true;
    }

    //////////////////////////////////////////////////////////////////////
    //
    //  Run segmentation Benchmark
    //
    //////////////////////////////////////////////////////////////////

    if(analysis_data.segmentation_mesh) {

        run_segmentation_benchmark_mesh(pc_struct, analysis_data);

    }

    if(analysis_data.segmentation_parts) {

        run_segmentation_benchmark_parts(pc_struct, analysis_data);

    }


    if(analysis_data.filters_mesh){

        run_filter_benchmarks_mesh(pc_struct, analysis_data);

    }

    if(analysis_data.filters_parts){

        run_filter_benchmarks_parts(pc_struct, analysis_data);

    }

    if(analysis_data.segmentation_eval) {
        //evaluate_segmentation(pc_struct,analysis_data,syn_image);

        //evaluate_filters(pc_struct,analysis_data,syn_image,input_image);

        Part_rep p_rep;
        p_rep.pars = pars;


        run_real_segmentation(pc_struct,analysis_data, pars);

        //run_ray_cast(pc_struct,analysis_data,input_image,pars);

        //real_adaptive_grad(pc_struct,analysis_data,input_image, pars);


        // evaluate_enhancement(pc_struct,analysis_data,syn_image,input_image,p_rep);
    }

    if(analysis_data.filters_eval) {

        // evaluate_filters(pc_struct,analysis_data,syn_image,input_image);
        evaluate_filters_guassian(pc_struct,analysis_data,syn_image,input_image,.5);
        evaluate_filters_guassian(pc_struct,analysis_data,syn_image,input_image, 2);

        evaluate_adaptive_smooth(pc_struct,analysis_data,syn_image,input_image);
        evaluate_adaptive_grad(pc_struct,analysis_data,syn_image,input_image);

        //evaluate_filters_log(pc_struct,analysis_data,syn_image,input_image);
    }

    ////////////////////////////////////////////////////////////////////
    //
    //  File Size Information
    //
    /////////////////////////////////////////////////////////////////////

    //write apr and image to file

    if(analysis_data.file_size) {

        std::string file_name = pars.output_path + pars.name;

        write_apr_pc_struct(pc_struct, pars.output_path, pars.name);

        write_image_tiff(input_image, file_name + ".tif");

        analysis_data.get_data_ref<float>("apr_full_size")->data.push_back(
                GetFileSize(file_name + "_pcstruct_part.h5"));
        analysis_data.part_data_list["apr_full_size"].print_flag = true;

        analysis_data.get_data_ref<float>("image_size")->data.push_back(GetFileSize(file_name + ".tif"));
        analysis_data.part_data_list["image_size"].print_flag = true;



        //produce the compressed image file to get baseline
        std::string compress_file_name = file_name + ".bz2";
        std::string system_string = "pbzip2 -c -9 <" + file_name + ".tif" + ">" + compress_file_name;
        system(system_string.c_str());

        analysis_data.get_data_ref<float>("comp_image_size")->data.push_back(GetFileSize(compress_file_name));
        analysis_data.part_data_list["comp_image_size"].print_flag = true;
    }

    analysis_data.get_data_ref<float>("relerror")->data.push_back(pars.rel_error);
    analysis_data.part_data_list["relerror"].print_flag = true;

    ///////////////////////////
    //
    //  Compute Reconstruction Quality Metrics
    //
    ///////////////////////////

    timer.verbose_flag = false;

    timer.start_timer("Image Quality");

    MeshData<float> rec_img;
    std::string name;

    if(analysis_data.quality_metrics_gt || analysis_data.quality_metrics_input) {

        //get the pc reconstruction

        pc_struct.interp_parts_to_pc(rec_img, pc_struct.part_data.particle_data);


    }

    //check_var(input_image,pars,"test",analysis_data,syn_image);

    if(analysis_data.debug == true){

        MeshData<T> rec_img_d;

        pc_struct.interp_parts_to_pc(rec_img_d, pc_struct.part_data.particle_data);
        write_image_tiff(rec_img_d, pars.output_path + pars.name + "_rec.tif");

        write_image_tiff(input_image, pars.output_path + pars.name + "_debug.tif");

        write_apr_pc_struct(pc_struct, pars.output_path, pars.name);

        write_apr_full_format(pc_struct,pars.output_path, pars.name);

        MeshData<float> variance;

        get_variance(input_image,variance,pars);

        debug_write(variance,"var");

        MeshData<uint8_t> k_img;

        interp_depth_to_mesh(k_img,pc_struct);

        write_image_tiff(k_img, pars.output_path + pars.name + "_k.tif");

        MeshData<T> gt_image;
        generate_gt_image(gt_image, syn_image);

        name = "debug";
        compare_E_debug( gt_image,rec_img_d, pars, name, analysis_data);


        debug_write(gt_image,"gt_image");

        generate_gt_seg(gt_image,syn_image);

        debug_write(gt_image,"template_image");

    }


    if(analysis_data.check_scale){

        Part_rep p_rep;
        p_rep.pars = pars;

        //test_local_scale(input_image, p_rep,pc_struct,analysis_data);

        cont_solution(input_image, p_rep,pc_struct,analysis_data);


        std::cout << " full  sampling: " << full_sampling << std::endl;
    }

    if(analysis_data.quality_true_int) {
//#fixme
        //Generate clean gt image
        MeshData<float> gt_imaged;
        generate_gt_image(gt_imaged, syn_image);

        Part_rep p_rep(input_image.y_num,input_image.x_num,input_image.z_num);

        p_rep.pars = pars;

        APR<float> t_apr;

        create_pc_data_new(t_apr,pc_struct);

        ExtraPartCellData<float> true_parts(t_apr);

        Particle_map<float> part_map(p_rep);

        part_map.downsample(gt_imaged);

        //t_apr.get_parts_from_img(part_map.downsampled,true_parts);


        MeshData<float> true_int_m;

        MeshData<uint16_t> gt_image;


        // t_apr.interp_img(true_int_m,true_parts); #broken

        generate_gt_image(gt_image, syn_image);

        name = "true";
        //compare_E(true_int_m, gt_image, pars, name, analysis_data);

        //calc_mse(true_int_m, gt_image, name, analysis_data);

        compare_E(gt_image,true_int_m, pars, name, analysis_data);

        calc_mse(gt_image,true_int_m, name, analysis_data);

        //debug_write(true_int_m,"true");
        //debug_write(gt_image,"gt");
        //debug_write(input_image,"input");




    }




    if(analysis_data.quality_metrics_gt) {


        //Generate clean gt image
        MeshData<uint16_t> gt_image;
        generate_gt_image(gt_image, syn_image);


        name = "orggt";
        compare_E(gt_image, input_image, pars, name, analysis_data);

        calc_mse(gt_image, input_image, name, analysis_data);



        MeshData<float> median_filt_img;

        median_filt_img.initialize(input_image.y_num,input_image.x_num,input_image.z_num);

        std::copy(input_image.mesh.begin(),input_image.mesh.end(),median_filt_img.mesh.begin());

        calc_median_filter(median_filt_img);


        name = "mediangt";
        compare_E(gt_image, median_filt_img, pars, name, analysis_data);

        calc_mse(gt_image, median_filt_img, name, analysis_data);


        name = "recgt";

        //get the MSE
        calc_mse(gt_image, rec_img, name, analysis_data);
        compare_E(gt_image, rec_img, pars, name, analysis_data);


        std::vector<float> scale = {2,2,2};

        MeshData<float> smooth_img;

        interp_parts_to_smooth(smooth_img,pc_struct.part_data.particle_data,pc_struct,scale);

        name = "smoothgt2";

        //get the MSE
        calc_mse(gt_image, smooth_img, name, analysis_data);
        compare_E(gt_image, smooth_img, pars, name, analysis_data);

         scale = {1,1,1};

        interp_parts_to_smooth(smooth_img,pc_struct.part_data.particle_data,pc_struct,scale);

        name = "smoothgt1";

        //get the MSE
        calc_mse(gt_image, smooth_img, name, analysis_data);
        compare_E(gt_image, smooth_img, pars, name, analysis_data);

        //debug_write(smooth_img,"smooth_img");

//        ParticleDataNew<float, uint64_t> part_new;
//        //flattens format to particle = cell, this is in the classic access/part paradigm
//        part_new.initialize_from_structure(pc_struct);
//
//        //generates the nieghbour structure
//        PartCellData<uint64_t> pc_data;
//        part_new.create_pc_data_new(pc_data);
//
//        pc_data.org_dims = pc_struct.org_dims;
//        part_new.access_data.org_dims = pc_struct.org_dims;
//
//        part_new.particle_data.org_dims = pc_struct.org_dims;
//
//
//        MeshData<float> w_interp_out;
//
//        weigted_interp_img(w_interp_out, pc_data, part_new, part_new.particle_data,false,true);
//
//        MeshData<float> min_img;
//        MeshData<float> max_img;
//
//        min_max_interp(min_img,max_img,pc_data,part_new,part_new.particle_data,false);
//
//
//        std::string name = "we_";
//        compare_E( gt_image,w_interp_out, pars, name, analysis_data);
//
//        name = "max_";
//        compare_E( gt_image,max_img, pars, name, analysis_data);
//
//        name = "min_";
//        compare_E( gt_image,min_img, pars, name, analysis_data);



    }

    if(analysis_data.quality_metrics_input){

        name = "input";

        //get the MSE
        calc_mse(input_image, rec_img, name, analysis_data);
        compare_E(input_image, rec_img, pars, name, analysis_data);






    }

    timer.stop_timer();

    timer.start_timer("information_content");

    ///////////////////////////////////////
    //
    //  Calc information content
    //
    /////////////////////////////////////////////

    if(analysis_data.information_content) {

        calc_information_content(syn_image, analysis_data);

    }
    timer.stop_timer();


    if(analysis_data.comp_perfect) {

        Part_rep p_rep;
        p_rep.pars = pars;
        p_rep.pars.name = "perfect";

        MeshData<float> norm_grad_image;

        MeshData<float> gt_image;
        generate_gt_image(gt_image, syn_image);

        p_rep.pars.lambda = -1;

        MeshData<float> grad_image;

        grad_image.initialize(gt_image.y_num,gt_image.x_num,gt_image.z_num,0);

        //generate_gt_norm_grad(norm_grad_image,syn_image,true,pars.dx,pars.dy,pars.dz);
        get_gradient_3D(p_rep, gt_image, grad_image);



       // generate_gt_norm_grad(grad_image,syn_image,false,1,1,1);

        //debug_write(grad_image,"grad_p");

        MeshData<float> var_gt;

        generate_gt_var(gt_image,var_gt,syn_image,pars);


        p_rep.initialize(input_image.y_num,input_image.x_num,input_image.z_num);

        PartCellStructure<float, uint64_t> pc_struct_perfect;
        get_apr_perfect(input_image,grad_image,var_gt,p_rep,pc_struct_perfect,analysis_data);

        std::cout << "Num Parts: " << pc_struct.get_number_parts() << std::endl;

        std::cout << "Num Parts Perfect: " << pc_struct_perfect.get_number_parts() << std::endl;

        analysis_data.add_float_data("perfect_parts",pc_struct_perfect.get_number_parts());

        // Get the ground truth variance

        MeshData<float> variance_u;
        //need to down sample / then upsample variance
        down_sample(var_gt,variance_u,
                    [](float x, float y) { return std::max(x,y); },
                    [](float x) { return x; },true);

        std::vector<unsigned int> dims = {(unsigned int)var_gt.y_num,(unsigned int)var_gt.x_num,(unsigned int)var_gt.z_num};


        int y_size_max = ceil(dims[0]/2.0)*2;
        int x_size_max = ceil(dims[1]/2.0)*2;
        int z_size_max = ceil(dims[2]/2.0)*2;

        const int z_num = std::min(var_gt.z_num*2,z_size_max);
        const int x_num = std::min(var_gt.x_num*2,x_size_max);
        const int y_num = std::min(var_gt.y_num*2,y_size_max);

        var_gt.initialize(y_num,x_num,z_num,0);

        const_upsample_img(var_gt,variance_u,dims);

        MeshData<float> rec_perfect;

        pc_struct_perfect.interp_parts_to_pc(rec_perfect, pc_struct_perfect.part_data.particle_data);


        // Compute Image Stats
        std::string name_p = "rv_perfect";

        compare_E(gt_image,rec_perfect, pars, name_p, analysis_data);

        std::string name_p_v = "gtv_perfect";

        compare_E(gt_image,rec_perfect, pars, name_p_v, analysis_data,var_gt);

        std::string name_p_vr = "gtv_rec";

        compare_E(gt_image,rec_img, pars, name_p_vr, analysis_data,var_gt);

        name_p_vr = "gtv_org";

        compare_E(gt_image,input_image, pars, name_p_vr, analysis_data,var_gt);

        // compute perfect MSE and PSNR everywhere

        calc_mse(gt_image, rec_perfect, "perfect", analysis_data);


        compare_depth_rep(pc_struct,pc_struct_perfect,analysis_data);

        MeshData<float> variance;

        get_variance(input_image, variance, pars);

        compare_var_func(pc_struct_perfect,var_gt,variance,analysis_data,pars.mean_scale);

        debug_write(var_gt,"var_gt");

        //debug_write(rec_perfect,"rec_perfect");


        true_int(input_image,analysis_data,pc_struct_perfect,syn_image,pars,"perfect",var_gt);


    }


}
template<typename T>
void produce_apr_analysis(MeshData<T>& input_image,AnalysisData& analysis_data,PartCellStructure<float,uint64_t>& pc_struct,Proc_par& pars){
    //
    //  Bevan Cheeseman 2017
    //
    //  Interface for running on real data without syn image benchmarks
    //
    //

    SynImage syn_image_temp;

    //these cannot be run without a syntehtic image ground truth
    analysis_data.quality_metrics_gt = false;
    analysis_data.information_content = false;

    produce_apr_analysis(input_image,analysis_data,pc_struct,syn_image_temp,pars);

}

void compute_var_ratio_perfect(SynImage& syn_image_loc,Part_rep& p_rep,MeshData<uint16_t>& input_img,AnalysisData& analysis_data){

    MeshData<float> variance;

    get_variance(input_img, variance, p_rep.pars);

    p_rep.pars.name = "perfect";

    MeshData<float> norm_grad_image;

    MeshData<float> gt_image;
    generate_gt_image(gt_image, syn_image_loc);

    p_rep.pars.lambda = -1;

    MeshData<float> grad_image;

    grad_image.initialize(gt_image.y_num,gt_image.x_num,gt_image.z_num,0);

    //generate_gt_norm_grad(norm_grad_image,syn_image,true,pars.dx,pars.dy,pars.dz);
    get_gradient_3D(p_rep, gt_image, grad_image);

//debug_write(grad_image,"grad_img");

MeshData<float> var_gt;

generate_gt_var(gt_image,var_gt,syn_image_loc,p_rep.pars);
//debug_write(var_gt,"var_gt");

//Part_rep p_rep;
//p_rep.pars = p_rep.pars;
p_rep.pars.name = "perfect";

p_rep.initialize(input_img.y_num,input_img.x_num,input_img.z_num);

PartCellStructure<float, uint64_t> pc_struct_perfect;
get_apr_perfect(input_img,grad_image,var_gt,p_rep,pc_struct_perfect,analysis_data);


MeshData<float> variance_u;
//need to down sample / then upsample variance
down_sample(var_gt,variance_u,
[](float x, float y) { return std::max(x,y); },
[](float x) { return x; },true);

std::vector<unsigned int> dims = {(unsigned int)var_gt.y_num,(unsigned int)var_gt.x_num,(unsigned int)var_gt.z_num};
const_upsample_img(var_gt,variance_u,dims);


//debug_write(variance,"vvar");

compare_var_func(pc_struct_perfect,var_gt,variance,analysis_data);

//debug_write(var_gt,"var_g2t");

}

void calc_information_content(SynImage syn_image,AnalysisData& analysis_data){
    //
    //  Bevan Cheeseman 2016
    //
    //  Information content of a synthetic image.
    //
    //  Computes as the integral of the normalized gradient normalized by voxel size.
    //

    MeshDataAF<float> info_x;
    MeshDataAF<float> info_y;
    MeshDataAF<float> info_z;

    syn_image.PSF_properties.type = "gauss_dx";
    syn_image.noise_properties.noise_type = "none";
    syn_image.global_trans.gt_ind = false;
    syn_image.PSF_properties.normalize = true;

    syn_image.generate_syn_image(info_x);

    info_x.transfer_from_arrayfire();
    info_x.free_arrayfire();

    //af::sync();
    af::deviceGC();

    syn_image.PSF_properties.type = "gauss_dy";
    syn_image.noise_properties.noise_type = "none";
    syn_image.global_trans.gt_ind = false;
    syn_image.PSF_properties.normalize = true;

    syn_image.generate_syn_image(info_y);

    info_y.transfer_from_arrayfire();
    info_y.free_arrayfire();

    //af::sync();
    af::deviceGC();

    syn_image.PSF_properties.type = "gauss_dz";
    syn_image.noise_properties.noise_type = "none";
    syn_image.global_trans.gt_ind = false;
    syn_image.PSF_properties.normalize = true;

    syn_image.generate_syn_image(info_z);

    info_z.transfer_from_arrayfire();
    info_z.free_arrayfire();

    //af::sync();
    af::deviceGC();

    af::array info_;

    info_x.transfer_to_arrayfire();
    info_y.transfer_to_arrayfire();
    info_z.transfer_to_arrayfire();

    info_x.af_mesh = sum(sum(sum(sqrt(pow(info_x.af_mesh,2)+pow(info_y.af_mesh,2)+pow(info_z.af_mesh,2)))));

    info_y.free_arrayfire();
    info_z.free_arrayfire();

    float info_content = info_x.af_mesh.scalar<float>()*((syn_image.sampling_properties.voxel_real_dims[0]*syn_image.sampling_properties.voxel_real_dims[1]*syn_image.sampling_properties.voxel_real_dims[2]));

    analysis_data.add_float_data("info",info_content);

}

#endif //PARTPLAY_ANALYZE_APR_H