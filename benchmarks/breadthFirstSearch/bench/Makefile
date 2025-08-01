include common/parallelDefs
BNCHMRK = BFS

CHECKFILES = $(BNCHMRK)Check.o

COMMON = 

INCLUDE = -Icommon

%.o : %.C $(COMMON)
	time $(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BNCHMRK)Check : $(CHECKFILES)
	time $(CC) $(LFLAGS) -o $@ $(CHECKFILES)

clean :
	rm -f $(BNCHMRK)Check *.o *.pyc
