# MPI demo
Simple demo for multi-platform development using a mock MPI implementation on Windows.

# Compile
MPI is only available on Linux (WSL included!).

## Windows
To compile in windows, install Microsoft Visual Studio and find the path to it. In my case it was `C:\Program Files\Microsoft Visual Studio\2022`. Edit file `configure.bat` and overwrite my path with yours. Then run:
```
.\configure.bat
```

## Linux
```
MPI_ENABLED=true bash configure.sh
```
Choose MPI_ENABLED between `true` or `false` to enable or disable compilation in MPI. Not that to compile with MPI you'll need to install package `mpich`.

# Run
Once compiled, you can run it with:
```Powershell
.\bin\mpidemo.exe          # On windows
```
```bash
./bin/mpidemo              # On Linux
mpirun -np 4 bin/mpidemo   # On Linux with mpi
```
Change the `4` with the number of ranks you desire. Note that if you mpirun an executable compiled without MPI, it's simply going run the same process in each rank independently.
