/*˼�룺����һ����������뵽E:\\intput.txt�У����ļ��ж�����ֵ�����飬�����ݷ������
���̣�������ִ�п��ţ������0�����յ�����������Ȼ��ִ�п��ţ�����������E:\\intput.txt*/
#define MPICH_SKIP_MPICXX
#pragma comment (lib, "mpi.lib")
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
using namespace std;
#define length 100//�������鳤��

//���Ž�����Ԫ�غ���
void swap(int *data, int i, int j) {
  int temp = data[i];
  data[i] = data[j];
  data[j] = temp;
 }

//һ�˿��Ź��̺���
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
 
//���ŵĵݹ麯��
void quicksort(int *data, int start, int end) {
     if (end-start+1 < 2) return;
     int pivot = partition(data, start, end);
     quicksort(data, start, pivot);
     quicksort(data, pivot+1, end);
 }

//������
int main(int argc, char *argv[]) {
     MPI_Init(&argc, &argv);
     int rank, size;
	 fstream f;
     MPI_Comm_rank (MPI_COMM_WORLD, &rank);//��õ�ǰ����
     MPI_Comm_size (MPI_COMM_WORLD, &size);//�ܽ��̸���
     srand(time(0));
     int *data = (int*)malloc(sizeof(int)*length);
     int i;
	 int x;
	 //����һ�����������E:\\input.txt�ļ�
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
	 //��E:\\input.txt�ļ���������data����
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
	 //��������
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
	 //��������ִ�п���
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
		 //��data�������E:\\output.txt
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
         cout<<"OK:�鿴E:\\��txt�ļ�"<<endl;
	     /*for(int i=0;i<length;i++){
			  printf("%d",data[i]);
			  printf("\n");}*/
     }
     MPI_Finalize();
     return 0;
}