#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_value = rank + 1; // 每个进程的本地值

    int global_sum; // 用于存放全局和

    // 执行全局归约操作，将各进程的 local_value 求和，结果保存在 global_sum 中
    MPI_Allreduce(&local_value, &global_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // 打印每个进程的局部值和全局和
    std::cout << "Process " << rank << ": Local Value = " << local_value << ", Global Sum = " << global_sum << std::endl;

    MPI_Finalize();

    return 0;
}
