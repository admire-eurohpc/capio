#include <iostream>
#include <mpi.h>
#include "../../capio_mpi/capio_mpi.hpp"

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}




int main(int argc, char** argv) {
    int size, rank, **matrix;
    MPI_Init(&argc, &argv);
    if (argc != 4) {
        std::cout << "input error: num of rows, num of columns and config file needed" << std::endl;
        MPI_Finalize();
        return 1;
    }
    int num_rows = std::stoi(argv[1]);
    int num_cols = std::stoi(argv[2]);
    std::string config_path(argv[3]);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    capio_mpi capio(true, false, rank, config_path);
    matrix =  alloc_2d_int(num_rows, num_cols);
    capio.capio_all_to_all(nullptr, num_rows * num_cols / size, matrix[0]);
    std::ofstream output_file("output_file_capio_all_to_all_" + std::to_string(rank) + ".txt");
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            output_file << matrix[i][j] << " ";
        }
        output_file << "\n"; // vs std::endl
    }
    output_file.close();
    free(matrix[0]);
    free(matrix);
    MPI_Finalize();
    return 0;
}