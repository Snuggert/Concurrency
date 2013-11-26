#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#include "assign3_2.h"


/* Calculate the amount to source to the left */
int left_steps_to_source(int id, int root, int world_size){
	if(root <= id){
		return id - root;
	}else{
		return world_size - root + id; 
	}
}

/* Calculate the amount to source to the right */
int right_steps_to_source(int id, int root, int world_size){
	if(root >= id){
		return root - id;
	}else{
		return world_size - id + root;
	}
}

/* Calculate the max amount */
int max_right(int world_size){
	if(world_size % 2 != 1){
		return (world_size / 2) - 1;
	}else{
		return (world_size / 2);
	}
}

/* Simple broadcast to all */
void my_bcast(void* buffer, int count, MPI_Datatype datatype, int root,
		MPI_Comm communicator) {
	int myId;
	MPI_Comm_rank(communicator, &myId);
	int world_size;
	MPI_Comm_size(communicator, &world_size);
 
	if (myId == root) {
		// If we are the root process, send our data to everyone
		int i;
		for (i = 0; i < world_size; i++) {
			if (i != myId) {
				MPI_Send(buffer, count, datatype, i, 0, communicator);
			}
		}
	} else {
		// If we are a receiver process, receive the data from the root
		MPI_Recv(buffer, count, datatype, root, 0, communicator,
				 MPI_STATUS_IGNORE);
		
	}
}

/* Simple one way circle broadcast*/
void my_bcast_circle_1_way(void* buffer, int count, MPI_Datatype datatype, int root,
		MPI_Comm communicator) {
	int myId, world_size, send;
	MPI_Comm_rank(communicator, &myId);
	MPI_Comm_size(communicator, &world_size);
	// int* buf = (int*)buffer;
	// printf("%d\n", buf);

	if(world_size < 2)
		return;

	send = (myId + 1) % (world_size);
	if(send < 0){
		send = world_size + send;
	}
 
	if (myId == root) {
		MPI_Send(buffer, count, datatype, send, myId, communicator);
		printf("[init]From %d to %d\n", myId, (myId + 1) % (world_size));

	} else {
		int recv;

		recv = (myId - 1) % (world_size);
		if(recv < 0){
			recv = world_size + recv;
		}

		MPI_Recv(buffer, count, datatype, recv, recv, communicator,
				 MPI_STATUS_IGNORE);

		if(send != root){
		 	MPI_Send(buffer, count, datatype, send, myId, communicator);
			printf("[resend]From %d to %d\n", myId, send);
		}

	}
}

/* Complex two way circle broadcast*/
void my_bcast_circle_2_way(void* buffer, int count, MPI_Datatype datatype, int root,
		MPI_Comm communicator) {
	int myId, world_size, left_neighbor, right_neighbor;

	MPI_Status status;
	MPI_Comm_rank(communicator, &myId);
	MPI_Comm_size(communicator, &world_size);


	if(world_size < 2)
		return;

	/* Calculate the left and right neighbors */
	left_neighbor 	= (myId - 1) % (world_size);
	if(left_neighbor < 0)
		left_neighbor = world_size + left_neighbor;
	right_neighbor 	= (myId + 1) % (world_size);


	if (myId == root) {
	 	/* If the thread is the root send to left and right neighbor */
		MPI_Send(buffer, count, datatype, right_neighbor, 0, communicator);
		MPI_Send(buffer, count, datatype, left_neighbor, 0, communicator);
	} else {
	 	/* 
	 	 * If the thread is not the root send to left and right neighbor
	 	 * when the maximum steps in that directions are not yet reached.
	 	 */
		
		MPI_Recv(buffer, count, datatype, MPI_ANY_SOURCE, 0, communicator,
				 &status);

		int distance, max_distance;

		printf("%d received from %d\n", myId, status.MPI_SOURCE);

		/* Send to next neighbor */
		if(status.MPI_SOURCE == right_neighbor && left_neighbor != root){
			max_distance = max_right(world_size);
			distance = right_steps_to_source(myId, root, world_size);
			if (distance <= max_distance){
				MPI_Send(buffer, count, datatype, left_neighbor, 0, communicator);	
			}
		}else if(status.MPI_SOURCE == left_neighbor && right_neighbor != root){
			max_distance = (world_size/2)-1;
			distance = left_steps_to_source(myId, root, world_size);
			if (distance <= max_distance){
				MPI_Send(buffer, count, datatype, right_neighbor, 0, communicator);	
			}
		}
	}
}

int main(int argc, char *argv[]){
	MPI_Init(NULL, NULL);

	int myId, world_size, root;
	root = 5;
	int buffer;

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);

	if (root >= world_size || root < 0){
		if(myId == 0)
			printf("Root has to be within the range of threads\n");
		return EXIT_FAILURE;
	}

	if (myId == root) {
		buffer = 12;
		printf("Process %d broadcasting data %d\n", root, buffer);
		my_bcast_circle_2_way(&buffer, 1, MPI_INT, root, MPI_COMM_WORLD);
	} else {
		my_bcast_circle_2_way(&buffer, 1, MPI_INT, root, MPI_COMM_WORLD);
		printf("Thread %d received value %d\n", myId, buffer);
	}

	MPI_Finalize();

	return EXIT_SUCCESS;
}