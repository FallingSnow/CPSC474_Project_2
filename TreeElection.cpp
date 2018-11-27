#include <stdio.h>
#include "mpi.h"
#include <vector>

using namespace std;

struct node{
    int rank=-1;   
    bool msgRecived= false;
};
bool checkRcvs(vector<node*> x){
    int count=0;
      
    for(int i=0; i< x.size(); i++){
        if(!(x[i]->msgRecived)){
            count++;
            if(count>=2){
                return false;   
            }
        }
        
    }
    return true;
}
int main( int argc, char *argv[] )
{
    int rank, size;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    vector<node*> neighbors; 
    bool rightReceive= false;
    bool leftReceive= false;
    node * parent;
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
    vector<int> number(neighbors.size(), -1);
    vector<node*> children;
    //tag 0 =parent msgs
    MPI_Request ireq;
    while(!checkRcvs){
        //receive msg and set variables in node
        //remove the node from whom you recieved a message form neighbors
        //add it to children vector
        for(int i = 0; i<neighbors.size(); i++){
            MPI_Ircv(&number[i], 1, MPI_INT, neighbors[i]->rank, 0, MPI_COMM_WORLD, &ireq);
            if(number[i]!=-1){
                children.push_back(neighbors[i]);   
            }
        }
    }
    
        
    for(int i=0; i<neighbors.size(); i++){//used to select who your parent is not done yet
        if(!(neighbors[i]->msgRecived)){
            parent=neighbors[i];
            neighbors.erase(i);
        }
    }
    
    MPI_Finalize();
    return 0;
}
