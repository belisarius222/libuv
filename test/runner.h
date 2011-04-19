/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef RUNNER_H_
#define RUNNER_H_


/*
 * The maximum number of processes (main + helpers) that a test / benchmark
 * can have.
 */
#define MAX_PROCESSES 8


/*
 * Struct to store both tests and to define helper processes for tasks.
 */
typedef struct {
  char *task_name;
  char *process_name;
  int (*main)();
  int is_helper;
} task_entry_t, bench_entry_t;


/* Runs an individual task; returns 1 if the test succeeded, 0 if it failed. */
/* If the test fails it prints diagnostic information. */
/* If benchmark_output is nonzero, the output from the main process is
/* always shown. */
int run_task(task_entry_t *test, int timeout, int benchmark_output);


/*
 * Macros used by test-list.h and benchmark-list.h.
 */
#define TASK_LIST_START                           \
  task_entry_t TASKS[] = {

#define TASK_LIST_END                             \
    { 0, 0, 0, 0 }                            \
  };

#define TEST_DECLARE(name)                        \
  int run_test_##name();

#define TEST_ENTRY(name)                      \
    { #name, #name, &run_test_##name, 0 },

#define TEST_HELPER(name, proc)               \
    { #name, #proc, &run_test_##proc, 1 },

#define BENCHMARK_DECLARE(name)                   \
  int run_benchmark_##name();

#define BENCHMARK_ENTRY(name)                     \
    { #name, #name, &run_benchmark_##name, 0 },

#define BENCHMARK_HELPER(name, proc)              \
    { #name, #proc, &run_benchmark_##proc, 1 },

/*
 * Include platform-dependent definitions
 */
#ifdef _WIN32
# include "runner-win.h"
#else
# include "runner-unix.h"
#endif


/* The array that is filled by test-list.h or benchmark-list.h */
extern task_entry_t TASKS[];

/* Start a specific process declared by TEST_ENTRY or TEST_HELPER. */
/* Returns the exit code of the specific process. */
int run_task(task_entry_t *test, int timeout, int benchmark_output);

/* Start a specific process declared by TEST_ENTRY or TEST_HELPER. */
/* Returns the exit code of the specific process. */
int run_process(char* name);


/*
 * Stuff that should be implemented by test-runner-<platform>.h
 * All functions return 0 on success, -1 on failure, unless specified
 * otherwise.
 */

/* Do platform-specific initialization. */
void platform_init();

/* Invoke "arv[0] test-name". Store process info in *p. */
/* Make sure that all stdio output of the processes is buffered up. */
int process_start(char *name, process_info_t *p);

/* Wait for all `n` processes in `vec` to terminate. */
/* Time out after `timeout` msec, or never if timeout == -1 */
/* Return 0 if all processes are terminated, -1 on error, -2 on timeout. */
int process_wait(process_info_t *vec, int n, int timeout);

/* Returns the number of bytes in the stdio output buffer for process `p`. */
long int process_output_size(process_info_t *p);

/* Copy the contents of the stdio output buffer to `fd`. */
int process_copy_output(process_info_t *p, int fd);

/* Return the name that was specified when `p` was started by process_start */
char* process_get_name(process_info_t *p);

/* Terminate process `p`. */
int process_terminate(process_info_t *p);

/* Return the exit code of process p. */
/* On error, return -1. */
int process_reap(process_info_t *p);

/* Clean up after terminating process `p` (e.g. free the output buffer etc.). */
void process_cleanup(process_info_t *p);

/* Move the console cursor one line up and back to the first column. */
int rewind_cursor();

#endif /* RUNNER_H_ */
