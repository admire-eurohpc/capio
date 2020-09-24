#include <iostream>
#include <mpi.h>
#include "../../../capio_mpi/capio_mpi.hpp"
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
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (argc != 3) {
        std::cout << "input error: number of consumers and config file needed " << std::endl;
        MPI_Finalize();
        return 0;
    }
    int num_cons = std::stoi(argv[1]);
    std::string config_path = argv[2];
    capio_mpi capio(num_cons, false, rank, config_path);
    std::cout << "writer " << rank << "created capio object" << std::endl;
    if (NUM_ELEM % size == 0) {
        data = new int[NUM_ELEM];
        initialize(data, NUM_ELEM, rank);
        capio.capio_all_reduce(data, nullptr, NUM_ELEM, MPI_INT, sum, rank);
        initialize(data, NUM_ELEM, rank + 1);
        capio.capio_all_reduce(data, nullptr, NUM_ELEM, MPI_INT, sum, rank);
        free(data);
    }
    std::cout << "writer " << rank << "ended " << std::endl;
    MPI_Finalize();
    return 0;
}