C_FLAGS = -c
ARM = arm-none-linux-gnueabi-gcc
OBJS = manuevers_test.o drone_comm.o
#OBJS = manuevers_test.o take_off_land.o drone_comm.o

manuevers_test: $(OBJS)
	$(ARM) -o manuevers_test $(OBJS)

manuevers_test.o: manuevers_test.cpp
	$(ARM) $(C_FLAGS) manuevers_test.cpp

take_off_land: $(OBJS)
	$(ARM) -o take_off_land $(OBJS)

take_off_land.o: take_off_land.cpp
	$(ARM) $(C_FLAGS) take_off_land.cpp

drone_comm.o: drone_comm.h drone_comm.cpp
	$(ARM) $(C_FLAGS) drone_comm.cpp

hello:
	$(ARM) $(C_FLAGS) hello_world.c -o hello

clean: 
	rm -f manuevers_test take_off_land hello *~ *.o

.IGNORE: clean

