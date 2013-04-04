C_FLAGS = -c
ARM = arm-none-linux-gnueabi-gcc
BATT-OBJS = battery_query.o
TEST-OBJS = testDrone_navdata.o 
#TEST-OBJS = manuevers_test.o take_off_land.o drone_comm.o

battery_query: $(BATT-OBJS)
	$(ARM) -o battery_query $(BATT-OBJS)

battery_query.o: battery_query.c
	$(ARM) $(C_FLAGS) battery_query.c

testDrone_navdata: $(TEST-OBJS)
	$(ARM) -o testDrone_navdata $(TEST-OBJS)

testDrone_navdata.o: testDrone_navdata.c
	$(ARM) $(C_FLAGS) testDrone_navdata.c

clean: 
	rm -f battery_query testDrone_navdata *~ *.o

.IGNORE: clean

