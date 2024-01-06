#include <stdio.h>

#include <pthread.h>

#include <semaphore.h>

#include <stdlib.h>

#include <unistd.h>

//---------------------------- DO NOT TOUCH THIS CODE (lines 8 to 55) -------
// ABSTRACTION LAYER for differences in semaphores between Linux and macOS
//---------------------------- DO NOT TOUCH THIS CODE (lines 8 to 53) -------
#if defined __APPLE__
#include <dispatch/dispatch.h> // macOS ONLY

#endif

#if defined __APPLE__
#define semaphore_t dispatch_semaphore_t
#else
#define semaphore_t sem_t
#endif

semaphore_t semPing, semPong, semPung;

semaphore_t semaphore_create(semaphore_t *semaphore, unsigned int level)
{
#if defined __APPLE__
  return dispatch_semaphore_create((long long)(level));
#else
  sem_init(semaphore, 0, level);
  return *semaphore;
#endif
}

void semaphore_wait(semaphore_t *semaphore)
{
#if defined __APPLE__
  dispatch_semaphore_wait(*semaphore, DISPATCH_TIME_FOREVER);
#else
  sem_wait(semaphore);
#endif
}

void semaphore_signal(semaphore_t *semaphore)
{
#if defined __APPLE__
  dispatch_semaphore_signal(*semaphore);
#else
  sem_post(semaphore);
#endif
}

void semaphore_destroy(semaphore_t *semaphore)
{
#if defined __APPLE__
  dispatch_release(*semaphore);
#else
  sem_destroy(semaphore);
#endif
}
//---------------------------- DO NOT TOUCH THIS CODE (lines 8 to 55) -------

#define ROUNDS 5

semaphore_t sem1, sem2, sem3, sem4, semEnd;

// Function to print "1 " and synchronize with printTwo function via sem1 and sem2
void *printOne(void *arg)
{
  for (int i = 0; i < ROUNDS; ++i)
  {
    semaphore_wait(&sem1);
    printf("1 ");
    fflush(stdout);
    semaphore_signal(&sem2);
    semaphore_wait(&sem1);
    printf("1\n");
    fflush(stdout);
    if (i < ROUNDS - 1)
      semaphore_signal(&sem1); // Do not post on the last round
  }
  semaphore_signal(&semEnd);
  return NULL;
}

// Function to print "2 " and synchronize with printOne and printThree functions via sem2 and sem3
void *printTwo(void *arg)
{
  for (int i = 0; i < ROUNDS; ++i)
  {
    semaphore_wait(&sem2);
    printf("2 ");
    fflush(stdout);
    semaphore_signal(&sem3);
    semaphore_wait(&sem2);
    printf("2 ");
    fflush(stdout);
    semaphore_signal(&sem1);
  }
  return NULL;
}

// Function to print "3 " four times in each round and synchronize with printTwo and printFour functions via sem3 and sem4
void *printThree(void *arg)
{
  for (int i = 0; i < ROUNDS; ++i)
  {
    semaphore_wait(&sem3);
    printf("3 ");
    fflush(stdout);
    semaphore_signal(&sem4);

    semaphore_wait(&sem3);
    printf("3 ");
    fflush(stdout);
    semaphore_signal(&sem4);

    semaphore_wait(&sem3);
    printf("3 ");
    fflush(stdout);
    semaphore_signal(&sem4);

    semaphore_wait(&sem3);
    printf("3 ");
    fflush(stdout);
    semaphore_signal(&sem2); // On the last iteration post to sem2
  }
  return NULL;
}

// Function to print "4 " four times in each round and synchronize with printThree function via sem4
void *printFour(void *arg)
{
  for (int i = 0; i < ROUNDS; ++i)
  {
    semaphore_wait(&sem4);
    printf("4 ");
    fflush(stdout);
    semaphore_signal(&sem3);

    semaphore_wait(&sem4);
    printf("4 ");
    fflush(stdout);
    semaphore_signal(&sem3);

    semaphore_wait(&sem4);
    printf("4 ");
    fflush(stdout);
    semaphore_signal(&sem3);

    semaphore_wait(&sem4);
    printf("4 ");
    fflush(stdout); // Flush the output buffer
    semaphore_signal(&sem3);
  }
  return NULL;
}

// Control thread function to print "SUCCESS!\n" once all rounds are completed by all threads
void *controlThread(void *arg)
{
  semaphore_wait(&semEnd); // Wait on semEnd for completion signal from printOne function
  printf("SUCCESS!\n");
  printf("\n\tdone...\n");
  fflush(stdout);
  return NULL;
}

int main()
{
  pthread_t t1, t2, t3, t4, control;

  // Initialize semaphores using abstraction layer function
  sem1 = semaphore_create(&sem1, 1);
  sem2 = semaphore_create(&sem2, 0);
  sem3 = semaphore_create(&sem3, 0);
  sem4 = semaphore_create(&sem4, 0);
  semEnd = semaphore_create(&semEnd, 0);

  pthread_create(&t1, NULL, printOne, NULL);
  pthread_create(&t2, NULL, printTwo, NULL);
  pthread_create(&t3, NULL, printThree, NULL);
  pthread_create(&t4, NULL, printFour, NULL);
  pthread_create(&control, NULL, controlThread, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  pthread_join(control, NULL);

  // Destroy semaphores using abstraction layer function
  semaphore_destroy(&sem1);
  semaphore_destroy(&sem2);
  semaphore_destroy(&sem3);
  semaphore_destroy(&sem4);
  semaphore_destroy(&semEnd);

  return 0;
}

// Task 1: Create a program that prints five rows of: ping pong
// epw@EPWPC:~/pingpongpung$ ./pingpong
// ping pong
// ping pong
// ping pong
// ping pong
// ping pong
// SUCCESS!

//         done...

// Task 2: Create a program that prints five rows of: ping pong pung
// epw@EPWPC:~/pingpongpung$ ./pingpongpung
// ping pong pung
// ping pong pung
// ping pong pung
// ping pong pung
// ping pong pung
// SUCCESS!

//         done...

// Task 3: Create a program that prints 5 rows of : ping pong pung pyng pung pong
// epw@EPWPC:~/pingpongpung$ ./pingpongpung
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// ping pong pung pyng pung pong
// SUCCESS!

//         done...

// Task 4: Create a program that prints 5 rows of: 1 2 3 4 3 4 3 4 3 2 1
// epw@EPWPC:~/pingpongpung$ ./task4
// 1 2 3 4 3 4 3 4 3 2 1
// 1 2 3 4 3 4 3 4 3 2 1
// 1 2 3 4 3 4 3 4 3 2 1
// 1 2 3 4 3 4 3 4 3 2 1
// 1 2 3 4 3 4 3 4 3 2 1
// SUCCESS!

// done...