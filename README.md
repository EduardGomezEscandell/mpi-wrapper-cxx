# MPI wrapper for C++
This codebase is a proof-of-concept wrapper around MPI C libraries with modern C++ constructs to allow for better code. See some demos:
- [Hello world](demos/helloworld/)
- [Mandelbrot set drawer](demos/mandelbrot/)

### Platform-independent
You can disable MPI if the library is not available in your system, and a mock implementation will run as if you called the program with MPI in a single process.

### Less verbose
Thanks to template argument deduction and constexpr functions.

<table>
<tr>
<td> C library
<td> C++ wrapper </td>
</tr>
<tr>
<td>

```C
MPI_Bcast(data, 5, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
```

</td>
<td>

```c++
comm.broadcast(data, 0);
```

</td>
</tr>
</table>




### Safer
Thanks to concepts, templates, and hiding `void*` away

<table>
<tr>
<td> C library
<td> C++ wrapper </td>
</tr>
<tr>
<td>

```C
unsigned int data[5] = whatever();
MPI_Bcast(data, 5, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
// Runtime silent error! Should be MPI_INT instead
```

</td>
<td>

```c++
std::array<int, 5> data = whatever();
comm.broadcast(data, 0);
// Automatically deducts type is int, and size is 5
```

</td>
</tr>
</table>


### Modern
By using standard containers instead of C-style arrays and pointers.

<table>
<tr>
<td> C library
<td> C++ wrapper </td>
</tr>
<tr>
<td>

```C++
std::vector<unsigned long> data = whatever();
MPI_Bcast(data.data(), 5, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
```

</td>
<td>

```c++
std::vector<unsigned long> data = whatever();
comm.broadcast(data, 0);
```

</td>
</tr>
</table>

### Portable
When desired, the wrapper can be substituted at compile-time with a mock wrapper that mimicks MPI behaviour as though there was only one rank, without actaully using any MPI libraries. This allows you to target applications that work in platforms where MPI is not available, without splitting your codebase. To ensure equal behaviour, the same unit tests run for the MPI and mock wrappers.


## Status
Very, very early stage, assume everything is broken and an ever-braking API.

**DONE**

The following instructions are implemented:
- `MPI_Init` becomes `mpi::environment::initialize`, and is called automatically when a communicator is first used.
- `MPI_Finalize` becomes `mpi::environment::finalize`, and is called automatically upon program termination.
- `MPI_Get_processor_name` becomes `mpi::communicator::processor_name`
- `MPI_Comm_size` becomes `mpi::communicator::size`
- `MPI_Comm_rank` becomes `mpi::communicator::rank`
- `MPI_Barrier` becomes `mpi::communicator::barrier`
- `MPI_Send` becomes `mpi::communicator::send`
- `MPI_Recv` becomes `mpi::communicator::recieve`
- `MPI_Bcast` becomes `mpi::communicator::broadcast`
- `MPI_Gather` becomes `mpi::communicator::gather`

**TODO**

- Implement a wrapper for Windows MPI
- Support for `std::span`
- More instructions.
- More tests
- Use doctest's MPI testing framework

# How-to
## Compile
MPI is only available on Linux (WSL included!).

### Windows
To compile in windows, install Microsoft Visual Studio and find the path to it. In my case it was `C:\Program Files\Microsoft Visual Studio\2022`. Edit file `configure.bat` and overwrite my path with yours. Then run:
```
.\configure.bat
```

### Linux
First you need to install dependencies:
```
sudo apt-get -y install cmake libtbb-dev
sudo apt-get -y install mpich    # Only if you want to compile with MPI
```
Make sure you have GCC version at least 11.2. Otherwise you might need to add some compiler flags to allow for newer C++ features.

Then you can compile, optionally enabling or disabling MPI:
```
MPI_ENABLED=true bash configure.sh
```

### Linker errors?
Note: if you compile it without MPI, and later want to compile it in MPI, you'll get a long list of linker errors. Fix it by doing the following:
```bash
rm -r bin/Release/ build/Release/
MPI_ENABLED=true bash configure.sh
```
Change from Release to Debug if relevant.

## Run
In order to run a demo, check out either of the following:
- [Hello world](demos/helloworld/)
- [Mandelbrot set drawer](demos/mandelbrot/)

## Test
Once compiled, you can run it with:
```Powershell
.\bin\test.exe          # On windows
```
```bash
./bin/test              # On Linux
mpirun -np 4 bin/test   # On Linux with mpi
```
Change the `4` with the number of ranks you desire.
