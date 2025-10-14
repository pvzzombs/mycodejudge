# Using the C++ backend
## Requirements:
### Operating System
- GNU/Linux
#### Feautures Needed
- chroot
- cgroup2
- timeout
- sh
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
## Steps
### Preparing for steps
1. Go to the `backend` folder
### Preparing the database
2. Open `main.cpp` file, edit `OUTPUTLOCATION` to point to your sqlite database directory
3. Execute `prepare.sql` to your sqlite database, this prepares the tables for you
### Preparing cgroup2
4. Open `cgroup.sh`, change the cgroup2 directory or other things to suite your needs
5. Using `sudo` or running as a root user, execute `cgroup.sh`
### Compilation of backend server
5. Open `main.cpp`, edit `FAKESYSTEMLOCATION` to point to chroot sandbox directory, ensure there is no slash on the end of the path
6. Open `nrp.hpp`, edit `OUTPUT_FILE` and `INPUT_FILE` to where you store the files to be use for backend communication
7. Open `rootrunner.cpp`, edit `OUTPUT_FILE`, `INPUT_FILE` similarly to the above step, it should match the `nrp.hpp` macro definitions
8. Open `rootrunner.cpp`, edit `FAKESYSTEMLOCATION` similarly to step 1, it should match the `main.cpp` macro definition
9. Run `compile.sh` normally
### Running the backend
10. Ensure cgroup2 is functioning
11. Run the main server file, it has name `a.out`, run it first
12. After running `a.out`, run `rootrunner.out` as root or using `sudo`
### Running the frontend (not part of this guide)
13. Ensure `node` and `yarn` are installed and working
14. Using `yarn`, run `yarn run dev --host`
### Other steps to do (Optional)
- If you have a firewall running, you may allow your local network to access your backend and frontend
- Ensure your cgroup2 is working fine, if not, check first if your kernel supports it
- If you don't have `yarn` installed, you can install it or use other package manager
