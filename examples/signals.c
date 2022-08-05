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
 * Signals example
 *
 * A main program forks into child process
 * The child process
 *  - writes random log lines in an infinite loop
 *  - handles SIGUSR1 by logging an info line
 *  - handles SIGTERM by setting the termination flag
 * The main program:
 *  - handles SIGINT and SIGTERM:
 *     * sets termination flag
 *     * sends SIGTERM to the child
 *  - logs something witin an infinite loop
 *  - at random intervals floods the child with SIGUSR1
 */

#include "../vlogger.h"

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

/// Termination flag
static bool terminate = false;

/**
 * Info flag to distinguish the parent
 * from the child within the common
 * signal handlers
 */
static bool isParent = true;

/**
 * Handles SIGINT and SIGTERM
 */
void stop(int signal);

/**
 * Handles SIGUSR* (child)
 */
void usr(int signal);

/**
 * Installs a signal handler
 */
int catch(int sig, void (*handler)(int));

/**
 * Runs the parent code
 */
int parent(pid_t child);

/**
 * Runs the child code
 */
int child();

/**
 * Initialises the log and start the parent and child process
 */
int main(int argc, char const *argv[]) {
  char *logFilePath = NULL;

  FatalIf(argc > 2, "Invalid arguments count: %d", argc);

  if (argc == 2) {
    logFilePath = (char *)argv[1];
    fprintf(stdout, "Log file path is: %s\n", logFilePath);
  } else {
    fprintf(stdout, "No file selected, logging to STDERR\n");
  }

  if (!vLogInit(LOG_DEFAULT, logFilePath)) {
    fprintf(stdout, "Unable to initialise the log engine: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // Install the common signal handlers
  catch(SIGTERM, stop);
  srand(time(NULL));

  pid_t worker = fork();
  if (worker > 0) {
    return parent(worker);
  }

  return child();
}

int parent(pid_t child) {
  Info("[parent] created child with pid %d", child);

  // Installs SIGINT handler
  catch(SIGINT, stop);

  // Define an interval in milliseconds
  struct timespec ts;
  int msec = 10;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;

  // Dummy working loop
  while(!terminate) {
    int delay = rand() % 5;
    Info("[parent] working hard: %d", rand());

    // 60% possibility to flood the child
    if (delay < 4) {
      Info("[parent] flooding child: %d%%", delay * 20);
      for (int i = 0; i < 20; i++) {
        kill(child, SIGUSR1);
        nanosleep(&ts, NULL);
      }
    }

    sleep(delay);
  }

  // Try to kill the child on exit
  int res = kill(child, SIGTERM);
  InfoIf(res == 0, "[parent] child %d stopped!", child);
  ErrorIf(res < 0, "[parent] unable to stop child %d: %s", child, strerror(errno));

  return EXIT_SUCCESS;
}

int child() {
  isParent = false;

  // Ignore SIGINT (left to the parent)
  catch(SIGINT, SIG_IGN);

  // Handle SIGUSR1
  catch(SIGUSR1, usr);

  // Dummy working loop
  Info("[child] worker started");
  while(!terminate) {
    int delay = rand() % 5;
    Info("[child] working hard: %d", rand());
    sleep(delay);
  }

  Info("[child] worker stopped");
  return EXIT_SUCCESS;
}

/**
 * Sets the termination flag and logs a message
 */
void stop(int signal) {
  terminate = true;
  Info(
    "[%s] received signal %d in pid %d",
    (isParent ? "parent" : "child"), signal, getpid()
  );
}

/**
 * Handles SIGUSR* by logging a message
 */
void usr(int signal) {
  (void)signal;
  Info("[child] received SIGUSR*");
}

int catch(int sig, void (*handler)(int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;
   return sigaction (sig, &action, NULL);
}
