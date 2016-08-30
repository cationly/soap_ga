# rematch_soap_ga
### GA parameter optimisation of RE-Match Soap molecule similarity kernel
to build: ``` :~$ make ```

Note that parent directory has to include:  
boost: http://www.boost.org/  
boost.simd: https://github.com/NumScale/boost.simd  
and GA implementation
  
to run: ``` :~$ mpiexec -n <no_of_processes> ./static.exe ```

Number of molecules for training and input dataset file are specified in static.c
