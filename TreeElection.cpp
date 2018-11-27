#include <stdio.h>
#include "mpi.h"

int main( int argc, char *argv[] )
{
    int rank, size;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    int parent;
    int rightc;
    int leftc;
    if(rank!=0){
      parent=(rank-1)/2;
      rightc=2*rank+1;
      leftc=2*rank+2;
    }
   if(rightc>=size){
    rightc=-1;    
   }
   if(leftc>=size){
    leftc=-1;
   }
   
    MPI_Finalize();
    return 0;
}
