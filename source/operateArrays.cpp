//
// Created by esoo on 01.06.2023.
//

#include "virtualMemory.h"


int main(int argc, char const *argv[])
{
	if (argc != USER_INPUT_COUNT) {		// 8 input
		cout << "Usage: 8 command line arguments are required!" << endl
			 << "EXAMPLE: $ ./operateArrays 12 5 10 LRU inverted 10000 diskFileName.dat" << endl;
		exit(EXIT_FAILURE);
	}
	if ( isdigit(atoi(argv[1]))!=0 || isdigit(atoi(argv[2]))!=0 || isdigit(atoi(argv[3]))!=0 || isdigit(atoi(argv[6]))!=0 ) {		// 1. 2. 3. 6. must be number
		cout << "Usage: 8 command line arguments are required!" << endl
			 << "EXAMPLE: $ ./operateArrays 12 5 10 LRU inverted 10000 diskFileName.dat" << endl;
		exit(EXIT_FAILURE);
	}
	if ( isdigit(atoi(argv[4])) || isdigit(atoi(argv[5])) || isdigit(atoi(argv[7]))) {		// 4. 5. 7. must not be number
		cout << "Usage: 8 command line arguments are required!" << endl
			 << "EXAMPLE: $ ./operateArrays 12 5 10 LRU inverted 10000 diskFileName.dat" << endl;
		exit(EXIT_FAILURE);
	}
	if ( strcmp(argv[4], SC)!=0 && strcmp(argv[4], LRU)!=0 && strcmp(argv[4], WSCLOCK)!=0 ) {
		cout << "Usage: 8 command line arguments are required!" << endl
			 << "EXAMPLE: $ ./operateArrays 12 5 10 LRU inverted 10000 diskFileName.dat" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		frame_size = pow(2, atoi(argv[1]));				// argv[1]
		physical_frame_size = pow(2, atoi(argv[2]));		// argv[2]
		virtual_frame_size = pow(2, atoi(argv[3]));		// argv[3]

		page_repl_algorithm = argv[4];					// argv[4]
		table_structure = argv[5];						// argv[5]
		print_table_limit = atoi(argv[6]);			// argv[6]
		disk_filename = argv[7];						// argv[7]
		backing_store = fopen(disk_filename, "w+");
		if (backing_store == NULL) {
			perror("file error!");
			exit(EXIT_FAILURE);
		}
	}

	int physical_mem_size = frame_size * physical_frame_size;
	int virtual_mem_size = frame_size * virtual_frame_size;

	if (physical_mem_size > virtual_mem_size) {
		cout << "Virtual memory size must be bigger than physical memory size!! " << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Frame size : \t" << frame_size << endl;
	cout << "Physical frame size : "<< physical_frame_size << endl;
	cout << "Virtual frame size : " << virtual_frame_size << endl;
	cout << "Physical mem size : "<<physical_mem_size<<endl;
	cout << "Virtual mem size :  "<<virtual_mem_size<<endl;

	virtualMemory = new int[virtual_mem_size];		// Virtual memory array'ini oluşturma
	physicalMemory = new int[physical_mem_size];
	result_multiply_1 = new int[virtual_mem_size];
	result_multiply_2 = new int[virtual_mem_size];
	result_add = new int[virtual_mem_size];
	result_vector_sum = new int[virtual_mem_size];


	physical_page_array = new PageTableEntry[physical_frame_size];
	virtual_page_array = new PageTableEntry[virtual_frame_size];

	fill_memory();	// Virtual memory array'ini vs. rastgele tamsayılarla doldurma
	initialize_statistics();
	print_page_table();

	//////////// Perform operations, set/get values in the virtual memory...

	// Create threads for multiplication and summation operations
	pthread_t multiply_thread, summation_thread;
	int multiply_thread_id = 0;
	int summation_thread_id = 1;

	pthread_create(&multiply_thread, NULL, multiply_virtual_memory, &multiply_thread_id);
	pthread_create(&summation_thread, NULL, add_multiplied_values, &summation_thread_id);

	// Wait for multiplication and summation threads to finish
	pthread_join(multiply_thread, NULL);
	pthread_join(summation_thread, NULL);

	// Create threads for linear and binary search operations
	pthread_t linear_search_thread, binary_search_thread;
	int linear_search_thread_id = 2;
	int binary_search_thread_id = 3;

////
/*
	pthread_create(&linear_search_thread, NULL, linear_search_virtual_memory, &linear_search_thread_id);
	pthread_create(&binary_search_thread, NULL, binary_search_virtual_memory, &binary_search_thread_id);

	// Wait for search threads to finish
	pthread_join(linear_search_thread, NULL);
	pthread_join(binary_search_thread, NULL);
*/
////

	cout << "\n-----------------------------------------------------------------" << endl<<endl;
	cout << ">>>>  After operations  <<<<"<<endl<<endl;
	print_page_table();
	print_page_table_struct();
	print_statistics();

	delete[] virtualMemory; // Dinamik belleği serbest bırakır
	delete[] physicalMemory;
	delete[] physical_page_array;
	delete[] virtual_page_array;
	delete[] result_multiply_1;
	delete[] result_multiply_2;
	delete[] result_add;
	delete[] result_vector_sum;
	fclose(backing_store);

	return 0;
}


void initialize_statistics() {
	multiplication_static.num_reads = 0;
	multiplication_static.num_writes = 0;
	multiplication_static.num_page_misses = 0;
	multiplication_static.num_page_replacements = 0;
	multiplication_static.num_disk_page_writes = 0;
	multiplication_static.num_disk_page_reads = 0;
	multiplication_static.working_set = 0;

	summation_static.num_reads = 0;
	summation_static.num_writes = 0;
	summation_static.num_page_misses = 0;
	summation_static.num_page_replacements = 0;
	summation_static.num_disk_page_writes = 0;
	summation_static.num_disk_page_reads = 0;
	summation_static.working_set = 0;

	linear_search_static.num_reads = 0;
	linear_search_static.num_writes = 0;
	linear_search_static.num_page_misses = 0;
	linear_search_static.num_page_replacements = 0;
	linear_search_static.num_disk_page_writes = 0;
	linear_search_static.num_disk_page_reads = 0;
	linear_search_static.working_set = 0;

	binary_search_static.num_reads = 0;
	binary_search_static.num_writes = 0;
	binary_search_static.num_page_misses = 0;
	binary_search_static.num_page_replacements = 0;
	binary_search_static.num_disk_page_writes = 0;
	binary_search_static.num_disk_page_reads = 0;
	binary_search_static.working_set = 0;
}


void print_statistics() {
	cout << "-----------------------------------------------------------------" << endl;
	cout << ">>>>  Program Statistics  <<<<" << endl;
	cout << "Multiplication Statistics" << endl
		 << "Number of reads : " << multiplication_static.num_reads << endl
		 << "Number of writes : " << multiplication_static.num_writes << endl
		 << "Number of page misses : " << multiplication_static.num_page_misses << endl
		 << "Number of page replacements : " << multiplication_static.num_page_replacements << endl
		 << "Number of disk page writes : " << multiplication_static.num_disk_page_writes << endl
		 << "Number of disk page reads : " << multiplication_static.num_disk_page_reads << endl
		 << "Estimated working set function w for each thread as a table : " << multiplication_static.working_set << endl << endl;

	cout << "Summation Statistics" << endl
		 << "Number of reads : " << summation_static.num_reads << endl
		 << "Number of writes : " << summation_static.num_writes << endl
		 << "Number of page misses : " << summation_static.num_page_misses << endl
		 << "Number of page replacements : " << summation_static.num_page_replacements << endl
		 << "Number of disk page writes : " << summation_static.num_disk_page_writes << endl
		 << "Number of disk page reads : " << summation_static.num_disk_page_reads << endl
		 << "Estimated working set function w for each thread as a table : " << summation_static.working_set << endl << endl;

	cout << "Linear Search Statistics" << endl
		 << "Number of reads : " << linear_search_static.num_reads << endl
		 << "Number of writes : " << linear_search_static.num_writes << endl
		 << "Number of page misses : " << linear_search_static.num_page_misses << endl
		 << "Number of page replacements : " << linear_search_static.num_page_replacements << endl
		 << "Number of disk page writes : " << linear_search_static.num_disk_page_writes << endl
		 << "Number of disk page reads : " << linear_search_static.num_disk_page_reads << endl
		 << "Estimated working set function w for each thread as a table : " << linear_search_static.working_set << endl << endl;

	cout << "Binary Search Statistics" << endl
		 << "Number of reads : " << binary_search_static.num_reads << endl
		 << "Number of writes : " << binary_search_static.num_writes << endl
		 << "Number of page misses : " << binary_search_static.num_page_misses << endl
		 << "Number of page replacements : " << binary_search_static.num_page_replacements << endl
		 << "Number of disk page writes : " << binary_search_static.num_disk_page_writes << endl
		 << "Number of disk page reads : " << binary_search_static.num_disk_page_reads << endl
		 << "Estimated working set function w for each thread as a table : " << binary_search_static.working_set << endl << endl;
}


void print_page_table_struct() {
	cout << "-----------------------------------------------------------------" << endl;
	cout << ">> Page table has " << virtual_frame_size << " elements." << endl << endl;
	for (int i=0 ; i<virtual_frame_size ; i++) {
		cout << "- Entry " << i << endl
			 << "Referenced Bit :\t" << virtual_page_array[i].referenced_bit << endl
			 << "Modified Bit :\t\t" << virtual_page_array[i].modified_bit << endl
			 << "Present/Absent Bit :\t" << virtual_page_array[i].present_absent_bit << endl
			 << "Page Frame Number :\t" << virtual_page_array[i].page_frame_num << endl << endl;
	}
}


void print_page_table() {
	int temp=0;
	cout << "-----------------------------------------------------------------" << endl;
	cout << ">> Page table has " << virtual_frame_size << " elements." << endl << endl;
	for (int i=0 ; i<virtual_frame_size ; i++) {
		cout << "- Entry " << i << " ->\t";
		for(int j=0 ; j<frame_size ; ++j) {
			cout << virtualMemory[temp] << " , ";
			temp++;
		}
		cout << endl;
	}
}


void print_virtual_memory(int size) {
	for (int i=0 ; i<size ; ++i) {
		cout << *(virtualMemory + i) << ", ";
	}
}


// Set a value in the virtual memory at the specified index
void set_virtual_memory(int index, int value) {
	virtualMemory[index] = value;
}


// Get a value from the virtual memory at the specified index
int get_virtual_memory(int index) {
	return virtualMemory[index];
}


int is_page_in_physical(int page_num) {
	for(int i=0 ; i<physical_frame_size ; i++){
		if(page_num == physical_page_array[i].page_frame_num) {
			return 1;		// the page in the physical page table
		}
	}
	return 0;
}


// Get a value from the physical memory at the specified index
int get_physical_memory(int index) {
	//return physicalMemory[index];
	int keep;
	int block_index = index / frame_size;
	for(int i=0 ; i<physical_frame_size ; i++){
		if(block_index==physical_page_array[i].page_frame_num){
			keep=i;
		}
	}
	int mod = index % frame_size;
	return physicalMemory[(keep*frame_size)+mod];
}


// Multiply the virtual memory array with the vector array using multiple threads
void* multiply_virtual_memory(void* arg) {
	int thread_id = *(int*)arg;
	int chunk_size = virtual_frame_size*frame_size / 2;
	int start = thread_id * chunk_size;
	int end = start + chunk_size;

	// Multiply the virtual memory array with the vector array for the assigned chunk
	for (int i = 0; i < virtual_frame_size*frame_size; i++) {
		//set_virtual_memory(i, result);
		result_multiply_1[i] = get_virtual_memory(i)*3;
		result_multiply_2[i] = get_virtual_memory(i)* get_virtual_memory(i);
		//result_multiply_1[i] = get(i)*3;
		//result_multiply_2[i] = get(i)* get(i);
	}

	// Update statistics for multiplication
	multiplication_static.num_reads += chunk_size;
	multiplication_static.num_writes += chunk_size;

	return NULL;
}


// Add the multiplied values of the virtual memory array using a single thread
void* add_multiplied_values(void* arg) {
	// Add the multiplied values of the virtual memory array
	int thread_id = *(int*)arg;
	int chunk_size = virtual_frame_size*frame_size / 2;
	int start = thread_id * chunk_size;
	int end = start + chunk_size;

	// Add the multiplied values of the virtual memory array for the assigned chunk
	for (int i = 0; i < virtual_frame_size*frame_size ; i++) {
		//sum += get_virtual_memory(i);
		result_add[i] = result_multiply_1[i] + result_multiply_2[i];
		//set_virtual_memory(i, result_add[i]);
		set(i, result_add[i]);
		summation_static.working_set++;
	}
	// Update statistics for summation
	summation_static.num_reads += chunk_size;
	summation_static.num_writes += chunk_size;

	return NULL;
}


// Search for numbers in the virtual memory array using a linear search algorithm
void* linear_search_virtual_memory(void* arg) {
	// Perform linear search on the virtual memory array
	int* numbersToSearch = (int*)arg;

	// Perform linear search on the virtual memory array
	for (int i = 0; i < frame_size*virtual_frame_size ; i++) {
		bool found = false;
		for (int j = 0; j < virtual_frame_size; j++) {
			if (get_virtual_memory(j) == numbersToSearch[i]) {
				found = true;
				break;
			}
		}
		linear_search_static.working_set++;
		if (found) {
			// Number is found
			// Update statistics for linear search
			linear_search_static.num_reads++;
		} else {
			// Number is not found
			// Update statistics for linear search
			linear_search_static.num_reads++;
			linear_search_static.num_page_misses++;
		}
	}
	return NULL;
}


// Search for numbers in the virtual memory array using a binary search algorithm
void* binary_search_virtual_memory(void* arg) {
	// Perform binary search on the virtual memory array
	int* numbersToSearch = (int*)arg;

	// Perform binary search on the virtual memory array
	for (int i = 0; i < frame_size*virtual_frame_size; i++) {
		int left = 0;
		int right = virtual_frame_size - 1;
		bool found = false;

		while (left <= right) {
			int mid = left + (right - left) / 2;
			int currentValue = get_virtual_memory(mid);

			if (currentValue == numbersToSearch[i]) {
				found = true;
				break;
			} else if (currentValue < numbersToSearch[i]) {
				left = mid + 1;
			} else {
				right = mid - 1;
			}
		}
		binary_search_static.working_set++;
		if (found) {
			// Number is found
			// Update statistics for binary search
			binary_search_static.num_reads++;
		} else {
			// Number is not found
			// Update statistics for binary search
			binary_search_static.num_reads++;
			binary_search_static.num_page_misses++;
		}
	}
	return NULL;
}


int SC_page_replacement() {
	int victim_frame = -1;
	int i = 0;
	while (victim_frame == -1) {
		struct PageTableEntry* entry = &physical_page_array[i];
		if (entry->referenced_bit == 0) {
			if (entry->modified_bit == 1) {
				// Write the page back to the disk
				entry->modified_bit = 0;
				// Write to disk page writes statistic
				multiplication_static.num_disk_page_writes++;
			}
			victim_frame = i;
		} else {
			entry->referenced_bit = 0;
		}
		i = (i + 1) % physical_frame_size;
	}
	return victim_frame;
}


int LRU_page_replacement() {
	int victim_frame = -1;
	int min_reference = INT_MAX;
	for (int i = 0; i < physical_frame_size; i++) {
		struct PageTableEntry* entry = &physical_page_array[i];
		if (entry->referenced_bit < min_reference) {
			min_reference = entry->referenced_bit;
			victim_frame = i;
		}
	}
	if (victim_frame != -1) {
		if (physical_page_array[victim_frame].modified_bit == 1) {
			// Write the page back to the disk
			physical_page_array[victim_frame].modified_bit = 0;
			// Write to disk page writes statistic
			multiplication_static.num_disk_page_writes++;
		}
	}
	return victim_frame;
}


int WSClock_page_replacement() {
	int victim_frame = -1;
	static int hand = 0;
	int i = hand;
	while (victim_frame == -1) {
		struct PageTableEntry* entry = &physical_page_array[i];
		if (entry->referenced_bit == 0) {
			if (entry->modified_bit == 1) {
				// Write the page back to the disk
				entry->modified_bit = 0;
				// Write to disk page writes statistic
				multiplication_static.num_disk_page_writes++;
			}
			victim_frame = i;
		} else {
			entry->referenced_bit = 0;
		}
		i = (i + 1) % physical_frame_size;
		if (i == hand) {
			// All frames are referenced, reset the reference bits
			for (int j = 0; j < physical_frame_size; j++) {
				physical_page_array[j].referenced_bit = 0;
			}
		}
	}
	hand = (i + 1) % physical_frame_size;
	return victim_frame;
}

// Sayfa tablosunu kontrol et
// Sayfa mevcut değilse, bir sayfa yer değiştirme algoritması kullanarak sayfayı belleğe yerleştir
// Sayfa yerleştirme işlemi gerekiyorsa, diskten veriyi oku ve hedef çerçeveye yerleştir
// Sayfayı oku ve ilgili veriyi döndür
int get(int virtual_address) {
	int virtual_page_number = virtual_address / frame_size;
	int offset = virtual_address % frame_size;

	// Check if the page is present in the physical memory
	if (virtual_page_array[virtual_page_number].present_absent_bit == 1) {
		int physical_frame_number = virtual_page_array[virtual_page_number].page_frame_num;
		int physical_address = physical_frame_number * frame_size + offset;

		// Read the data from the physical memory
		int data = physicalMemory[physical_address];

		// Increment the read counter
		linear_search_static.num_reads++;

		//printf("Data at virtual address %d: %d\n", virtual_address, data);
		return data;
	}
	else {
		// Page fault occurred, need to fetch the page from disk
		// Increment the page miss counter
		linear_search_static.num_page_misses++;

		// Determine the victim frame using the page replacement algorithm
		int victim_frame;
		if (strcmp(page_repl_algorithm, SC) == 0) {
			victim_frame = SC_page_replacement();
		}
		else if (strcmp(page_repl_algorithm, LRU) == 0) {
			victim_frame = LRU_page_replacement();
		}
		else if (strcmp(page_repl_algorithm, WSCLOCK) == 0) {
			victim_frame = WSClock_page_replacement();
		}
		else {
			// Invalid page replacement algorithm
			cout << "Invalid page replacement algorithm!" <<endl;
			return -1;
		}

		// Update the page table entry for the victim frame
		virtual_page_array[physical_page_array[victim_frame].page_frame_num].present_absent_bit = 0;

		// Fetch the page from the disk
		fseek(backing_store, virtual_page_number * frame_size * sizeof(int), SEEK_SET);
		fread(physicalMemory + victim_frame * frame_size, sizeof(int), frame_size, backing_store);

		// Update the page table entry for the fetched page
		virtual_page_array[virtual_page_number].present_absent_bit = 1;
		virtual_page_array[virtual_page_number].page_frame_num = victim_frame;

		// Increment the page replacement counter
		linear_search_static.num_page_replacements++;

		// Read the data from the physical memory
		int data = physicalMemory[victim_frame * frame_size + offset];

		// Increment the read counter
		linear_search_static.num_reads++;

		//printf("Data at virtual address %d: %d\n", virtual_address, data);
		return data;
	}
}

// Sayfa tablosunu kontrol et
// Sayfa mevcut değilse, bir sayfa yer değiştirme algoritması kullanarak sayfayı belleğe yerleştir
// Sayfa yerleştirme işlemi gerekiyorsa, diskten veriyi oku ve hedef çerçeveye yerleştir
// Hedef çerçeveye değeri yaz
// Sayfa tablosunda değişiklikleri güncelle (referenced_bit, modified_bit vb.)
void set(int virtual_address, int value) {
	int virtual_page_number = virtual_address / frame_size;
	int offset = virtual_address % frame_size;

	virtualMemory[virtual_address] = value;
	// Check if the page is present in the physical memory
	if (virtual_page_array[virtual_page_number].present_absent_bit == 1) {
		int physical_frame_number = virtual_page_array[virtual_page_number].page_frame_num;
		int physical_address = physical_frame_number * frame_size + offset;

		// Write the value to the physical memory
		physicalMemory[physical_address] = value;

		// Set the modified bit of the page table entry
		virtual_page_array[virtual_page_number].modified_bit = 1;

		// Increment the write counter
		linear_search_static.num_writes++;

		//printf("Value set at virtual address %d: %d\n", virtual_address, value);
	}
	else {
		// Page fault occurred, need to fetch the page from disk
		// Increment the page miss counter
		linear_search_static.num_page_misses++;

		// Determine the victim frame using the page replacement algorithm
		int victim_frame;
		if (strcmp(page_repl_algorithm, SC) == 0) {
			victim_frame = SC_page_replacement();
		}
		else if (strcmp(page_repl_algorithm, LRU) == 0) {
			victim_frame = LRU_page_replacement();
		}
		else if (strcmp(page_repl_algorithm, WSCLOCK) == 0) {
			victim_frame = WSClock_page_replacement();
		}
		else {
			// Invalid page replacement algorithm
			cout << "Invalid page replacement algorithm!" <<endl;
			return;
		}

		// Update the page table entry for the victim frame
		virtual_page_array[physical_page_array[victim_frame].page_frame_num].present_absent_bit = 0;

		// Fetch the page from the disk
		fseek(backing_store, virtual_page_number * frame_size * sizeof(int), SEEK_SET);
		fread(physicalMemory + victim_frame * frame_size, sizeof(int), frame_size, backing_store);

		// Update the page table entry for the fetched page
		virtual_page_array[virtual_page_number].present_absent_bit = 1;
		virtual_page_array[virtual_page_number].page_frame_num = victim_frame;

		// Increment the page replacement counter
		linear_search_static.num_page_replacements++;

		// Write the value to the physical memory
		physicalMemory[victim_frame * frame_size + offset] = value;

		// Set the modified bit of the page table entry
		virtual_page_array[virtual_page_number].modified_bit = 1;

		// Increment the write counter
		linear_search_static.num_writes++;

		//printf("Value set at virtual address %d: %d\n", virtual_address, value);
	}
}


// mainde new operatörü kullanıldı
// burada VM ve PM rastgele sayılarla doldurulacak
// ayrıca page table lar initialize edilecek
// son olarak diske yazma yapılacak
void fill_memory() {
	int rand_num;
	srand(time(0));		// Rastgele sayı üretimi

	int rand_num_to_physical=0;
	for (int i=0 ; i<virtual_frame_size ; ++i) {
		rand_num_to_physical = i;
		virtual_page_array[i].page_frame_num = rand_num_to_physical;		//bunlar page replacement ile değiştirilecek sonra!!!!
		virtual_page_array[i].referenced_bit = 0;
		virtual_page_array[i].modified_bit = 0;
		virtual_page_array[i].present_absent_bit = 0;
	}
	for (int i=0 ; i<physical_frame_size ; ++i) {
		physical_page_array[i].page_frame_num = 0;
		physical_page_array[i].referenced_bit = 0;
		physical_page_array[i].modified_bit = 0;
		physical_page_array[i].present_absent_bit = 0;
	}

	//cout << "VM :  ";
	for(int i=0 ; i<virtual_frame_size*frame_size ; i++) {
		rand_num = rand()%1001;
		set(i, rand_num);			// Rastgele bir tamsayının virtual memory array'ine atanması
		//cout << virtualMemory[i] << ", ";
	}
	cout << endl;

	int count = 0;
	//cout << "PM :  ";
	//filling physical mem
	for(int i=0 ; i<physical_frame_size*frame_size ; i++) {
		physicalMemory[i] = rand()%1001;
		fprintf(backing_store,"%-11d\n",physicalMemory[i]);
		count++;
		//cout << physicalMemory[i] << ", ";
	}

	// fazlası disk'e yazılıyor.
	for(int i=count ; i<virtual_frame_size*frame_size ; i++){
		int r=rand()%1001;
		fprintf(backing_store,"%-11d\n",r);
	}

	// temporary olarak kullanacağım arraylere 0 atıyorum başta
	for (int i = 0; i < virtual_frame_size*frame_size; ++i) {
		result_multiply_1[i] = 0;
		result_multiply_2[i] = 0;
		result_add[i] = 0;
		result_vector_sum[i] = 0;
	}

	count=0;
	cout << endl;
}

