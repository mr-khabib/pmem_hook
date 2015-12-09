# pmem_hook

build:
Just "make"

To check if it works:
update testconfig.sh with TEST_LD_PRELOAD=<PathTo>/libhook.so and run tests

buy the default logfile location: 
#define HOOK_LOG_FILE "/tmp/hook.log"

