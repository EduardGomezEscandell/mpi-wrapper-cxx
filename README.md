# MPI for C++
This codebase ofers a wrapper around MPI C libraries with modern C++ constructs to allow for better code:

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
Mpi::broadcast(data, 0);
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
Mpi::broadcast(data, 0);
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
Mpi::broadcast(data, 0);
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
- `MPI_Init` and `MPI_Finalize` are called automatically via RAII
- `MPI_Get_processor_name` becomes `Mpi::processor_name`
- `MPI_Comm_size` becomes `Mpi::size`
- `MPI_Comm_rank` becomes `Mpi::rank`
- `MPI_Barrier` becomes `Mpi::barrier`
- `MPI_Send` becomes `Mpi::send`
- `MPI_Recv` becomes `Mpi::recieve`
- `MPI_Bcast` becomes `Mpi::broadcast`
- `MPI_Gather` becomes `Mpi::gather`

**TODO**

- Implement a wrapper for Windows MPI
- Support for `std::span`
- Support for multiple communicators
- More instructions.
- More tests
- Actual demos
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
sudo apt-get -y install cmake
sudo apt-get -y install mpich    # Only if you want to compile with MPI
```
Make sure you have GCC version at least 11.2. Otherwise you might need to add some compiler flags to allow for newer C++ features.

Then you can compile, optionally enabling or disabling MPI:
```
MPI_ENABLED=true bash configure.sh
```

## Run
Once compiled, you can run it with:
```Powershell
.\bin\mpidemo.exe          # On windows
```
```bash
./bin/mpidemo              # On Linux
mpirun -np 4 bin/mpidemo   # On Linux with mpi
```
Change the `4` with the number of ranks you desire. Note that if you mpirun an executable compiled without MPI, it's simply going run the same process in each rank independently.

## Test
Once compiled, you can run it with:
```Powershell
.\bin\test.exe          # On windows
```
```bash
./bin/test              # On Linux
mpirun -np 4 bin/test   # On Linux with mpi
```
Change the `4` with the number of ranks you desire. Note that if you mpirun an executable compiled without MPI, it's simply going run the same process in each rank independently.
