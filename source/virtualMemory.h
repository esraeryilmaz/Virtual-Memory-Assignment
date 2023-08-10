//
// Created by esoo on 01.06.2023.
//

#ifndef VIRTUALMEMORY_H
#define VIRTUALMEMORY_H

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <ctime>
#include <climits>

#define USER_INPUT_COUNT 8

using namespace std;

// Second-Chance (SC), Least-Recently-Used (LRU), Working set clock (WSClock) algorithms
const char* SC = "SC";
const char* LRU = "LRU";
const char* WSCLOCK = "WSClock";


struct PageTableEntry {
	//int caching_disabled;
	int referenced_bit;		// the reference bit is used in the SC and WSClock algorithms to keep track of page references.
	int modified_bit;
	//int protection_bit;
	int present_absent_bit;
	int page_frame_num;		// to physical address
};


// to hold statistic
struct Statistics {
	int num_reads;
	int num_writes;
	int num_page_misses;
	int num_page_replacements;
	int num_disk_page_writes;
	int num_disk_page_reads;
	int working_set;
};


// function declarations
void initialize_statistics();
void print_statistics();
void print_page_table_struct();
void print_page_table();
void print_virtual_memory(int size);
int is_page_in_physical(int page_num);
void* multiply_virtual_memory(void* arg);
void* add_multiplied_values(void* arg);
void* linear_search_virtual_memory(void* arg);
void* binary_search_virtual_memory(void* arg);
int SC_page_replacement();
int LRU_page_replacement();
int WSClock_page_replacement();
void fill_memory();
// bu iki fonksiyona alternatif olarak sonradan get() ve set() fonksiyonları yazıldı.
int get_virtual_memory(int index);
void set_virtual_memory(int index, int value);
int get(int virtual_address);
void set(int virtual_address, int value);


// Virtual, Physical memory array and page tables
int* virtualMemory;
int* physicalMemory;
struct PageTableEntry* physical_page_array;
struct PageTableEntry* virtual_page_array;

int* result_multiply_1;
int* result_multiply_2;
int* result_add;
int* result_vector_sum;

struct Statistics multiplication_static;
struct Statistics summation_static;
struct Statistics linear_search_static;
struct Statistics binary_search_static;

// Command line input variables
int frame_size = 0;						// argv[1]
int physical_frame_size = 0;			// argv[2]
int virtual_frame_size = 0;				// argv[3]
const char *page_repl_algorithm = NULL;	// argv[4]
const char *table_structure = NULL;		// argv[5]
int print_table_limit = 0;				// argv[6]
const char *disk_filename = NULL;		// argv[7]
FILE *backing_store;


#endif //VIRTUALMEMORY_H
