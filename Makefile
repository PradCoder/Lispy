BIN=Lispy

.PHONY : clean

$(BIN) : Lispy.c mpc.c
	$(CC) -std=c99 -Wall -ggdb Lispy.c mpc.c -ledit -lm -o $(BIN)

clean :
	rm -f $(BIN) *~

run : $(BIN)
	valgrind --leak-check=yes ./$(BIN)
