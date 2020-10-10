#include <fstream>
#include <iostream>
#include <mpi.h>


int main(int argc, char** argv) {
    int rank, size, *matrix;
    MPI_Init(&argc, &argv);
    if (argc != 3) {
        std::cout << "input error: num of rows and num of columns needed" << std::endl;
        MPI_Finalize();
        return 1;
    }
    int num_rows = std::stoi(argv[1]);
    int num_cols = std::stoi(argv[2]);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    std::ifstream file("output_file_" + std::to_string(rank) + ".txt");
    if (! file.is_open()) {
        std::cout << "error opening file" << std::endl;
        MPI_Finalize();
        return 1;
    }
    matrix = new int[num_rows * num_cols * size];


    for (int i = 0; i < num_rows * num_cols; ++i) {
        file >> matrix[i + rank * (num_rows * num_cols)];
    }

    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, matrix, num_rows * num_cols, MPI_INT, MPI_COMM_WORLD);

    std::ofstream output_file("output_all_gather_" + std::to_string(rank) + ".txt");
    for (int i = 0; i < num_rows * size; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            output_file << matrix[i * num_cols + j] << " ";
        }
        output_file << "\n"; // vs std::endl
    }
    output_file.close();
    file.close();
    free(matrix);
    MPI_Finalize();
    return 0;
}