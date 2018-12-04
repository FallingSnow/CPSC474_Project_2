#include "config.hpp"
#include "mpi.h"
#include <stdio.h>
#include <unistd.h>
#include <vector>

using namespace std;

struct node {
    int rank = -1;
};

int main(int argc, char *argv[]) {

    // Initialize MPI with process parameters
    MPI_Init(&argc, &argv);

    // Define our rank and size
    int rank, size;

    // Our parent
    node *parent = nullptr;
    vector<node *> neighbors;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Print out program version on first node
    if (rank == 0) {
        printf("%s Version %d.%d\n", argv[0], PROJECT_VERSION_MAJOR,
               PROJECT_VERSION_MINOR);
    }

    // If we are not the first process, lets add ourself to the neighbors vector
    if (rank != 0) {
        neighbors.push_back(new node {(rank - 1) / 2});
    }

    // Defined ranks of children
    int rightc = 2 * rank + 1;
    int leftc = 2 * rank + 2;

    // Create right child
    if (rightc < size) {
        neighbors.push_back(new node {rightc});
    }

    // Create left child
    if (leftc < size) {
        neighbors.push_back(new node {leftc});
    }

    vector<node *> children;
    printf("RANK: %d | # Neighbors: %zu\n", rank, neighbors.size());

    // If we only have 1 neighbor, we know it is our parent
    if (neighbors.size() == 1) {

        // Set our only neighbor as the parent
        parent = neighbors.back();
        int sendBuffer = 0;

        printf("RANK: %d | Sending \"%d\" to parent [%d]!\n", rank, sendBuffer,
               parent->rank);

        // Inform our parent that he is our parent
        int code =
            MPI_Send(&sendBuffer, 1, MPI_INT, parent->rank, 0, MPI_COMM_WORLD);
        printf("RANK: %d | Sent \"%d\" to parent [%d]!\n", rank, sendBuffer,
               parent->rank);

        // Check if send was successful
        if (code != MPI_SUCCESS) {
            printf("Unable to send to parent");
            return 1;
        }

    } else {
        // Create an array of requests and a message buffer
        MPI_Request requests[neighbors.size()];
        int msgs[neighbors.size()] = {-1};
        for (size_t i = 0; i < neighbors.size(); i++) {
            int recvCode = MPI_Irecv(&msgs[i], 1, MPI_INT, neighbors[i]->rank,
                                     0, MPI_COMM_WORLD, &requests[i]);

            // Check for recv error
            if (recvCode != MPI_SUCCESS) {
                printf("Unable to listen for neighbors");
                return 1;
            }
        }

        // Total # of complete recv's
        int total_completed = 0;
        // Number of completed recv's in a single loop
        int completed = 0;
        int completed_indices[neighbors.size()] = {0};

        while (total_completed < neighbors.size() - 1) {
	    printf("Completed: %zu < %d\n", total_completed, neighbors.size());
            // This sleep slows down the process but saves cpu cycles
            usleep(1000 * 1000);

            // Check for completed recv's
            MPI_Testsome(neighbors.size(), requests, &completed,
                         completed_indices, MPI_STATUSES_IGNORE);
            printf("RANK: %d | Completed this round: %d\n", rank, completed);
            total_completed += completed;
        }

        // Find out parent, the node that didn't complete
        for (size_t i = 0; i < neighbors.size(); i++) {
            if (msgs[i] == -1) {
                parent = neighbors[i];
                break;
            }
        }

        if (parent == nullptr)
            parent = neighbors.back();
    }

    MPI_Barrier(MPI_COMM_WORLD);

    printf("RANK: %d | My parent is %d.\n", rank, parent->rank);

    // Deinit MPI
    MPI_Finalize();

    // Return process end code
    return 0;
}
