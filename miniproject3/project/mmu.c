#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

// defines
#define PAGE_ENTRIES 256 // 2^8
#define PAGE_SIZE 256 // 2^8

#define TLB_ENTRIES 16 

#define FRAME_SIZE 256
#define FRAME_ENTRIES 128 

#define PHYSICAL_MEMORY_SIZE (FRAME_ENTRIES * FRAME_SIZE)
#define BACKSTORE_MEMORY_SIZE (256 * FRAME_SIZE)

#define PAGE_OFFSET 8
#define PAGE_MASK 255

#define TLB_FAILED -1
#define PAGE_FAULT -1
#define MEMORY_NOT_EMPTY -1

#define PAGE_TABLE_VAL_INIT -1
#define TLB_VAL_INIT -1

// variables
int address_logical;
int address_physical;
int address_page;

int page_number;
int offset;
int frame_number;

// storages
int tlb[TLB_ENTRIES][2]; // #entires x [page# and frame#]
int tlb_head = -1; 
int tlb_tail = -1; // works like pointers.

int page_table[PAGE_ENTRIES][2]; // # entires x [frame and counter]

int page_table_counter = 0; // global counter for page_table

char physical_memory[PHYSICAL_MEMORY_SIZE];
int memory_first_empty_address = 0;
int memory_LRU_address = 0;

int original_frame_address;
int original_page_address;

// temp
int queried_value;

// statistics
int n_page_fault = 0;
int n_tlb_hit = 0;
int n_address_read = 0;
float fault_rate = -1;
float tlb_rate = -1;

// functions
int get_page_number(int v_address){
    return (v_address >> PAGE_OFFSET);
}

int get_offset(int v_address){
    return (v_address & PAGE_MASK);
}

int get_page_table_frame(int a_page_number){
    if (page_table[page_number][0] == PAGE_FAULT) {
        n_page_fault = n_page_fault + 1;
    }
    return page_table[page_number][0];
}

int get_tlb_frame(int a_page_number){
    for (int i = 0; i < TLB_ENTRIES; i++) {
        if (tlb[i][0] == page_number) {
            n_tlb_hit = n_tlb_hit + 1;
            return tlb[i][1];
        }
    }
    // If we did not find tlb result in table, its a miss = -1.
    return TLB_FAILED;
}

int get_page_table_page(int a_frame_number){
    for (int i = 0; i < PAGE_ENTRIES; i++) {
        if(page_table[i][0] == a_frame_number){
            return i;
        }
    }
    // error!
    return -1; 
}

int get_LRU_page_number(){
    int lowest_count = 2147483000;
    int lowest_page = 2147483000;
    for (int i = 0; i < PAGE_ENTRIES; i++) {
        if(page_table[i][0] != -1){
            if(page_table[i][1] < lowest_count){
                lowest_page = i;
                lowest_count = page_table[i][1];
            }
        }
    }
    return lowest_page;
}

void update_page_table(int page_number, int frame_number){
    if(frame_number != -1){
        page_table[page_number][0] = frame_number;
        page_table[page_number][1] = page_table_counter;
        page_table_counter = page_table_counter + 1;
    }else{
        page_table[page_number][0] = -1;
        page_table[page_number][1] = -1;
    }
    
}

void update_tlb(int page_number, int frame_number){
    if (frame_number != -1){
        if (tlb_head == -1){
            // empty then add one
            tlb_head = 0;
            tlb_tail = 0;
        }else{
            // non-empty then go next (wrap around)
            tlb_head = tlb_head + 1;
            tlb_head = tlb_head % TLB_ENTRIES;

            tlb_tail = tlb_tail + 1;
            tlb_tail = tlb_tail % TLB_ENTRIES;
        }
    }
    tlb[tlb_tail][0] = page_number;
    tlb[tlb_tail][1] = frame_number;
    
}


int main(int n_arg, char *args[]){
    // file names
    char* fn_back_store; // file_name_back_store
    char* fn_address;
    char* fn_output;
    // files
    FILE* fp_address; // file_pointer_address
    FILE* fp_output;
    // file descriptors
    int fd_store;
    // virtual_memory
    char* back_store;
    // temp
    char current_address[10]; // reading 2^32 = 4294967296 = 10 digits
    
    
    if (n_arg != 4){
        printf("Error: wrong number of arguments. Give back_store.bin, addresses.txt and output.txt");
    }else{ // n_arg == 4
        fn_back_store = args[1];
        fn_address = args[2];
        fn_output = args[3];

        // deal with opening address
        if((fp_address = fopen(fn_address, "r")) == NULL){
            printf("Error: Address file could not be opened.");
            exit(EXIT_FAILURE); 
        }

        // deal with opening output
        if((fp_output = fopen(fn_output, "w")) == NULL){
            printf("Error: Output file could not be opened.");
            exit(EXIT_FAILURE); 
        }

        // deal with mapping store file to virtual memory
        fd_store = open(fn_back_store, O_RDWR);
        back_store = mmap(0, BACKSTORE_MEMORY_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd_store, 0);

        // check if mmap failed
        if(back_store == MAP_FAILED){
            close(fd_store);
            printf("Error: memory mapping back store file failed.");
            exit(EXIT_FAILURE);
        }

        // init page_table
        for (int i = 0; i < PAGE_ENTRIES; i++) {
            page_table[i][0] = PAGE_TABLE_VAL_INIT; // frame
            page_table[i][1] = PAGE_TABLE_VAL_INIT; // counter
        }

        // init tlb
        for (int i = 0; i < TLB_ENTRIES; i++) {
            tlb[i][0] = TLB_VAL_INIT;
            tlb[i][1] = TLB_VAL_INIT;
        }
        
        // first line on output
        // fprintf(fp_output, "%s,%s,%s\n", "Logical address", "Physical address", "Signed byte value");  

        // going through each line in address
        while (fgets(current_address, sizeof(current_address), fp_address)){
            // read the address number as string and convert it to int
            address_logical = atoi(current_address);
            // then increment number of address read
            n_address_read = n_address_read + 1;
            // get pg# and offset
            page_number = get_page_number(address_logical);
            offset = get_offset(address_logical);
            frame_number = get_tlb_frame(page_number);

            if(frame_number != TLB_FAILED){
                // TLB hit
                address_physical = frame_number + offset;
                queried_value = physical_memory[address_physical];
            }else{ 
                // TLB miss
                frame_number = get_page_table_frame(page_number);
                if (frame_number != PAGE_FAULT){
                    // page table hit
                    address_physical = frame_number + offset;
                    queried_value = physical_memory[address_physical];

                    // notify tlb
                    update_tlb(page_number, frame_number); // UPDATE THIS IF FORUM WRONG -----------------------------------------------
                }else{
                    // page table miss
                    address_page = page_number * PAGE_SIZE;
                    if (memory_first_empty_address != MEMORY_NOT_EMPTY){
                        // memory not full. need to copy back storage to RAM.
                        
                        memcpy(physical_memory + memory_first_empty_address, back_store + address_page, PAGE_SIZE);
                        frame_number = memory_first_empty_address;
                        address_physical = frame_number + offset;
                        queried_value = physical_memory[address_physical]; 

                        // notify page table and tlb of new change
                        update_page_table(page_number, frame_number);
                        update_tlb(page_number, frame_number);

                        // "adding" back memory index
                        // if the memory still has place to fill
                        if (memory_first_empty_address < PHYSICAL_MEMORY_SIZE - FRAME_SIZE){
                            memory_first_empty_address += FRAME_SIZE;
                        }else{
                            // maxed out
                            memory_first_empty_address = -1;
                        }
                    }else{
                        // memory full. need to swap with LRU algorithm
                        // 1. page out
                        int LRU_page_num = get_LRU_page_number();
                        original_frame_address = page_table[LRU_page_num][0];
                        original_page_address = LRU_page_num * PAGE_SIZE;
                        memcpy(back_store + original_page_address, physical_memory + original_frame_address, PAGE_SIZE);
                        // 2. change to invalid
                        update_page_table(LRU_page_num, -1);
                        update_tlb(LRU_page_num, -1);
                        // 3. page in
                        memcpy(physical_memory + original_frame_address, back_store + address_page, PAGE_SIZE);
                        // 4. validate
                        frame_number = original_frame_address;
                        address_physical = frame_number + offset;
                        queried_value = physical_memory[address_physical];
                        update_page_table(page_number, frame_number);
                        update_tlb(page_number, frame_number);
                    }
                }

            }

            // a line in csv
            fprintf(fp_output, "%d,%d,%d\n", address_logical, address_physical, queried_value);   
        }

        fault_rate = (float) n_page_fault / (float) n_address_read;
        tlb_rate = (float) n_tlb_hit / (float) n_address_read;

        printf("%s : %.3f\n", "fault_rate", fault_rate);
        printf("%s : %.3f\n", "tlb_rate", tlb_rate);

        fclose(fp_address);
        fclose(fp_output);
        close(fd_store);

    }

    return EXIT_SUCCESS;
}

