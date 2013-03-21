C_FLAGS = -c
ARM = arm-none-linux-gnueabi-gcc
OBJS = testDrone_navdata.o 
#OBJS = manuevers_test.o take_off_land.o drone_comm.o

testDrone_navdata: $(OBJS)
	$(ARM) -o testDrone_navdata $(OBJS)

testDrone_navdata.o: testDrone_navdata.c
	$(ARM) $(C_FLAGS) testDrone_navdata.c

clean: 
	rm -f testDrone_navdata *~ *.o

.IGNORE: clean

