#CC=$(CROSS)gcc

OBJS=parentControl.o

parentControl: $(OBJS) 
	$(CC) $(OBJS) -o $@
	chmod -R 777 ./*


#all: parentControl 

.o:
	$(CC) -c $(CFLAGS) stdio.h string.h


clean:
	rm -f *.o *.a parentControl 
