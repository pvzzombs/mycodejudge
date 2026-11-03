# MyCodeJudge

A lightweight (offline) code judge system

## Features:

- Offline code judge system, does not need internet, web dependencies
  are bundled for offline use after installation
- Execution of (unsafe) code are handled using Podman (**NEW!**)
- Execution of (unsafe) code are handled using chroot and cgroup2
- Supports checking of code using (multiple) custom testcase/s
- Currently only supports: `C`, `C++`, and `Java`

## Tech Stack

Currently, MyCodeJudge frontend was powered by [Vite](https://vite.dev/)
and MyCodeJudge backend was using C++, although its possible to port the
code into different language, as of now only few are planned.

## How It Works
### Without Podman
1. User submits the code
2. Code is compiled
3. Code is put into sandbox
4. Code is executed into sandbox
5. Program runs with constraints
6. Compilation and output returned is captured
7. Output sent to the user
### With Podman
1. User submits the code
2. Code is put into container and compiled
3. Compiled code is put into container and executed
4. Output/s is/are captured and returned
5. Ouptut/s is/are sent to the user

## TODO
- Port C++ code into Java with SpringBoot
- Use different web framework technology on the frontend
