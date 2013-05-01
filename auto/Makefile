C_FLAGS = -c
ARM = arm-none-linux-gnueabi-gcc
OBJS = maneuvers_test.o drone_comm.o
#OBJS = maneuvers_test.o take_off_land.o drone_comm.o

maneuvers_test: $(OBJS)
	$(ARM) -o maneuvers $(OBJS)

maneuvers_test.o: maneuvers_test.cpp
	$(ARM) $(C_FLAGS) maneuvers_test.cpp

take_off_land: $(OBJS)
	$(ARM) -o take_off_land $(OBJS)

take_off_land.o: take_off_land.cpp
	$(ARM) $(C_FLAGS) take_off_land.cpp

drone_comm.o: drone_comm.h drone_comm.cpp
	$(ARM) $(C_FLAGS) drone_comm.cpp

hello:
	$(ARM) $(C_FLAGS) hello_world.c -o hello

clean: 
	rm -f maneuvers take_off_land hello *~ *.o

.IGNORE: clean

