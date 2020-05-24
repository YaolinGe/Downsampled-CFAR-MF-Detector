#include "memory.h"
#include "downlink.h"
#include "SONARconfig.h"
#include "sampling.h"

// ---------- Memory handler for the sonar -------------------------------
// Keep track of how much memory has been used
//int memory_used = 0;
//double memory_used_percent = 0;

// Data vector for loading an echo with $000$LF$ command
//#define STORAGE_VECTOR_LEN 2*NFFT
//int storage_vector_len = DEFAULT_STORAGE_LEN;
//int16_t* storage_vector;
// Static storage:
const int storage_vector_len = DEFAULT_STORAGE_LEN;
const int storage_vector_dc_len = NFFT;
const int storage_vector_test_len = 20000;

int32_t storage_vector[DEFAULT_STORAGE_LEN];
int32_t storage_vector_dc[NFFT];
int32_t storage_vector_test[20000];



volatile int storage_windex = 0; // Storage vector write index
volatile int storage_rindex = 0; // Storage vector index
int Nstored=0;

volatile int storage_windex_dc = 0; // Storage vector write index
volatile int storage_rindex_dc = 0; // Storage vector index
int Nstored_dc=0;

volatile int storage_windex_test = 0; // Storage vector write index
volatile int storage_rindex_test = 0; // Storage vector index
int Nstored_test=0;

//int16_t storage_vector[STORAGE_VECTOR_LEN];

// Memory is usually allocated in segments, but I have not found 
// the segment size for the pic32 (4096 works).
//#define MEMORY_SEGMENT 4096 

// Testing of correlator
//#define NFFT 4096                       // Length of DSP vectors (should be 2^N)	

// Vector for storing correlation values and time values  
// of the last echo
int32_t corr_save[CORR_SAVE_LEN];
int32_t time_save[TIME_SAVE_LEN];

// Initialise sampling & DSP arrays
int16c scratch16[NFFT]; //holds FFT coefficients (16 bit)
int32c scratch32[NFFT]; //holds FFT coefficients (32 bits)
int32c refsig_time[NFFT]; //holds input template for FFT in Q15 format
int32c refsig_freq[NFFT]; //holds output template from FFT in Q15 format


int32c cfreq[NFFT_DC]; //holds MF input (32bit due to large values)
int32c ctime[NFFT_DC]; //holds MF output (32bit due to large values)
int32c twiddles32[NFFT]; // Twiddle factors for 32bit fft (required for lib function)

int32c scratch32_dc[NFFT_DC]; //holds FFT coefficients (32 bits)
int32c twiddles32_dc[NFFT_DC]; // Twiddle factors for 32bit fft (required for lib function)


void insert_into_storage(int32_t val) {
    // Treat storage as a round buffer in case of overflow
    if (storage_windex >= storage_vector_len) {
        storage_windex = 0;
    }
    storage_vector[storage_windex] = val;
    storage_windex++;
    if(Nstored<DEFAULT_STORAGE_LEN){
        Nstored++;
    }else{
        storage_rindex=storage_windex;
    }
}
    
void insert_into_storage_dc(int32_t val) {
    // Treat storage as a round buffer in case of overflow
    if (storage_windex_dc >= storage_vector_dc_len) {
        storage_windex_dc = 0;
    }
    storage_vector_dc[storage_windex_dc] = val;
    storage_windex_dc++;
    if(Nstored_dc<NFFT){
        Nstored_dc++;
    }else{
        storage_rindex_dc=storage_windex_dc;
    }
}

void insert_into_storage_test(int32_t val) {
    // Treat storage as a round buffer in case of overflow
    if (storage_windex_test >= storage_vector_test_len) {
        storage_windex_test = 0;
    }
    storage_vector_test[storage_windex_test] = val;
    storage_windex_test++;
    if(Nstored_test<DEFAULT_STORAGE_LEN){
        Nstored_test++;
    }else{
        storage_rindex_test=storage_windex_test;
    }
}

int16_t read_from_storage(void) {

    // Treat storage as a round buffer in case of overflow
    if (storage_rindex >= storage_vector_len) {
        storage_rindex = 0;
    }
    int16_t val = storage_vector[storage_rindex];
    storage_rindex++;
    if(Nstored>0){
        Nstored--;
    }else{
        // Do something?
    }
    return val;
}


// Set all values in the storage to zero, and clear read/write indices
void clear_storage(void){
    int i;
    for(i=0;i<storage_vector_len;i++){
        storage_vector[i]=0;
    }
    // Reset read and write indices
    storage_windex=0;
    storage_rindex=0;
}

void clear_storage_test(void){
    int i;
    for(i=0;i<storage_vector_test_len;i++){
        storage_vector_test[i]=0;
    }
    // Reset read and write indices
    storage_windex_test=0;
    storage_rindex_test=0;
}

void clear_storage_dc(void){
    int i;
    for(i=0;i<storage_vector_dc_len;i++){
        storage_vector_dc[i]=0;
    }
    // Reset read and write indices
    storage_windex_dc=0;
    storage_rindex_dc=0;
}


int16c* get_scratch16(void) { //holds FFT coefficients (16 bit)
    return scratch16;
}
int32c* get_scratch32(void) { //holds FFT coefficients (32 bits)
    return scratch32;
}
int32c* get_refsig_time(void) { //holds input template for FFT in Q15 format
    return refsig_time;
}
int32c* get_refsig_freq(void) { //holds output template from FFT in Q15 format
    return refsig_freq;
}

void print_refsig(void)
{
    int i;
    int32c *refsig_time = get_refsig_time();
    int32c *refsig_freq = get_refsig_freq();
    print_start_mark("REFSIG_TIME");
    for(i=0;i<NFFT;i++)
    {
        printLongInt(refsig_time[i].re); printDelim(); printEL();
        printLongInt(refsig_time[i].im); printDelim(); printEL();
    }
    print_end_mark("REFSIG_TIME");

    print_start_mark("REFSIG_FREQ");
    for(i=0;i<NFFT;i++)
    {
        printLongInt(refsig_freq[i].re); printDelim(); printEL();
        printLongInt(refsig_freq[i].im); printDelim(); printEL();
    }
    print_end_mark("REFSIG_FREQ");
}

int32c* get_cfreq(void) { //holds MF input (32bit due to large values)
    return cfreq;
}
int32c* get_ctime(void) { //holds MF output (32bit due to large values)
    return ctime;
}
int32_t* get_storage_vector(void) {
    return storage_vector;
}

int32_t* get_storage_vector_test(void) {
    return storage_vector_test;
}

int get_storage_vector_len(void) {
    return storage_vector_len;
}
int get_storage_windex(void) {
    return storage_windex;
}
void set_storage_windex(int ind) {
    storage_windex=ind;
}
int get_storage_rindex(void) {
    return storage_rindex;
}
void set_storage_rindex(int ind) {
    storage_rindex=ind;
}

int get_storage_windex_dc(void) {
    return storage_windex_dc;
}
void set_storage_windex_dc(int ind) {
    storage_windex_dc=ind;
}
int get_storage_rindex_dc(void) {
    return storage_rindex_dc;
}
void set_storage_rindex_dc(int ind) {
    storage_rindex_dc=ind;
}

int get_storage_vector_test_len(void) {
    return storage_vector_test_len;
}
int get_storage_windex_test(void) {
    return storage_windex_test;
}
void set_storage_windex_test(int ind) {
    storage_windex_test=ind;
}
int get_storage_rindex_test(void) {
    return storage_rindex_test;
}
void set_storage_rindex_test(int ind) {
    storage_rindex_test=ind;
}


/*void set_storage_vector_len(int len) {
    storage_vector_len=len;
}*/
/*
int get_memory_used(void) {
    return memory_used;
}
double get_memory_used_percent(void) {
    return memory_used_percent;
}*/

int32_t* get_corr_save(void){
    return corr_save;
}

int32_t* get_time_save(void){
    return time_save;
}

int get_corr_save_len(void){
    return CORR_SAVE_LEN;
}

int get_time_save_len(void){
    return TIME_SAVE_LEN;
}

int32c* get_twiddles32(void){
    return twiddles32;
}

int32c* get_scratch32_dc(void) { //holds FFT coefficients (32 bits)
    return scratch32_dc;
}

int32c* get_twiddles32_dc(void){
    return twiddles32_dc;
}
// Debug =================== can be deleted ================================
int get_Nstored(void)
{
    return Nstored;
}