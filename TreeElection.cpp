#include <stdio.h>
#include "mpi.h"
#include <vector>
struct node{
    int rank=-1;
    bool parent =False;
    bool msgRecived= False;
    bool child =False;
   
}
bool checkRcvs(vector<node*> x){
    int count=0;
    for(int i=0; i< x.size(); i++){
        if(!(x[i]->msgRecived)){
            count++;   
        }
    }
    
}
int main( int argc, char *argv[] )
{
    int rank, size;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    vector<node*> neighbors; 
    bool rightReceive= False;
    bool leftReceive= False;
    if(rank!=0){
        neighbors.push_back(new node);
        neighbors.back()->rank= (rank-1)/2;
    }
    
    int rightc=2*rank+1;
    int leftc=2*rank+2;
    
    if(rightc<size){
        neighbors.push_back(new node);
        neighbors.back()->rank=rightc;     
    }
    if(leftc<size){
        neighbors.push_back(new node);
        neighbors.back()->rank=leftc;    
    }
    while(!rightReceive && !leftReceive){
        //receive msg   
    }
    if(leftc==-1 && rightc=-1){
        //send msg to parent       
    }
    
    MPI_Finalize();
    return 0;
}