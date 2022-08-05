# vLogger

vLogger is a simple C log implementation library which aims to be fast and safe.

## Install

vLogger has currently been tested on GNU/Linux and macOS only.

Run `make && make install` to compile and install the components using the default directory prefix (`/usr/local`).

Alternatively, run `make && make install -e PREFIX=/custom/path` to install the components with a custom directory prefix. If the path is relative to your user's home directory, it's best to use `-e PREFIX=$HOME/some/path`.

The static library will be installed into `$PREFIX/lib/libvlogger.a` and the header file into `$PREFIX/include/vlogger.h`.

When compiling your own programs, use the `-I $PREFIX/include` and `-L $PREFIX/lib` GCC compiler options and `-lvlogger` options within your `Makefile`s or directly into the command line.

## Usage

```c
#include <vlogger.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  char *logFilePath = NULL;
  if (argc == 2) {
    logFilePath = (char *)argv[1];
  }

  // Initialise vLogger here...
  if (!vLogInit(LOG_INFO, logFilePath)) {
    fprintf(stderr, "Unable to initialise the log engine: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // The rest of your code here...

  Info("This is an info message with no arguments");
  Warn("This is a warning message regarding %s", someVariable);

  WarnIf((otherVar < 42), "Something strange happened to 'otherVar' %d", otherVar);

  // This will stop the program and return errno or EXIT_FAILURE
  Fatal("An unrecoverable error happened");

  return EXIT_SUCCESS;
}
```

See also the [examples](./examples/) directory for more examples.

## Date format

vLogger uses the [ISO 8601](https://en.wikipedia.org/wiki/ISO_8601) date format (local date/time with UTC offset). The output format of an event is:

```
YYYY-MM-DDTHH:MM:SS+ZZZZ | <PID> | <ThreadID> | <LEVEL> | <Your Message>
```

For example:

```
2022-04-07T16:09:33+0100 | 1234  | 5678910111 | INFO    | This thing happened
```

## Thread and Signal safety

vLogger writes the message to the log stream using the AS-Safe (async-safe) `write()` system call. The other intermediate functions are all MT-Safe (thread-safe):

 - `localtime_r()`: MT-Safe
 - `strftime()`: MT-Safe as long as no other threads call `setlocale()` while this function is executing
 - `snprintf()`: MT-Safe as long as the buffer is not shared with other threads
 - `strlen()`: MT-Safe
 - `time()`: MT-Safe
 - `getpid()`: MT-Safe and AS-Safe
 - `pthread_self()`: MT-Safe and AS-Safe

## Run the tests

Run `make tests`.

## Play with the examples

Run `make examples`, you will find each example compiled under `bin/examples/`.

## License

vLogger is licensed under LGPL. Please refer to the [LICENSE](./LICENSE) file for detailed information.
