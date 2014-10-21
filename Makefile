OBJ = distributor
override CFLAGS += -g -Wall
override LFLAGS += -lpthread

$(OBJ) : distributor.o
		$(CC) $^ -o $@ $(LFLAGS)

%.o : %.c
		$(CC) $(CFLAGS) -c $^

clean :
		rm -f *.o $(OBJ)
