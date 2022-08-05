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
 * Simple example
 *
 * Initialises the log to either STDERR or a file and
 * writes some lines.
 *
 *  - vlog <no arguments>: writes to STDERR
 *  - vlog <path/to/fole>: tries to write to the given file or
 *    writes the error to the STDOUT
 *  - vlog <more than 1 argument>: exits with fatal error to STDERR
 */

#include "../vlogger.h"

#include <stdlib.h>

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

  Log("This is a default log message with no args");
  Log("This is a default log message with args: %s", argv[0]);

  Trace("This is a trace log with no args");
  Trace("This is a trace log with args: %s", argv[0]);

  Debug("This is a debug log with no args");
  Debug("This is a debug log with args: %s", argv[0]);

  Info("This is an info log with no args");
  Info("This is an info log with args: %s", argv[0]);

  Warn("This is a warning log with no args");
  Warn("This is a warning log with args: %s", argv[0]);

  WarnIf(argc < 2, "Arguments count is just %d", argc);

  Error("This is an error log with no args");
  Error("This is an error log with args: %s", argv[0]);

  ErrorIf(argc < 2, "Arguments count is just %d", argc);

  Fatal("This is a fatal log with no args"); // => will EXIT_FAILURE
  Fatal("This is a fatal log with args: %s", argv[0]);

  return EXIT_SUCCESS;
}
