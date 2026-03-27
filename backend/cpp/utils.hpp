#pragma once

#if defined(_WIN32)
#error "Not Yet Supported!"
#else
#define COMPILE_COMMAND_CPP "g++ %s.cpp -o %s.out 2>&1"
#define COMPILE_COMMAND_C "gcc %s.c -o %s.out 2>&1"
#define COMPILE_COMMAND_JAVA "export PATH=$PATH:/opt/jdk/bin; javac %s.java 2>&1"

#define RUN_COMMAND_CPP "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/ccpplang/cgroup.procs; unshare --mount --pid --net --ipc --uts chroot %s /bin/su - %s -c '\\''%s.out'\\'' < %s.txt 2>&1'"
#define RUN_COMMAND_C "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/ccpplang/cgroup.procs; unshare --mount --pid --net --ipc --uts chroot %s /bin/su - %s -c '\\''%s.out'\\'' < %s.txt 2>&1'"
#define RUN_COMMAND_JAVA "timeout 2s sh -c 'echo $$ > /sys/fs/cgroup/javalang/cgroup.procs; unshare --mount --net --ipc --uts chroot %s /bin/su - %s -c '\\''java -Xmx32m -Xms1m -Xss256k %s'\\'' < %s.txt 2>&1'"

#define COMPILE_COMMAND_EXEC_CPP sprintf(buffer_cmd, COMPILE_COMMAND_CPP, baseName.c_str(), baseName.c_str());
#define COMPILE_COMMAND_EXEC_C sprintf(buffer_cmd, COMPILE_COMMAND_C, baseName.c_str(), baseName.c_str());
#define COMPILE_COMMAND_EXEC_JAVA sprintf(buffer_cmd, COMPILE_COMMAND_JAVA, baseName.c_str());

#define RUN_COMMAND_EXEC_CPP sprintf(buffer_cmd, RUN_COMMAND_CPP, FAKESYSTEMLOCATION, FAKESYSTEMUSER, name.c_str(), baseName.c_str())
#define RUN_COMMAND_EXEC_C sprintf(buffer_cmd, RUN_COMMAND_C, FAKESYSTEMLOCATION, FAKESYSTEMUSER, name.c_str(), baseName.c_str())
#define RUN_COMMAND_EXEC_JAVA sprintf(buffer_cmd, RUN_COMMAND_JAVA, FAKESYSTEMLOCATION, FAKESYSTEMUSER, baseNameNoExtension.c_str(), baseName.c_str())
#endif
