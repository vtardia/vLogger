/**
 * Copyright (C) 2022 Vito Tardia
 *
 * This file is part of vLogger.
 *
 * vLogger is a simple C logging utility which aims to be
 * fast and safe.
 *
 * vLogger is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

/**
 * Threads example
 *
 * Launches a bunch of threads sharing the output,
 * each thread writes something at a random interval.
 */

#include "../vlogger.h"

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

/// Termination flag
static bool terminate = false;

/**
 * Runs a dummy loop
 */
void* run(void* data);

/**
 * Handles SIGINT and SIGTERM
 */
void stop(int signal);

/**
 * Installs a signal handler
 */
int catch(int sig, void (*handler)(int));


/**
 * Starts a customisable number of dummy threads
 * and waits for them to finish
 */
int main(int argc, char const *argv[]) {
  size_t maxThreads = 10;

  if (argc > 2) {
    printf("Usage: %s [num-threads]\n", argv[0]);
    return EXIT_FAILURE;
  }

  if (argc == 2) {
    maxThreads = strtol(argv[1], NULL, 10);
  }

  if (!vLogInit(LOG_DEFAULT, NULL)) {
    fprintf(stdout, "Unable to initialise the log engine: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  FatalIf(maxThreads == 0, "No threads");

  FatalIf(maxThreads > 50, "Too many threads (%zu)", maxThreads);

  Info("Launching %zu threads", maxThreads);

  // Install signal handlers
  catch(SIGINT, stop);
  catch(SIGTERM, stop);

  // Start the threads
  pthread_t threadId[maxThreads];
  int values[maxThreads];
  for(size_t i = 0; i < maxThreads; i++) {
    values[i] = i;
    pthread_create(&threadId[i], NULL, run, &values[i]);
  }

  // Wait for the threads to finish
  Info("[Main] waiting... %lu", (unsigned long)pthread_self());
  for(size_t j = 0; j < maxThreads; j++) {
    int res = pthread_join(threadId[j], NULL);
    InfoIf(res == 0, "[Main] thread %d joined!", j);
    ErrorIf(res != 0, "[Main] unable to join thread %d: %s", j, strerror(errno));
  }

  Info("[Main] done!");
  return EXIT_SUCCESS;
}


/**
 * Infinite loop that output random numbers
 * and sleep a random amount of seconds
 */
void* run(void* data) {
  int *id = (int *)data;
  srand(time(NULL));
  Info("[Thread %d] starting %lu", *id, (unsigned long)pthread_self());
  while (!terminate) {
    int delay = rand() % 5;
    Info("[Thread %d] working hard: %d", *id, rand());
    sleep(delay);
  }
  return NULL;
}

/**
 * Sets the termination flag and logs a message
 */
void stop(int signal) {
  terminate = true;
  Info(
    "Received signal %d in thread %lu",
    signal, (unsigned long)pthread_self()
  );
}

int catch(int sig, void (*handler)(int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;
   return sigaction (sig, &action, NULL);
}
