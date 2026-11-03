#pragma once

#include "includes.hpp"

#if defined(_WIN32)
#error "Not Yet Supported!"
#else
#define COMPILE_COMMAND_CPP "podman run --pull=never --network=none --volume=%s%s:/workspace:rw --workdir=/workspace mycodejudge g++ %s.cpp -o %s.out 2>&1"
#define COMPILE_COMMAND_C "podman run --pull=never --network=none --volume=%s%s:/workspace:rw --workdir=/workspace mycodejudge gcc %s.c -o %s.out 2>&1"
#define COMPILE_COMMAND_JAVA "export PATH=$PATH:/opt/jdk/bin; javac %s.java 2>&1"

#define RUN_COMMAND_CPP "podman run --pull=never --network=none --read-only --memory=256m --cpus=1 --pids-limit=64 --timeout=10 --volume=%s%s:/workspace:ro --workdir=/workspace mycodejudge sh -c './%s.out < %s.txt' 2>&1"
#define RUN_COMMAND_C "podman run --pull=never --network=none --read-only --memory=256m --cpus=1 --pids-limit=64 --timeout=10 --volume=%s%s:/workspace:ro --workdir=/workspace mycodejudge sh -c './%s.out < %s.txt' 2>&1"
#define RUN_COMMAND_JAVA "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/javalang/cgroup.procs; unshare --mount --net --ipc --uts chroot %s /bin/su - %s -c '\\''java -Xmx32m -Xms1m -Xss256k %s'\\'' < %s.txt 2>&1'"

#define COMPILE_COMMAND_EXEC_CPP sprintf(buffer_cmd, COMPILE_COMMAND_CPP, FAKESYSTEMLOCATION, FAKESYSTEMLOCATIONHOME, extractedBaseName.c_str(), extractedBaseName.c_str());
#define COMPILE_COMMAND_EXEC_C sprintf(buffer_cmd, COMPILE_COMMAND_C, FAKESYSTEMLOCATION, FAKESYSTEMLOCATIONHOME, extractedBaseName.c_str(), extractedBaseName.c_str());
#define COMPILE_COMMAND_EXEC_JAVA sprintf(buffer_cmd, COMPILE_COMMAND_JAVA, baseName.c_str());

#define RUN_COMMAND_EXEC_CPP sprintf(buffer_cmd, RUN_COMMAND_CPP, FAKESYSTEMLOCATION, FAKESYSTEMLOCATIONHOME, name.c_str(), name.c_str())
#define RUN_COMMAND_EXEC_C sprintf(buffer_cmd, RUN_COMMAND_C, FAKESYSTEMLOCATION, FAKESYSTEMLOCATIONHOME, name.c_str(), name.c_str())
#define RUN_COMMAND_EXEC_JAVA sprintf(buffer_cmd, RUN_COMMAND_JAVA, FAKESYSTEMLOCATION, FAKESYSTEMUSER, baseNameNoExtension.c_str(), baseName.c_str())
#endif
