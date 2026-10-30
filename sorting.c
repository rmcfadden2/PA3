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
// extern void* merging_worker(void* arg);

int main(void) {
  printf("Unsorted input array:\n");
  for (int i = 0; i < SIZE; i++) {
    printf("%d ", input_array[i]);
  }
  printf("\n");
  printf("Sorted sub-array 1:\n");

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
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);


  // TEST SORTED SUB-ARRAYS
  // int *sub_array1 = params1->sub_array_start;
  // int *sub_array2 = params2->sub_array_start;
  // int size1 = params1->size;
  // int size2 = params2->size;

  // for (int i = 0; i < size1; i++) {
  //   printf("%d ", sub_array1[i]);
  // }

  // printf("\n");
  // printf("Sorted sub-array 2:\n");

  // for (int i = 0; i < size2; i++) {
  //   printf("%d ", sub_array2[i]);
  // }
  // printf("\n");

  // Launch the merge thread

  // Wait for merge and handle heap

  return 0;
}

extern void* sorting_worker(void* arg) {
  SortParams *params = (SortParams *) arg;
  int *sub_array = params->sub_array_start;
  int size = params->size;

  // Perform insertion sort on the sub-array
  for (int i = 1; i < size; i++) {
    int key = sub_array[i];
    int j = i - 1;
    while (j >= 0 && sub_array[j] > key) {
      sub_array[j + 1] = sub_array[j];
      j--;
    }
    sub_array[j + 1] = key;
  }

  return NULL;
}