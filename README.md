# Semaphore implementation

This assignment involves creating a multi-threaded program that prints out specific patterns like "ping, pong, pung" in a deterministic manner. The output should be consistent across multiple runs. This is achieved by the use of semaphores to control the order of printing by different threads. The assignment is a practical exercise in interprocess or inter-thread communication, focusing on concepts such as Producer/Consumer, Reader/Writer, and Dining Philosophers problems.

## Requirements

- Language: C programming
- Environment: Compatible with macOS and Linux

## Key Concepts

- Semaphores: Utilize provided semaphore abstraction layer functions like semaphore_create(), semaphore_wait(), semaphore_signal(), semaphore_destroy(), and semaphore_t. Avoid using Linux or macOS-specific semaphore functions.
- Multi-threading: Implement a deterministic multi-threaded program with each thread running a distinct function.
- Thread Management: Use pthread_t for each thread and a shared pthread_attr_t for thread attributes.
- Critical Section Management: Control access to critical sections using semaphore_wait() and semaphore_signal().

## Running the program

- Compile the code using a C compiler like gcc. Since the program uses the pthread library, you need to link it during compilation. Use the following command:

```bash
gcc -o pingpongpung pingpongpung.c -lpthread
```

This command will compile the code and create an executable named pingpongpung.

## Demo

![example demo](./images/semaphore.gif)

## Contributor

Erik Williams
