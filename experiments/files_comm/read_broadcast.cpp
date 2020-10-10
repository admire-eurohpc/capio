#include <fstream>
#include <iostream>
#include <mpi.h>

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}


int main(int argc, char** argv) {
    int rank, **matrix;
    MPI_Init(&argc, &argv);
    if (argc != 3) {
        std::cout << "input error: num of rows and num of columns needed" << std::endl;
        MPI_Finalize();
        return 1;
    }
    int num_rows = std::stoi(argv[1]);
    int num_cols = std::stoi(argv[2]);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    matrix = alloc_2d_int(num_rows, num_cols);
    std::ifstream file("output_file_" + std::to_string(rank) + ".txt");
    if (!file.is_open()) {
        std::cout << "error opening file" << std::endl;
        MPI_Finalize();
        return 1;
    }
    for (int i = 0; i < num_rows; ++i) {
        for(int j = 0; j < num_cols; ++j) {
            file >> matrix[i][j];
        }
    }
    file.close();
    std::ofstream output_file("output_file_read_broadcast_" + std::to_string(rank) + ".txt");
    for (int i = 0; i < num_rows; ++i) {
        for(int j = 0; j < num_cols; ++j) {
            output_file << matrix[i][j] << "\n";
        }
    }
    output_file.close();
    free(matrix[0]);
    free(matrix);
    MPI_Finalize();
    return 0;
}