# Using the C++ backend
## Requirements:
### Operating System
- GNU/Linux
#### Feautures Needed
- chroot
- cgroup2
- timeout
- sh
- su
### External Dependencies
- [libsodium](https://doc.libsodium.org/)
- [sqlite3](https://sqlite.org/)
### Bundled Dependencies
- [json](https://github.com/nlohmann/json)
- [SQLiteCpp](https://github.com/geekyMrK/SQLiteCpp)
- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- [loguru](https://github.com/emilk/loguru)
### Target programming languages
- C
- C++
- Java
## Steps
### Preparing for steps
1. Go to the `backend` folder
### Preparing the database
2. Open `main.cpp` file, edit `OUTPUTLOCATION` to point to your sqlite database directory, also edit `ADMINNAME` and `ADMINPASS` for your admin name and password, respectively
3. Execute `prepare.sql` to your sqlite database, this prepares the tables for you
### Prepare chroot sandbox
See below (Should not be part of this guide)
### Preparing cgroup2
4. Open `cgroup.sh`, change the cgroup2 directory or other things to suite your needs
5. Using `sudo` or running as a root user, execute `cgroup.sh`
### Compilation of backend server
6. Open `main.cpp`, edit `FAKESYSTEMLOCATION` to point to chroot sandbox directory, ensure there is no slash on the end of the path, also edit `FAKESYSTEMLOCATIONHOME` which points to the home folder of the sandbox non root user directory and make sure there is slash at the end of the path
7. Open `nrp.hpp`, edit `OUTPUT_FILE` and `INPUT_FILE` to where you store the files to be use for backend communication
8. Open `rootrunner.cpp`, edit `OUTPUT_FILE`, `INPUT_FILE` similarly to the above step, it should match the `nrp.hpp` macro definitions
9. Open `rootrunner.cpp`, edit `FAKESYSTEMLOCATION` similarly to step 6, it should match the `main.cpp` macro definition, also edit `FAKESYSTEMUSER`, it should match the name of the non root user inside the chroot sandbox
10. Run `compile.sh` normally
### Running the backend
11. Ensure cgroup2 is functioning
12. Run the main server file, it has name `a.out`, run it first
13. After running `a.out`, run `rootrunner.out` as root or using `sudo`
### Running the frontend (not part of this guide)
14. Ensure `node` and `yarn` are installed and working
15. Using `yarn`, run `yarn run dev --host`
### Other steps to do (Optional)
- If you have a firewall running, you may allow your local network to access your backend and frontend
- Ensure your cgroup2 is working fine, if not, check first if your kernel supports it
- If you don't have `yarn` installed, you can install it or use other package manager
## Chroot Sandbox  
This should not be part of this guide, but in case, here it is.

- Make sure there is a non root user inside the chroot sandbox
- Make sure the chroot sandbox root directory is owned by `root`
1. Choose a directory, anywhere you want as long as you owned it.
2. Create a new directory named `fakesystem`, this will be the root directory of the chroot sandbox
3. Populate with the directories: `bin`, `home`, `lib`, `lib64`, `usr`, `etc`, `sbin`, `root`; this folders depends on what type of linux os you will be using on chroot
4. Add binaries and shared libraries files to the directories, I recommend using [busybox](https://busybox.net/)
5. Create a new file on `etc` directory with name `passwd`
6. Edit `passwd` file, add the lines below:
```
root:x:0:0:root:/root:/bin/sh
sandbox:x:1000:1000:Sandbox:/home/sandbox:/bin/sh
```
You can change the second line (`sandbox`), to whatever you want, but make sure to not change the first line, the (`root`) should remain as it is

7. Create a new file on `etc` directory with name `group`
8. Edit `group` file, add the lines below:
```
sandbox:x:1000:
```
You can change it to match the name of user you created

9. Create a directory inside home, name it with the user you created on the `passwd` file, for example, `sandbox`
10. Using `sudo` or running as root, execute the following: 
```
chown -R 1000:1000 sandbox
```
11. Go to the `fakesystem` directory, then `cd ..`
12. Using `sudo` or running as root, execute the following:
```
chown -R root:root fakesystem
chown -R 1000:1000 fakesystem/home/sandbox
chmod 1777 fakesystem/home/sandbox
chmod 755 fakesystem
```
13. With that, you should have a minimal working chroot sandbox directory
