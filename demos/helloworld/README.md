# Hello world!
This is a small program that prints stuff to screen.

## How to run

Once compiled, you can run it with:
````powershell
.\bin\mpidemo.exe          # On windows
```
```bash
./bin/mpidemo              # On Linux
mpirun -np 4 bin/mpidemo   # On Linux with mpi
```
Change the 4 with the number of processes you desire. Note that if you mpirun an executable compiled without MPI, it's simply going run the same process in each rank independently.
