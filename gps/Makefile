DIR = ~/Desktop/gpsd-3.5-pc
ARM = gcc

# DIR = ~/Desktop/gpsd-3.7-arm
# ARM = arm-none-linux-gnueabi-gcc
INC = -I $(DIR)
C_FLAGS = -c $(INC)
OBJS = gps_test.o
LIBS = -L $(DIR) -lgps

gps_test: $(OBJS)
	$(ARM) -o gps_test $(OBJS) $(LIBS) 

gps_test.o: gps_test.c
	$(ARM) $(C_FLAGS) gps_test.c

clean: 
	rm -f gps_test *~ *.o

.IGNORE: clean
