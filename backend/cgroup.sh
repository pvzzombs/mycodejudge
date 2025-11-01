#!/bin/sh
mount -t cgroup2 none /sys/fs/cgroup
echo "+cpu +io +memory +pids" > /sys/fs/cgroup/cgroup.subtree_control 2>/dev/null || true
cd /sys/fs/cgroup
mkdir ccpplang
cd ccpplang
echo 50M > memory.max
echo 50 > pids.max
echo 50000 100000 > cpu.max
cd ..
mkdir javalang
cd javalang
echo 512M > memory.max
echo 50 > pids.max
echo 50000 100000 > cpu.max
