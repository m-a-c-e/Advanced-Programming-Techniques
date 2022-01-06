#VARIABLES
SRC_C       = Lab6_mpi.cpp
EXECUTABLE  = Lab6_mpi.out
E_OUT       = myscript.err
F_OUT	    = Temperatures.csv
CC          = mpic++
CLEAN_FILES = $(EXECUTABLE) $(E_OUT) $(F_OUT) $(E_OUT)
PBS         = Lab6_mpi.pbs
	
#RULES
$(EXECUTABLE): $(SRC_C)
	$(CC) $(SRC_C) -o Lab6_mpi.out

submit_job: $(EXECUTABLE)
	qsub $(PBS)

clean: $(EXECUTABLE) $(E_OUT)
	rm -f $(CLEAN_FILES)

Lab6_mpi: $(PBS)
	rm -f $(CLEAN_FILES) && $(CC) $(SRC_C) -o $(EXECUTABLE) && qsub $(PBS)







