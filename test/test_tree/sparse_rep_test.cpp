//
// Created by Bevan Cheeseman 3.11.2016
//
#include "utils.h"
#include "tree_fixtures.hpp"
#include "../../src/data_structures/Tree/PartCellStructure.hpp"


TEST_F(CreateSmallTreeTest, LEVEL_ITERATOR_SMALL_TEST)
{
    
    //
    //  Sparse Particle Structure Test Cases
    //
    //
    
    PartCellStructure<float,uint64_t> pcell_test(particle_map);
    
    ASSERT_TRUE(compare_sparse_rep_with_part_map(particle_map,pcell_test));
    
    
}


TEST_F(CreateBigTreeTest, LEVEL_ITERATOR_BIG_TEST)
{
    
    PartCellStructure<float,uint64_t> pcell_test(particle_map);
    
    ASSERT_TRUE(compare_sparse_rep_with_part_map(particle_map,pcell_test));
    
}

TEST_F(CreateNarrowTreeTest, LEVEL_ITERATOR_NARROW_TEST)
{
    
    PartCellStructure<float,uint64_t> pcell_test(particle_map);
    
    ASSERT_TRUE(compare_sparse_rep_with_part_map(particle_map,pcell_test));
    
}

int main(int argc, char **argv) {
    
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    
}