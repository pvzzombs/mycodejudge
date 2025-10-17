#!/bin/sh
mount -t cgroup2 none /sys/fs/cgroup
echo "+cpu +io +memory +pids" > /sys/fs/cgroup/cgroup.subtree_control 2>/dev/null || true
cd /sys/fs/cgroup
mkdir guest
cd guest
echo 20M > memory.max
echo 20 > pids.max
echo 50000 100000 > cpu.max