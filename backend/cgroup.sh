#!/bin/sh
mount -t cgroup2 none /sys/fs/cgroup
echo "+cpu +io +memory +pids" > /sys/fs/cgroup/cgroup.subtree_control 2>/dev/null || true
cd /sys/fs/cgroup
mkdir guest
cd guest
echo 5M > memory.max