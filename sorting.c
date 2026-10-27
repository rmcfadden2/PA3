#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Sets each instance of SIZE to 11
# define SIZE 11

// Shared array declarations
int input_array[SIZE] = {7, 12, 19, 3, 18, 4, 2, -5, 6, 15, 8};
int result_array[SIZE];

// Defines the structure for passing array chunks to threads
typedef struct {
  // Pointer to first element
  int *sub_array_start;
  int size;
} SortParams;

// Defines the structure for passing sorted chunks to be merged
typedef struct {
  SortParams *left_chunk;
  SortParams *right_chunk;
} MergeParams;

// Function signatures for worker threads
extern void* sorting_worker(void* arg);
extern void* merging_worker(void* arg);

int main(void) {
  // Declares threads
  pthread_t thread1, thread2, thread3;
  // Will be used to check for system call errors
  int rc;

  // Allocates thread parameters to the heap
  SortParams *params1 = (SortParams *) malloc(sizeof(SortParams));
  SortParams *params2 = (SortParams *) malloc(sizeof(SortParams));

  // Checks for memory allocation failure
  if (params1 == NULL || params2 == NULL) {
    free(params1);
    free(params2);
    return EXIT_FAILURE;
  }

  // Splits up starting array
  params1->sub_array_start = &input_array[0];
  params1->size = SIZE / 2;
  params2->sub_array_start = &input_array[SIZE / 2];
  params2->size = SIZE - (SIZE / 2);

  // Launch sorting threads
  rc = pthread_create(&thread1, NULL, sorting_worker, (void*)params1);
  // Checks for creation errors
  if (rc != 0) {
    // Delete later
    fprintf(stderr, "Error: Failed to create sorting thread 1 (Code: %d)\n", rc);


    // Handles memory
    free(params1);
    free(params2);
    return EXIT_FAILURE;
  }

  rc = pthread_create(&thread2, NULL, sorting_worker, (void*)params2);
  // Checks for creation errors
  if (rc != 0) {
    // Delete later
    fprintf(stderr, "Error: Failed to create sorting thread 2 (Code: %d)\n", rc);

    // Handles memory
    pthread_join(thread1, NULL);
    free(params1);
    free(params2);
    return EXIT_FAILURE;
  }

  // Wait for sorting to happen

  // Launch the merge thread

  // Wait for merge and handle heap

  return 0;
}