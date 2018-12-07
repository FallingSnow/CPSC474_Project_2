# Tree Leader Selection Algorithm
California State University Fullerton<br>
CPSC 474: Parallel and Distributed Computing<br>
Created by: *Adam Shirley* & *Ayrton Sparling*

## Dependencies
Make sure these dependencies are in your PATH. You can check by using `which {command}`. For example `which ninja` should return the path to your currently install version of ninja. You can usually check the version of the installed binary using `{command} --version`. For example `g++ --version`.
* [Cmake](https://cmake.org/) 3.1
* C++17 Compiler ([GCC](https://www.gnu.org/software/gcc/) 7 | [Clang](https://clang.llvm.org/) 5)
* [Ninja](https://ninja-build.org/) 1.8
* [OpenMPI](https://www.open-mpi.org/) 3.1

## Getting the code
```sh
git clone git@github.com:FallingSnow/CPSC474_Project_2.git
cd CPSC474_Project_2
```

## Compiling
This will create a LeaderSelection executable in the build directory.

```sh
$ mkdir -p build && cd build    # Create a build directory
$ cmake -GNinja ../             # Generate build files
$ ninja                         # Compile
```

## Running
After compiling the program, you may run the compiled executable.

```sh
$ cd build                       # Change to the build directory
$ mpirun -n 8 Leader_Selection   # Where 8 is the number of processes you want to run
```
