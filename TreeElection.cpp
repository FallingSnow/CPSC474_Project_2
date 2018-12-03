#include "config.hpp"
#include "mpi.h"
#include <stdio.h>
#include <vector>

using namespace std;

struct node {
    int rank = -1;
    bool msgReceived = false;
};

bool checkRcvs(vector<node *> x) {
    int count = 0;

    for (int i = 0; i < x.size(); i++) {

        // Check if this node has received a message
        if (!(x[i]->msgReceived)) {
            count++;

            // If 2 or more neighbors have not received a message, return false
            if (count >= 2) {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char *argv[]) {

    // Initialize MPI with process parameters
    MPI_Init(&argc, &argv);

    // Define our rank and size
    int rank, size;

    // Our parent
    node *parent;
    vector<node *> neighbors;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Print out program version on first node
    if (rank == 0) {
        fprintf(stdout, "%s Version %d.%d\n", argv[0], PROJECT_VERSION_MAJOR,
                PROJECT_VERSION_MINOR);
    }

    // If we are not the first process, lets add ourself to the neighbors vector
    if (rank != 0) {
        neighbors.push_back(new node);
        neighbors.back()->rank = (rank - 1) / 2;
    }

    // Defined ranks of children
    int rightc = 2 * rank + 1;
    int leftc = 2 * rank + 2;

    // Create right child
    if (rightc < size) {
        neighbors.push_back(new node);
        neighbors.back()->rank = rightc;
    }

    // Create left child
    if (leftc < size) {
        neighbors.push_back(new node);
        neighbors.back()->rank = leftc;
    }

    // Hold response acks from neighbors, currently set to neightbors.size()
    // -1's
    vector<int> number(neighbors.size(), -1);
    vector<node *> children;
    MPI_Request ireq;

    // tag 0 =parent msgs
    while (!checkRcvs(neighbors)) {
        // receive msg and set variables in node
        // add it to children vector
        for (int i = 0; i < neighbors.size(); i++) {

            // Nonblocking receive for neighbor[i]
            MPI_Irecv(&number[i], 1, MPI_INT, neighbors[i]->rank, 0,
                      MPI_COMM_WORLD, &ireq);

            if (number[i] != -1) {
                children.push_back(neighbors[i]);
                int temp = 0;
                MPI_Send(&temp, 1, MPI_INT, parent->rank, 0, MPI_COMM_WORLD);
                neighbors[i]->msgReceived = true;
            }
        }
    }

    int temp = 1;
    // TODO: used to select who your parent is not done yet
    for (int i = 0; i < neighbors.size(); i++) {
        if (!(neighbors[i]->msgReceived)) {
            parent = neighbors[i];
            break;
        }
    }

    // Although this shouldn't happen, in the event of a nullptr for parent,
    // we set parent to the last child
    if (parent == nullptr) {
        parent = children.back();
    }

    MPI_Isend(&temp, 1, MPI_INT, parent->rank, 0, MPI_COMM_WORLD, &ireq);
    temp = -1;
    MPI_Recv(&temp, 1, MPI_INT, parent->rank, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);

    if (temp == 1) {
        children.push_back(parent);
    }

    printf("my parent=%d\n", parent->rank);
    printf("my rank=%d\n", rank);

    // Deinit MPI
    MPI_Finalize();

    // Return process end code
    return 0;
}
