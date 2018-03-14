#include <iostream>
#include <mpi.h>
#include <chrono>
#include <time.h>
//#include <conio>
#include <math.h>

int n, a;

int indexOf(int x, int y){
  int val = x*a + y;
  return val;
}

int computeOne(long n){
 float *Xprev = new float[(long) n];
 float *Xnext = new float[(long) n];
 float *A = new float[(long) n * n];
  srand(time(NULL));
 int iterations = 0;

 for(long i = 0; i<n; i++)	Xprev[i] = rand();
 for(long i = 0; i<n; i++)	Xnext[i] = 0;
 for(long i = 0; i<n;i++){
	for(long j = 0; j<n; j++) 	A[indexOf(i,j)] = rand();
 }

 double t1 = MPI_Wtime();
 
 while(iterations<20){
  for(long i = 0; i<n; i++){
    for( long j = 0; j<n ; j++)  
    { if(j==0) Xnext[i] = A[indexOf(i,j)]*Xprev[j];
    Xnext[i] += A[indexOf(i,j)]*Xprev[j];
    }
  }	
   iterations++;
   for(long i = 0; i<n; i++){Xprev[i] = Xnext[i];	Xnext[i] = 0;}
   
 }
 double t2 = MPI_Wtime();
 std::cerr<<t2-t1<<"s\n";
 delete []  Xprev;
 delete [] Xnext;
 delete [] A;
 return 0;
}



void computeGreater(int n, int rank, int size, int sched){
 float *Xprev, *A, *Xnext, *Xsend;
 int root = sqrt(size);
 //std::cout << root <<std::endl;
 a = n/root;
 if(sched == 1)	a = n;
 srand(time(NULL));
 
 A = new float[(long) a * a];
 //std::cout<<rank <<"  malloced A \n";

 
 for(int i = 0; i<a; i++){
  for(int j = 0; j<a; j++)  A[indexOf(i,j)]= rand();
 }
// std::cout<<rank <<"  Initialissed A arrays \n";

 Xprev = new float[a];
 Xsend = new float[a];
 Xnext = new float[a]; 
 for(int j = 0; j<a; j++) {Xsend[j] = 0; Xprev[j] = 0; Xnext[j] = 0;}
 //std::cout<<rank <<"  Initialissed all three arrays \n"; 

 //Creating the Xarray in the topmost row of nodes and sending it to the next root-1 nodes in the same column.
 if(rank /root ==0){
   for(int i = 0; i < a; i++)  Xprev[i] = rand();
 }
 //std::cout<<rank <<"  Created the  X array \n";

 
 double t1 = MPI_Wtime();

 int iterations = 0;

 
 while(iterations < 20){
 
  //Send the X aray to all nodes in the same column..------------------------------------------------------
 if(rank/root ==0){
   for(int i = root + rank ; i<size; i = i+root)   MPI_Send(Xprev, a, MPI_FLOAT, i, 0, MPI_COMM_WORLD); 
  //std::cout<<rank <<"  Sent the X array \n";
 }
  //Receiving the X array from the topmost node in the column.--------------------------------------------------------
 else     MPI_Recv(Xprev, a, MPI_FLOAT, rank%root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  //std::cout<<rank <<"  Received the X array \n";}

  //Calculation of sum in each node----------------------------------------------------------------------------------------------
  for(int i = 0; i< a; i++){
    for(int j = 0; j < a; j++){ if(j == 0) Xnext[i] = A[indexOf(i,j)]*Xprev[j];
 				     else Xnext[i] +=  A[indexOf(i,j)]*Xprev[j];}	
  }
  
 //std::cout<<rank <<"  Calculation Done \n";

  iterations++;
  //Reduction Row-wise-----------------------------------------------------------------------------------------------------------
  int r = (rank/root);
  int first = r * root;
  //if( rank != first)  
  MPI_Reduce(Xnext, Xsend, a, MPI_FLOAT, MPI_SUM, first, MPI_COMM_WORLD);
  //else                  MPI_Reduce(Xnext, Xsend, a, MPI_FLOAT, MPI_SUM, first, MPI_COMM_WORLD);

  std::cout<<rank <<"  Reduction done \n";

  //Sending the Xsend value to top row elements--------------------------------------------------------------------------------------
  if(rank == 0) {Xprev = Xsend;
   //std::cout<<rank <<"  Xpre = Xsend \n"; 
  } 
  else if((rank/root > 0)&&(rank%root == 0))
   {
    int send = rank/root;
     //std::cout<<rank <<"  Rooooot \n";

    MPI_Send(Xsend, a, MPI_FLOAT, send, 0, MPI_COMM_WORLD);
    std::cout<<rank <<" sending the X array to "<< send<<"\n";
  
   }
  //Receiving the values from first elements of row in the corresponding column------------------------------------------------------
   else if((rank/root == 0) && (rank !=0)){
   //std::cout<<rank <<"  Receiving \n";

   int recv = rank*root ;
   MPI_Recv(Xprev, a, MPI_FLOAT, recv, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
   //std::cout<<rank <<" Received the Xarray \n";
   }
//std::cout<<iterations<<"=======done"<<rank<<"\n";
}
 
 MPI_Barrier(MPI_COMM_WORLD);
 double t2 = MPI_Wtime();

 if(rank == 0){
    std::cerr<<t2-t1 << "s\n";
  }

 delete [] A;
 delete [] Xprev;
 delete [] Xnext;
 delete [] Xsend;
}




int main (int argc, char* argv[]){

  if(argc < 3){
    std::cerr<<"usage: mpirun"<<argv[0]<<" n "<<" SCHED" <<std::endl;
    return -1;
  }

  int rank,size;  
//  int root = sqrt(size);
  //std::cout << root <<std::endl;
 
  int scaling = atoi(argv[2]);
  int d = atoi(argv[1]);
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if(scaling == 0){
  if(d == 1)            n = 15800 ;
  else if(d == 4)	n =  31000;
  else if(d == 20)	n = 50000;
  else if(d == 80)	n = 111400;}
  else{
  if(d == 1)  n = 15800;
  else if(d == 2) n= 22000;
  else if(d == 4) n = 33000;
  }

  if(size == 1)		computeOne(n);
  else 			computeGreater(n, rank, size, scaling);
  MPI_Finalize();

  return 0;
  
}
