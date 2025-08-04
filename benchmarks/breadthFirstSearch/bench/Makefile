include common/parallelDefs
BNCHMRK = BFS

CHECKFILES = $(BNCHMRK)Check.o

COMMON = 

INCLUDE = -Icommon

%.o : %.C $(COMMON)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.s : %.C $(COMMON)
	$(CC) $(CFLAGS) $(INCLUDE) -S $< -o $@

$(BNCHMRK)Check : $(CHECKFILES)
	$(CC) $(LFLAGS) -o $@ $(CHECKFILES)

assembly: $(BNCHMRK)Check.s

clean :
	rm -f $(BNCHMRK)Check *.o *.s *.pyc
