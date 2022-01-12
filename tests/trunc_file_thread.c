#include "../fs/operations.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define GRN "\x1B[32m"
#define RESET "\x1B[0m"
#define NUM_THREADS 20


char *buffer = "Lorem ipsum, or lipsum as it is sometimes known, is dummy text used in laying out print, graphic or web designs. The passage is attributed to an unknown typesetter in the 15th century who is thought to have scrambled parts of Cicero's De Finibus Bonorum et Malorum for use in a type specimen book.";
char *path = "/f1";

void *write_thread() {
  int fd = tfs_open(path, TFS_O_TRUNC);
  assert(fd != -1);
  ssize_t r = tfs_write(fd, buffer, strlen(buffer));
  assert(tfs_close(fd) != -1);
  assert(r == strlen(buffer));
  return NULL;
}

int main() {
  assert(tfs_init() != -1);

  int fd = tfs_open(path, TFS_O_CREAT);
  assert(fd != -1);
  assert(tfs_close(fd) != -1);
  
  pthread_t tid[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i++) {
    if (pthread_create(&tid[i], NULL, write_thread, NULL) != 0) {
      perror("pthread_create error");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    if (pthread_join(tid[i], NULL) != 0) {
      perror("pthread_join error");
      exit(EXIT_FAILURE);
    }
  }

  assert((fd = tfs_open(path, 0)) != -1);
  char *aux_buf = malloc(BLOCK_SIZE); // BLOCK_SIZE works here, could be another
  ssize_t bytes_read = tfs_read(fd, aux_buf, BLOCK_SIZE);
  assert(bytes_read == strlen(buffer)); // if correctly truncated, only one buffer should be written
  assert(tfs_close(fd) != -1);

  assert(tfs_destroy() != -1);

  printf(GRN "Successful test\n" RESET);

  exit(EXIT_SUCCESS);
}