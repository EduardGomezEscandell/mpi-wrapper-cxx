# MPI demo
Simple demo for multi-platform development using a mock MPI implementation on Windows.

# Compile
MPI is only available on Linux (WSL included!).

## Windows
To compile in windows, install Microsoft Visual Studio and find the path to it. In my case it was `C:\Program Files\Microsoft Visual Studio\2022`. Edit file `configure.bat` and overwrite my path with yours. Then run:
```
.\configure.bat
```
On linux, do:
```
MPI_ENABLED=true bash configure.sh
```
Set MPI_ENABLED to true or false to choose.

# Run
To run it in a single rank, simply run the executable located inside of `bin`.

To run it in multple ranks, do:
```
mpirun -np 4 bin/mpidemo
```
Chenge the `4` for the number of ranks you desire. Note that if you mpirun an executable compiled without MPI, it's simply goint run the same process in each rank independently.