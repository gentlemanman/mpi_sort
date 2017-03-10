/*思想：产生一组随机数输入到E:\\intput.txt中，从文件中读出赋值给数组，将数据分配给各
进程，各进程执行快排，最后在0进程收到各进程数据然后执行快排，将结果输出到E:\\intput.txt*/
#define MPICH_SKIP_MPICXX
#pragma comment (lib, "mpi.lib")
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
using namespace std;
#define length 100//定义数组长度

//快排交换两元素函数
void swap(int *data, int i, int j) {
  int temp = data[i];
  data[i] = data[j];
  data[j] = temp;
 }

//一趟快排过程函数
int partition(int *data, int start, int end) {
     if (start >= end) return 0;
     int pivotValue = data[start];
     int low = start;
     int high = end - 1;
     while (low < high) {
         while (data[low] <= pivotValue && low < end) low++;
         while (data[high] > pivotValue && high > start) high--;
         if (low < high) swap(data, low, high);
     }
     swap(data, start, high);
     return high;
 }
 
//快排的递归函数
void quicksort(int *data, int start, int end) {
     if (end-start+1 < 2) return;
     int pivot = partition(data, start, end);
     quicksort(data, start, pivot);
     quicksort(data, pivot+1, end);
 }

//主函数
int main(int argc, char *argv[]) {
     MPI_Init(&argc, &argv);
     int rank, size;
	 fstream f;
     MPI_Comm_rank (MPI_COMM_WORLD, &rank);//获得当前进程
     MPI_Comm_size (MPI_COMM_WORLD, &size);//总进程个数
     srand(time(0));
     int *data = (int*)malloc(sizeof(int)*length);
     int i;
	 int x;
	 //产生一组随机数存入E:\\input.txt文件
	 f.open("E:\\input.txt",ios::out);
	 if(f.fail())
     {
        cout<<"file problems!"<<endl;
        exit(1);
	 } 
	 for (i=0; i<length; i++){
		 f<<rand()<<endl;
	 }
     f.close();
	 //从E:\\input.txt文件读出存入data数组
	 f.open("E:\\input.txt",ios::in);
	 if(f.fail())
     {
        cout<<"file problems!"<<endl;
        exit(1);
	 } 
     for (i=0; i<length/size; i++){
		 f>>x;
         data[i]=x;
	 }
	 f.close();
	 /*for (i=0; i<length/size; i++){
          data[i] = rand();
	 }*/
     MPI_Status status;
	 //分配数据
     if (rank == 0) {
         for (i=1; i<size; i++)
             MPI_Recv(data+i*length/size, length/size, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
     }
     else {
         MPI_Send(data,  length/size,  MPI_INT,  0,  0,  MPI_COMM_WORLD);
     }

     int s;
     int localDataSize =  length;
     int pivot;
	 //各个进程执行快排
     for (s=size; s > 1; s /= 2) {
         if (rank % s == 0) {
             pivot = partition(data, 0, localDataSize);
             MPI_Send(data+pivot, localDataSize - pivot,MPI_INT, rank + s/2, 0, MPI_COMM_WORLD);
             localDataSize = pivot;
         }
         else if (rank % s == s/2) {
             MPI_Recv(data, length, MPI_INT, rank - s/2,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
         }
     }
	 quicksort(data, 0, localDataSize);
     if (rank == 0)
     {
		 //从data数组输出E:\\output.txt
         f.open("E:\\output.txt",ios::out);
         if(f.fail())
		 {
            cout<<"file problems!"<<endl;
            exit(1);
		 }
         for(int i=0;i<length;i++){
            f<<data[i]<<endl;
		 }
         f.close();
         cout<<"OK:查看E:\\的txt文件"<<endl;
	     /*for(int i=0;i<length;i++){
			  printf("%d",data[i]);
			  printf("\n");}*/
     }
     MPI_Finalize();
     return 0;
}