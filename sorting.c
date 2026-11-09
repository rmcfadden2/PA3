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
void* sorting_worker(void* arg);
void* merging_worker(void* arg);

int main(void) {
  printf("Unsorted input array:\n");
  for (int i = 0; i < SIZE; i++) {
    printf("%d ", input_array[i]);
  }

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
    fprintf(stderr, "Error: Failed to create sorting thread 1 (Code: %d)\n", rc);
    // Handles memory
    free(params1);
    free(params2);
    return EXIT_FAILURE;
  }

  rc = pthread_create(&thread2, NULL, sorting_worker, (void*)params2);
  // Checks for creation errors
  if (rc != 0) {
    fprintf(stderr, "Error: Failed to create sorting thread 2 (Code: %d)\n", rc);

    // Handles memory
    rc = pthread_join(thread1, NULL);
    if (rc != 0) {
      fprintf(stderr, "Error: Failed to join sorting thread 1 (Code: %d)\n", rc);
      return EXIT_FAILURE;
    }
    free(params1);
    free(params2);
    return EXIT_FAILURE;
  }

  // Wait for sorting to happen
  rc = pthread_join(thread1, NULL);
  if (rc != 0) {
    fprintf(stderr, "Error: Failed to join sorting thread 1 (Code: %d)\n", rc);
    return EXIT_FAILURE;
  }

  rc = pthread_join(thread2, NULL);
  if (rc != 0) {
    fprintf(stderr, "Error: Failed to join sorting thread 2 (Code: %d)\n", rc);
    return EXIT_FAILURE;
  }

  // Launch the merge thread
  MergeParams *merge_params = (MergeParams *) malloc(sizeof(MergeParams));
  if (merge_params == NULL) {
    fprintf(stderr, "Error: Failed to allocate memory for merge parameters\n");
    free(params1);
    free(params2);
    return EXIT_FAILURE;
  }

  merge_params->left_chunk = params1;
  merge_params->right_chunk = params2;
  rc = pthread_create(&thread3, NULL, merging_worker, (void*) merge_params);
  if(rc != 0) {
    fprintf(stderr, "Error: Failed to create merging thread (Code: %d)\n", rc);
    free(params1);
    free(params2);
    free(merge_params);
    return EXIT_FAILURE;
  }
  
  // Wait for merge and handle heap
  rc = pthread_join(thread3, NULL);
  if (rc != 0) {
    fprintf(stderr, "Error: Failed to join merging thread (Code: %d)\n", rc);
    return EXIT_FAILURE;
  }

  printf("\nSorted result array:\n");
  for (int i = 0; i < SIZE; i++) {
    printf("%d ", result_array[i]);
  }
  printf("\n");

  free(params1);
  free(params2);
  free(merge_params);

  return 0;
}

void* sorting_worker(void* arg) {
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
void* merging_worker(void* arg) {
  const MergeParams *params = arg;
  const SortParams *left = params->left_chunk;
  const SortParams *right = params->right_chunk;
  int i = 0, j = 0, k = 0;

  // Merge the two sorted chunks into result_array.
  while (i < left->size && j < right->size) {
    if (left->sub_array_start[i] <= right->sub_array_start[j]) {
      result_array[k++] = left->sub_array_start[i++];
    } else {
      result_array[k++] = right->sub_array_start[j++];
    }
  }

  // Copy any remaining elements.
  while (i < left->size) {
    result_array[k++] = left->sub_array_start[i++];
  }
  while (j < right->size) {
    result_array[k++] = right->sub_array_start[j++];
  }

  return NULL;
}
