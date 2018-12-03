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

    // Hold messages from neighbors, currently set to neighbors.size() (count)
    // -1's (fill)
    vector<int> recvMsgs(neighbors.size(), -1);
    vector<node *> children;
    MPI_Request ireq;

    // tag 0 =parent msgs
    while (!checkRcvs(neighbors)) {
        // receive msg and set variables in node
        // add it to children vector
        for (int i = 0; i < neighbors.size(); i++) {

            // Nonblocking receive for neighbor[i]
            int recvCode = MPI_Irecv(&recvMsgs[i], 1, MPI_INT, neighbors[i]->rank,
                                    0, MPI_COMM_WORLD, &ireq);

            // Check for recv error
            if (recvCode != MPI_SUCCESS) {
                printf("Unable to listen for neighbors");
                return 1;
            }

            if (recvMsgs[i] != -1) {
                children.push_back(neighbors[i]);
                int sendBuffer = 0;
                int sendCode = MPI_Send(&sendBuffer, 1, MPI_INT, parent->rank, 0,
                                       MPI_COMM_WORLD);
                if (sendCode != MPI_SUCCESS) {
                    printf("Unable to send to parent");
                    return 1;
                }
                neighbors[i]->msgReceived = true;
            }
        }
    }

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


    int sendBuffer = 1;
    int ackCode = MPI_Isend(&sendBuffer, 1, MPI_INT, parent->rank, 0, MPI_COMM_WORLD, &ireq);
    if (ackCode != MPI_SUCCESS) {
        printf("Unable to send ack");
        return 1;
    }


    int recvBuffer = -1;
    int recvAckCode = MPI_Recv(&recvBuffer, 1, MPI_INT, parent->rank, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    if (recvAckCode != MPI_SUCCESS) {
        printf("Unable to receive ack");
        return 1;
    }

    if (recvBuffer == 1) {
        children.push_back(parent);
    }

    printf("my parent=%d\n", parent->rank);
    printf("my rank=%d\n", rank);

    if (MPI_Request_free(&ireq) != MPI_SUCCESS) {
        printf("Unable to free MPI_Request");
        return 1;
    }

    // Deinit MPI
    MPI_Finalize();

    // Return process end code
    return 0;
}
