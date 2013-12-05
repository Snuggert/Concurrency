/*
 * file.h
 *
 * Contains several functions for file I/O.
 *
 */

#pragma once

void file_read_float_array(const char *filename, float *array, int n);
void file_write_float_array(const char *filename, float *array, int n);
