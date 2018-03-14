#include <iostream>
#include <mpi.h>
#include <chrono>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

int n,rank,size;

int indexOf(int x, int y){
  int a = x*n + y;
  return a;
}

int computeHeatOne(int n){

  float *prev = new float[(unsigned int) n * n];
  float *next = new float[(unsigned int) n * n];
  srand(time(NULL));

  for(int i = 0; i<n; i++){
    for(int j = 0; j<n; j++){
      prev[indexOf(i,j)] = rand();
      next[indexOf(i,j)] = 0;
    }
  }
 double t1 = MPI_Wtime();	
  int iterations = 0;
  
 while(iterations < 20){
  for(int i = 0; i<n; i++){
    for(int j = 0; j<n ; j++){

      if(i == 0){
	if(j == 0){
	  next[indexOf(0,0)] = (6*prev[indexOf(0,0)] + prev[indexOf(0,1)] + prev[indexOf(1,1)] + prev[indexOf(1,0)])/9;
	}
	else if(j == n-1)  next[indexOf(i,j)] = (prev[indexOf(i-1,j)] +  6 * prev[indexOf(i,j)]  + prev[indexOf(i-1,j-1)] + prev[indexOf(i,j-1)])/9;
	else
	  next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
      	
      }

      else if(i == n-1){
        if(j == 0){
          next[indexOf(n-1,0)] = (6*prev[indexOf(n-1,0)] + prev[indexOf(n-1,1)] + prev[indexOf(n-2,1)] + prev[indexOf(n-2,0)])/9;
        }
        else if(j == n-1)  next[indexOf(i,j)] = (prev[indexOf(i,j-1)] +  6 * prev[indexOf(i,j)]  + prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)])/9;
	else	    
	    next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i-1,j-1)]+ prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] )/9;
	  
      }

      else if(j == 0)	next[indexOf(i,j)] = (prev[indexOf(i-1,0)]+ prev[indexOf(i-1,1)] + 4*prev[indexOf(i,0)] + prev[indexOf(i,1)] + prev[indexOf(i+1,0)] + prev[indexOf(i+1,1)])/9;
      
      else if(j == n-1)	next[indexOf(i,j)] = (prev[indexOf(i-1, j-1)]+ prev[indexOf(i-1,j)]+ prev[indexOf(i,j-1)]+ 4*prev[indexOf(i,j)]+ prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)])/9; 		
      

      else	next[indexOf(i,j)] = (prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] + prev[indexOf(i,j-1)] + prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
      
       
    }
  
  }
  //   std::cout<<"done  "<<iterations<< std::endl;
	iterations++;
	for(int i =0; i<n; i++){
	for(int j = 0; j<n; j++) {prev[indexOf(i,j)] = next[indexOf(i,j)];
  				  next[indexOf(i,j)] = 0;		}	
	}
 }

   double t2 = MPI_Wtime();
  std::cerr<<t2-t1<<"s\n";	
  delete [] prev;
  delete [] next;
  return 0;
}

void synchronize(float *array, int rank, int size, int a){
 if(rank == 0){
    
    MPI_Request request;
    MPI_Status status;
    MPI_Irecv(&array[indexOf(a+1,0)], n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD, &request);
    MPI_Send(&array[indexOf(a,0)], n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD);
    MPI_Wait( &request, &status);
  //    std::cout<<rank<<" - "<<"Done sending and receiving rank1 arrays" <<std::endl;
  }

 
  
  else if(rank == size-1){
    
    MPI_Request request;
    MPI_Status status;
    MPI_Irecv(&array[indexOf(0,0)], n , MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD, &request);
    MPI_Send(&array[indexOf(1,0)], n, MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD);
    MPI_Wait(&request, &status);
//    std::cout<<rank<<"-"<<"Done receiving and sending  arrays" <<std::endl;

  }
  else{
    
    MPI_Request request1,request2;
    MPI_Status status1,status2;
  
    MPI_Irecv(&array[indexOf(0,0)], n , MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD, &request1);
    MPI_Irecv(&array[indexOf(a+1,0)], n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD, &request2);
    MPI_Send(&array[indexOf(1,0)], n, MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD);
    MPI_Send(&array[indexOf(a,0)], n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD);
    MPI_Wait( &request1, &status1);
    MPI_Wait( &request2, &status2);
    //std::cout<<rank<<"-"<<"Done receiving and sending all other arrays" <<std::endl;
  }
}


int computeHeatGreater(int n, int start, int end){
  
  int a = end - start;
  float *prev = new float[(unsigned int) (a+2) * n];
  float *next = new float[(unsigned int) (a+2) * n];

  //std::cout<<rank<<"-"<<"Done declaring arrays" <<std::endl;

  srand(time(NULL));

   for(int i = 0; i<a+2; i++){
    for(int j = 0; j<n; j++){
      prev[indexOf(i,j)] = rand();
      next[indexOf(i,j)] = 0;
    }
    }

  MPI_Barrier(MPI_COMM_WORLD);
  
  double t1 = MPI_Wtime();
  synchronize(prev, rank,size, a);

  int iterations = 0;


  while(iterations < 20){

     synchronize(prev, rank , size,a); 

	for(int i = 1; i < a+1;i++){
  	for(int j = 0; j < n; j++){
         	if(rank == 0){
		  	if((i == 1) &&(j == 0))	  next[indexOf(i,j)] = (6*prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j+1)] + prev[indexOf(i+1,j)])/9;

			else if((i == 1) &&(j == n-1))	next[indexOf(i,j)] = (prev[indexOf(i,j-1)] +  6 * prev[indexOf(i,j)] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)])/9;
			else if(i == 1)	 next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;			

			else if((j == 0))
			 	next[indexOf(i,j)] = (prev[indexOf(i-1,j)]+ prev[indexOf(i-1,j+1)] + 4*prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)])/9;
			
			else if(j == n-1)
				next[indexOf(i,j)] = (prev[indexOf(i-1, j-1)]+ prev[indexOf(i-1,j)]+ prev[indexOf(i,j-1)]+ 4*prev[indexOf(i,j)]+ prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)])/9; 	
			
			else
			next[indexOf(i,j)] = (prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] + prev[indexOf(i,j-1)] + prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
				
					
 		}
		else if(rank == size-1){
			if((i == a)&&(j == 0))          next[indexOf(i,j)] = (6*prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i-1,j+1)] + prev[indexOf(i-1,j)])/9;
			else if((i == a)&&(j == n-1))	next[indexOf(i,j)] = (prev[indexOf(i,j-1)] +  6 * prev[indexOf(i,j)]  + prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)])/9;
			else if(i == a)		    next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i-1,j-1)]+ prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] )/9;
			else if(j == 0)		next[indexOf(i,j)] = (prev[indexOf(i-1,j)]+ prev[indexOf(i-1,j+1)] + 4*prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)])/9;
			else if(j == n-1)	next[indexOf(i,j)] = (prev[indexOf(i-1, j-1)]+ prev[indexOf(i-1,j)]+ prev[indexOf(i,j-1)]+ 4*prev[indexOf(i,j)]+ prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)])/9; 	


			else 			next[indexOf(i,j)] = (prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] + prev[indexOf(i,j-1)] + prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
		
		}	
		else{
			if(j == 0)		next[indexOf(i,j)] = (prev[indexOf(i-1,j)]+ prev[indexOf(i-1,j+1)] + 4*prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)])/9;
			else if(j == n-1)	next[indexOf(i,j)] = (prev[indexOf(i-1, j-1)]+ prev[indexOf(i-1,j)]+ prev[indexOf(i,j-1)]+ 4*prev[indexOf(i,j)]+ prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)])/9;
			else 			next[indexOf(i,j)] = (prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] + prev[indexOf(i,j-1)] + prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
		}			  	
		
	
	}
	}
    
//	if(rank == 1) std::cout<<"done-- before-for loop"<<iterations<< std::endl;
     iterations++;   
    //MPI_Barrier(MPI_COMM_WORLD);
	 for(int i =1; i<a+1; i++){
	for(int j = 0; j<n; j++) prev[indexOf(i,j)] = next[indexOf(i,j)];}
    //    if(rank == 1) std::cout<<"done---"<<iterations<< std::endl;
   } 
     MPI_Barrier(MPI_COMM_WORLD);
     double t2 = MPI_Wtime();
     if(rank == 0)   std::cerr<<t2-t1<<"s\n";

  delete [] prev;
  delete [] next;
  /*delete [] send1st;
  delete [] send2nd;
  delete [] rec1st;
  delete [] rec2nd;*/

  return 0;
}

int computeHeat(int n, int rank, int size, int scaling){
 if(scaling == 0){
  if(size == 1)    computeHeatOne(n);
  else{
  int startx = rank * (n/size);
  int endx = (rank + 1) * (n/size);
  if(rank == size-1)   endx = n;
  computeHeatGreater(n, startx, endx); 
  }
 }
 else {
  if(size == 1)    computeHeatOne(n);
  else{
  int startx = 0;
  int endx = n;
  computeHeatGreater(n, startx, endx);
  } 
 }
   
  return 0;
}


int main (int argc, char* argv[]){

  if(argc < 3){
    std::cerr<<"usage: mpirun"<<argv[0]<<" n "<<" scaling 0-strong 1-weak "<<std::endl;
    return -1;
  }

  
  int s = atoi(argv[1]);
  int scaling = atoi(argv[2]);
  if(scaling == 0){
  if(s == 1)		n = 15800 ;
  else if(s == 4)  	n =  31000;
  else if(s == 10) 	n = 50000;
  else if(s == 80)      n = 111000;}
  else{
  if(s == 500) 	n = 11000;
  else if(s == 1) n= 15800;
  else if(s == 2) n = 22000;
  }
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
//  std::cout<<size<<std::endl;
  computeHeat(n, rank, size, scaling);
  MPI_Barrier(MPI_COMM_WORLD);  
  MPI_Finalize();

  return 0;
  
}
