#include <iostream>
#include <mpi.h>
#include "../../../capio_ordered/capio_ordered.hpp"
#include "../../common/utils.hpp"

int const NUM_ELEM = 100;

void sum(void* input_data, void* output_data, int* count, MPI_Datatype* data_type) {
    int* input = (int*)input_data;
    int* output = (int*)output_data;

    for(int i = 0; i < *count; i++) {
        output[i] += input[i];
    }
}

int main(int argc, char** argv) {
    int* data;
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (argc != 2) {
        std::cout << "input error: config file needed " << std::endl;
        MPI_Finalize();
        return 0;
    }
    std::string config_path = argv[1];
    capio_ordered capio(false, true, rank, config_path);
    std::cout << "writer " << rank << "created capio object" << std::endl;
    data = new int[NUM_ELEM];
    initialize(data, NUM_ELEM, rank);
    capio.capio_reduce(data, nullptr, NUM_ELEM, MPI_INT, sum, 0);
    initialize(data, NUM_ELEM, rank + 1);
    capio.capio_reduce(data, nullptr, NUM_ELEM, MPI_INT, sum, 0);
    free(data);
    std::cout << "writer " << rank << "ended " << std::endl;
    MPI_Finalize();
    return 0;
}