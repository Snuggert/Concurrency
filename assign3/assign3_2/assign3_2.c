#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#include "assign3_2.h"

struct Buffer
{
	int data;
	int cur_i;
	int max_i;
}; 

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
	int myId, world_size;
	MPI_Comm_rank(communicator, &myId);
	MPI_Comm_size(communicator, &world_size);

	if(world_size < 2)
		return;
 
	if (myId == root) {
		if (myId == world_size - 1)
			MPI_Send(buffer, count, datatype, 0, 0, communicator);
		else
			MPI_Send(buffer, count, datatype, root + 1, 0, communicator);
		
		// If we are the root process, send our data to everyone

	} else {
		int recv;
		if (myId - 1 != 0)
			recv = myId - 1;
		else
			recv = 0;

		MPI_Recv(buffer, count, datatype, recv, 0, communicator,
				 MPI_STATUS_IGNORE);

		/* If the next neighbor is not the root the message will be passed */
		if(myId != world_size - 1 && myId + 1 != root)
			MPI_Send(buffer, count, datatype, myId + 1, 0, communicator);
		else if(root != 0)
			MPI_Send(buffer, count, datatype, 0, 0, communicator);
		
	}
}

/* Complex two way circle broadcast*/
void my_bcast_circle(void* buffer, int count, MPI_Datatype datatype, int root,
		MPI_Comm communicator) {
	int myId, world_size;
	MPI_Status status;
	MPI_Comm_rank(communicator, &myId);
	MPI_Comm_size(communicator, &world_size);

	if(world_size < 2)
		return;

 
	if (myId == root) {
		if(myId == 0){
			MPI_Send(buffer, count, datatype, root + 1, 0, communicator);
			MPI_Send(buffer, count, datatype, world_size - 1, 0, communicator);
			printf("world_size %d, root%d\n", world_size, root);
		}else{
			MPI_Send(buffer, count, datatype, myId - 1, 0, communicator);
			if(myId != world_size - 1){
				MPI_Send(buffer, count, datatype, myId + 1, 0, communicator);
			}else{
				MPI_Send(buffer, count, datatype, 0, 0, communicator);
			}
		}
	} else {
		// If we are a receiver process, receive the data from the root
		MPI_Recv(buffer, count, datatype, MPI_ANY_SOURCE, 0, communicator,
				 &status);

		int distance, max_distance;
		// struct Buffer buffer_t = (struct Buffer) buffer;
		printf("%d received from %d\n", myId, status.MPI_SOURCE);

		/* Send to next neighbor */
		if(status.MPI_SOURCE == myId - 1 && root != myId + 1 && myId != world_size -1){
			max_distance = (world_size/2)-1;
			distance = left_steps_to_source(myId, root, world_size);
				printf("11, %d, %d, %d\n", myId, distance, max_distance);
			if (distance <= max_distance){
				printf("1, %d, %d, %d\n", myId, distance, max_distance);
				MPI_Send(buffer, count, datatype, myId + 1, 0, communicator);	
			}
		}else if(status.MPI_SOURCE == myId + 1 && root != myId - 1 && myId != 0){
			max_distance = max_right(world_size);
			distance = right_steps_to_source(myId, root, world_size);
				printf("12, %d, %d, %d\n", myId, distance, max_distance);
			if (distance <= max_distance){
				printf("2, %d, %d, %d\n", myId, distance, max_distance);
				MPI_Send(buffer, count, datatype, myId - 1, 0, communicator);	
			}
		}else if(status.MPI_SOURCE == world_size - 1 && root != 0){
			max_distance = (world_size/2);
			distance = right_steps_to_source(myId, root, world_size);
				printf("13, %d\n", myId);
			if (distance <= max_distance){
				printf("3, %d\n", myId);
				MPI_Send(buffer, count, datatype, 0, 0, communicator);
			}
		}else if(status.MPI_SOURCE == 0 && root != world_size - 2){
			max_distance = max_right(world_size);
			distance = right_steps_to_source(myId, root, world_size);
				printf("14, %d, %d, %d\n", myId, distance, max_distance);
			if (distance <= max_distance){
				printf("4, %d, %d, %d\n", myId, distance, max_distance);
				MPI_Send(buffer, count, datatype, world_size - 2, 0,
						communicator);
			}
		}else if(myId - 1 < 0){
			max_distance = max_right(world_size);
			distance = right_steps_to_source(myId, root, world_size);
			printf("15, %d, %d, %d\n", myId, distance, max_distance);
			if (distance <= max_distance){
				printf("5, %d, %d, %d\n", myId, distance, max_distance);
				MPI_Send(buffer, count, datatype, world_size - 1, 0,
						communicator);
			}
		}else if(myId == world_size - 1){
			max_distance = (world_size/2);
			distance = left_steps_to_source(myId, root, world_size);
			printf("15, %d, %d, %d\n", myId, distance, max_distance);
			if (distance <= max_distance){
				printf("5, %d, %d, %d\n", myId, distance, max_distance);
				MPI_Send(buffer, count, datatype, 0, 0,
						communicator);
			}
		}

	}
}

int main(int argc, char *argv[]){
	MPI_Init(NULL, NULL);

	int myId, world_size, buffer, root;
	root = 0;

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
		my_bcast_circle(&buffer, 1, MPI_INT, root, MPI_COMM_WORLD);
	} else {
		my_bcast_circle(&buffer, 1, MPI_INT, root, MPI_COMM_WORLD);
	}

	MPI_Finalize();


	return EXIT_SUCCESS;
}