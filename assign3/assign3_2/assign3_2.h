void my_bcast(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);
void my_bcast_circle_1_way(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);
void my_bcast_circle_2_way(void* data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);
int left_steps_to_source(int id, int root, int world_size);
int right_steps_to_source(int id, int root, int world_size);
