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

semaphore_t semaphore_create(semaphore_t * semaphore, unsigned int level) {
  #if defined __APPLE__
  return dispatch_semaphore_create((long long)(level));
  #else
  sem_init(semaphore, 0, level);
  return * semaphore;
  #endif
}

void semaphore_wait(semaphore_t * semaphore) {
  #if defined __APPLE__
  dispatch_semaphore_wait( * semaphore, DISPATCH_TIME_FOREVER);
  #else
  sem_wait(semaphore);
  #endif
}

void semaphore_signal(semaphore_t * semaphore) {
  #if defined __APPLE__
  dispatch_semaphore_signal( * semaphore);
  #else
  sem_post(semaphore);
  #endif
}

void semaphore_destroy(semaphore_t * semaphore) {
  #if defined __APPLE__
  dispatch_release( * semaphore);
  #else
  sem_destroy(semaphore);
  #endif
}
//---------------------------- DO NOT TOUCH THIS CODE (lines 8 to 55) -------

#define ITERATIONS 5

semaphore_t semPing, semPong, semPung;

void * ping(void * x) {
  for (int i = 0; i < ITERATIONS; i++) {
    semaphore_wait( & semPing);
    printf("ping ");
    semaphore_signal( & semPong);
  }
  return NULL;
}

void * pong(void * x) {
  for (int i = 0; i < ITERATIONS; i++) {
    semaphore_wait( & semPong);
    printf("pong ");
    semaphore_signal( & semPung);
  }
  return NULL;
}

void * pung(void * x) {
  for (int i = 0; i < ITERATIONS; i++) {
    semaphore_wait( & semPung);
    printf("pung\n");
    semaphore_signal( & semPing);
  }
  return NULL;
}

int main(int argc,
  const char * argv[]) {
  pthread_t tid0, tid1, tid2;
  pthread_attr_t attr;

  semPing = semaphore_create( & semPing, 1);
  semPong = semaphore_create( & semPong, 0);
  semPung = semaphore_create( & semPung, 0);

  pthread_attr_init( & attr);
  pthread_create( & tid0, & attr, ping, NULL);
  pthread_create( & tid1, & attr, pong, NULL);
  pthread_create( & tid2, & attr, pung, NULL);

  pthread_join(tid0, NULL);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  semaphore_destroy( & semPing);
  semaphore_destroy( & semPong);
  semaphore_destroy( & semPung);

  printf("SUCCESS!\n");
  printf("\n\tdone...\n");

  return 0;
}


// erik@epw80pc:~/Desktop/pingpong$ ./pingpongpung
// ping pong pung
// ping pong pung
// ping pong pung
// ping pong pung
// ping pong pung
// SUCCESS!

//         done...