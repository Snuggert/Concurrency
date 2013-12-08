/*
 * file.h
 *
 * Contains several functions for file I/O.
 *
 */

 
void file_write_float_array(const char *filename, float *array, int n);
void file_read_float_array(const char *filename, float *array, int n);   
// class file{
//  public:
//     void file_write_float_array();
//     void file_read_float_array();    
// };