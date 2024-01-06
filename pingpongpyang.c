#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//---------------------------- DO NOT TOUCH THIS CODE (lines 8 to 55) -------
// ABSTRACTION LAYER for differences in semaphores between Linux and macOS
//---------------------------- DO NOT TOUCH THIS CODE (lines 8 to 53) -------
#if defined __APPLE__
#include <dispatch/dispatch.h>  // macOS ONLY

#endif

#if defined __APPLE__
#define semaphore_t dispatch_semaphore_t
#else
#define semaphore_t sem_t
#endif

semaphore_t semPing, semPong, semPung;

semaphore_t semaphore_create(semaphore_t* semaphore, unsigned int level) {
#if defined __APPLE__
  return dispatch_semaphore_create((long long)(level));
#else
  sem_init(semaphore, 0, level);
  return *semaphore;
#endif
}

void semaphore_wait(semaphore_t* semaphore) {
#if defined __APPLE__
  dispatch_semaphore_wait(*semaphore, DISPATCH_TIME_FOREVER);
#else
  sem_wait(semaphore);
#endif
}

void semaphore_signal(semaphore_t* semaphore) {
#if defined __APPLE__
  dispatch_semaphore_signal(*semaphore);
#else
  sem_post(semaphore);
#endif
}

void semaphore_destroy(semaphore_t* semaphore) {
#if defined __APPLE__
  dispatch_release(*semaphore);
#else
  sem_destroy(semaphore);
#endif
}
//---------------------------- DO NOT TOUCH THIS CODE (lines 8 to 55) -------

#define PINGS 5
#define PONGS PINGS

semaphore_t semPing, semPong, semPung, semPyng, semPung2, semPong2;

// Semaphore creation, waiting, signaling and destroying functions would be same
// as in your code

void* ping(void* x) {
  int pings = PINGS;
  while (pings-- > 0) {
    semaphore_wait(&semPing);
    printf("ping");
    semaphore_signal(&semPong);
  }
  return NULL;
}

void* pong(void* x) {
  int pongs = PONGS;
  while (pongs-- > 0) {
    semaphore_wait(&semPong);
    printf(" pong");
    semaphore_signal(&semPung);
  }
  return NULL;
}

void* pung(void* x) {
  int pungs = PINGS;
  while (pungs-- > 0) {
    semaphore_wait(&semPung);
    printf(" pung");
    semaphore_signal(&semPyng);
  }
  return NULL;
}

void* pyng(void* x) {
  int pyngs = PINGS;
  while (pyngs-- > 0) {
    semaphore_wait(&semPyng);
    printf(" pyng");
    semaphore_signal(&semPung2);
  }
  return NULL;
}

void* pung2(void* x) {
  int pungs = PINGS;
  while (pungs-- > 0) {
    semaphore_wait(&semPung2);
    printf(" pung");
    semaphore_signal(&semPong2);
  }
  return NULL;
}

void* pong2(void* x) {
  int pongs = PONGS;
  while (pongs-- > 0) {
    semaphore_wait(&semPong2);
    printf(" pong\n");  // Print a new line after the last word
    semaphore_signal(&semPing);
  }
  return NULL;
}

int main(int argc, const char* argv[]) {
  pthread_t pinger, ponger, punger, pynger, punger2, ponger2;
  pthread_attr_t attr;

  semPing = semaphore_create(&semPing, 1);
  semPong = semaphore_create(&semPong, 0);
  semPung = semaphore_create(&semPung, 0);
  semPyng = semaphore_create(&semPyng, 0);
  semPung2 = semaphore_create(&semPung2, 0);
  semPong2 = semaphore_create(&semPong2, 0);

  pthread_attr_init(&attr);
  pthread_create(&ponger, &attr, pong, NULL);
  pthread_create(&pinger, &attr, ping, NULL);
  pthread_create(&punger, &attr, pung, NULL);
  pthread_create(&pynger, &attr, pyng, NULL);
  pthread_create(&punger2, &attr, pung2, NULL);
  pthread_create(&ponger2, &attr, pong2, NULL);

  pthread_join(pinger, NULL);
  pthread_join(ponger, NULL);
  pthread_join(punger, NULL);
  pthread_join(pynger, NULL);
  pthread_join(punger2, NULL);
  pthread_join(ponger2, NULL);

  semaphore_destroy(&semPing);
  semaphore_destroy(&semPong);
  semaphore_destroy(&semPung);
  semaphore_destroy(&semPyng);
  semaphore_destroy(&semPung2);
  semaphore_destroy(&semPong2);

  printf("SUCCESS!\n");
  printf("\n\tdone...\n");
  return 0;
}

// erik@epw80pc:~/Desktop/pingpong$ ./pingpongpyang
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// SUCCESS!

//         done...