# pmem_hook

external dependencies: glib 2.0

build:
make

build and run tests:
make test

To check if it works:
update testconfig.sh with TEST_LD_PRELOAD=\<PathTo\>/libhook.so and run tests

by default log file location: \#define HOOK_LOG_FILE "/tmp/hook.log"

