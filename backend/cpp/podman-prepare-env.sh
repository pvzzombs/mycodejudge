#!/bin/sh

confirm() {
  read -rp "$1 [y|N]: " answer
  [[ "$answer" == "y" || "$answer" == "Y" ]]
}

if [[ $EUID -ne 0 ]]; then
  echo "Error: Please run this script as root!"
  exit 1
fi

if confirm "Start Cgroups2?"; then
  mkdir -p /sys/fs/cgroup
  mount -t cgroup2 none /sys/fs/cgroup
  ls -l /sys/fs/cgroup
  echo "Cgroup2 mounted!"
else
  echo "Cgroup2 not mounted!"
fi
