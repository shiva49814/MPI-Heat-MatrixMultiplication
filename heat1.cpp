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
        else if(j == n-1)  next[indexOf(i,j)] = (prev[indexOf(i-1,j)] +  6 * prev[indexOf(i,j)]  + prev[indexOf(i-1,j-1)] + prev[indexOf(i,j-1)])/9;
	else	    
	    next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i-1,j-1)]+ prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] )/9;
	  
      }

      else if(j == 0)	next[indexOf(i,j)] = (prev[indexOf(i-1,0)]+ prev[indexOf(i-1,1)] + 4*prev[indexOf(i,0)] + prev[indexOf(i,1)] + prev[indexOf(i+1,0)] + prev[indexOf(i+1,1)])/9;
      
      else if(j == n-1)	next[indexOf(i,j)] = (prev[indexOf(i-1, j-1)]+ prev[indexOf(i-1,j)]+ prev[indexOf(i,j-1)]+ 4*prev[indexOf(i,j)]+ prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)])/9; 		
      

      else	next[indexOf(i,j)] = (prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] + prev[indexOf(i,j-1)] + prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
      
       
    }
  
  }
  //   std::cout<<"done  "<<iterations<< std::endl;
	for(int i =0; i<n; i++){
	for(int j = 0; j<n; j++) {prev[indexOf(i,j)] = next[indexOf(i,j)];
  				  next[indexOf(i,j)] = 0;		}	
	}
 }
	
  delete [] prev;
  delete [] next;
  return 0;
}

void synchronize(float *array, int rank, int size){
 if(rank == 0){
    
    MPI_Request request[2];
    MPI_Status status[2];
    MPI_Isend(send2nd, n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(rec2nd, n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Waitall(2, request, status);
  }
  if(rank == 1) std::cout<<"Done sending and receiving rank1 arrays" <<std::endl;

 
  
  else if(rank == size-1){
    
    MPI_Request request[2];
    MPI_Status status[2];
    MPI_Isend(send1st, n, MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(rec1st, n , MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Waitall(2, request, status);
    std::cout<<"Done receiving and sending  arrays" <<std::endl;

  }
  else{
    
    MPI_Request request[4];
    MPI_Status status[4];
    MPI_Isend(send1st, n, MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Isend(send2nd, n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Irecv(rec1st, n , MPI_FLOAT, rank-1, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Irecv(rec2nd, n, MPI_FLOAT, rank+1, 0, MPI_COMM_WORLD, &request[4]);
    MPI_Waitall(4, request, status);
  }
}


int computeHeatGreater(int n, int start, int end){
  
  int a = end - start;
  float *prev = new float[(unsigned int) a * n];
  float *next = new float[(unsigned int) a * n];
  /*float *send1st = new float[n];
  float *send2nd = new float[n];
  float *rec1st = new float[n];
  float *rec2nd = new float[n];*/
  if(rank == 1) std::cout<<"Done declaring arrays" <<std::endl;

  srand(time(NULL));

   for(int i = 0; i<n; i++){
    for(int j = 0; j<a; j++){
      prev[indexOf(i,j)] = rand();
      next[indexOf(i,j)] = 0;
    }
    }
  if(rank == 1) std::cout<<"Done initialising prev and next arrays" <<std::endl;

  MPI_Barrier(MPI_COMM_WORLD);
  /*for(int i =0; i< n; i++){
  send1st[i] = 0; send2nd[i] = 0; rec1st[i] = 0; rec2nd[i] = 0;
  }
  if(rank == 1) std::cout<<"Done initialising four arrays" <<std::endl;*/
  synchronize(prev, rank, size);
  
  MPI_Barrier(MPI_COMM_WORLD);
  std::chrono::time_point<std::chrono::system_clock> start_time, end_time;

    start_time = std::chrono::system_clock::now();

  int iterations = 0;


while(iterations < 20){


 

                  

	for(int i = 0; i < n;i++){
  	for(int j = 0; j < a; j++){
        //Computing first row elements.	
          	if(i == 0){
		 if(j == 0){
			if(rank == 0)     next[indexOf(0,0)] = (6*prev[indexOf(0,0)] + prev[indexOf(0,1)] + prev[indexOf(1,1)] + prev[indexOf(1,0)])/9;		
			else		  next[indexOf(0,0)] = (rec1st[i] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + rec1st[i+1]+ prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
		 }
		 // if(rank == 1) std::cout<<"Done declaring arrays1" <<std::endl;
		 if(j == a-1){
	     	 	if(rank == size-1)   next[indexOf(i,j)] = (prev[indexOf(i-1,j)] +  6 * prev[indexOf(i,j)]  + prev[indexOf(i-1,j-1)] + prev[indexOf(i,j-1)])/9;
			else 		     next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + rec2nd[i] + prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)] + rec2nd[i+1] )/9;
		 }
		   	
		 else    next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9; 
		 if(rank == 1) std::cout<<i<<"  "<<j <<std::endl;
		

	  	}
        //Computing last row elements.
		else if(i == n-1){
		 if(j == 0){
                     	if(rank == 0)     next[indexOf(n-1,0)] = (6*prev[indexOf(n-1,0)] + prev[indexOf(n-1,1)] + prev[indexOf(n-2,1)] + prev[indexOf(n-2,0)])/9;
			else		  next[indexOf(i,j)] = (rec1st[i] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i-1,j-1)]+ rec1st[i-1] + prev[indexOf(i-1,j+1)] )/9;	 			    }
		
		 if(j == a-1){
			if(rank = size-1)  next[indexOf(i,j)] = (prev[indexOf(i-1,j)] +  6 * prev[indexOf(i,j)]  + prev[indexOf(i-1,j-1)] + prev[indexOf(i,j-1)])/9;
			else              next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + rec2nd[i] + prev[indexOf(i-1,j-1)]+ prev[indexOf(i-1,j)] + rec2nd[i-1] )/9;	     
		 }

		 else 	 next[indexOf(i,j)] = (prev[indexOf(i,j-1)] + 4 * prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i-1,j-1)]+ prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] )/9;
		 if(rank == 1) std::cout<<i<<"  "<<j <<std::endl;

		}
                   		
	//Computing 1st column of all ranks
      		else if((j == 0)&&(i != 0)&&(i != n-1)){
			if(rank == 0)   
				next[indexOf(i,j)] = (prev[indexOf(i-1,0)]+ prev[indexOf(i-1,1)] + 4*prev[indexOf(i,0)] + prev[indexOf(i,1)] + prev[indexOf(i+1,0)] + prev[indexOf(i+1,1)])/9;		
			else
			        next[indexOf(i,j)] = (rec1st[i-1] + prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] + rec1st[i] + prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + rec1st[i+1] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
			if(rank == 1) std::cout<<i<<"  "<<j <<std::endl;

		}		
 	//Computing last columns of all ranks
		else if((j == a-1)&&(i != 0)&&(i != n-1)){	
			if(rank==size-1)  
				next[indexOf(i,j)] = (prev[indexOf(i-1, j-1)]+ prev[indexOf(i-1,j)]+ prev[indexOf(i,j-1)]+ 4*prev[indexOf(i,j)]+ prev[indexOf(i+1,j-1)]+ prev[indexOf(i+1,j)])/9; 				else 	
				next[indexOf(i,j)] = (prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)] + rec2nd[i-1] + prev[indexOf(i,j-1)] + prev[indexOf(i,j)] + rec2nd[i] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)] + rec2nd[i+1] )/9;
			if(rank == 1) std::cout<<i<<"  "<<j <<std::endl;

		}
			  	
		
	//Computing all other indexes
		else {
		next[indexOf(i,j)] = (prev[indexOf(i-1,j-1)] + prev[indexOf(i-1,j)] + prev[indexOf(i-1,j+1)] + prev[indexOf(i,j-1)] + prev[indexOf(i,j)] + prev[indexOf(i,j+1)] + prev[indexOf(i+1,j-1)] + prev[indexOf(i+1,j)] + prev[indexOf(i+1,j+1)] )/9;
		if(rank == 1) std::cout<<i<<"  "<<j <<std::endl;

		}
	}
	}
    
	if(rank == 1) std::cout<<"done-- before-for loop"<<iterations<< std::endl;
     iterations++;   
    //MPI_Barrier(MPI_COMM_WORLD);
	 for(int i =0; i<a; i++){
	for(int j = 0; j<n; j++) prev[indexOf(i,j)] = next[indexOf(i,j)];}
    //    if(rank == 1) std::cout<<"done---"<<iterations<< std::endl;
}

 end_time = std::chrono::system_clock::now();
 std::chrono::duration<double> elapsed_seconds = end_time-start_time;
//  if(rank == 0) 
   std::cerr<< elapsed_seconds.count() << "s\n";


  delete [] prev;
  delete [] next;
  delete [] send1st;
  delete [] send2nd;
  delete [] rec1st;
  delete [] rec2nd;

  return 0;
}

int computeHeat(int n, int rank, int size){
  int startx = rank * (n/size);
  int endx = (rank + 1) * (n/size);
  if(rank = size-1)   endy = n;

  if(size == 1){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    computeHeatOne(n);
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr<< elapsed_seconds.count() << "s\n";
  }
  else{
    computeHeatGreater(n, starty, endy); 
  }

  return 0;
}


int main (int argc, char* argv[]){

  if(argc < 2){
    std::cerr<<"usage: mpirun"<<argv[0]<<" n "<<std::endl;
    return -1;
  }

  
  n = atoi(argv[1]);
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  computeHeat(n, rank, size);
  MPI_Barrier(MPI_COMM_WORLD);  
  MPI_Finalize();

  return 0;
  
}
