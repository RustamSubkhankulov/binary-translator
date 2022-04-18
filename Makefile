CC = gcc 

OBJ = obj/main.o obj/logs.o obj/general.o

all: global

global: $(OBJ) 
	$(CC) $(OBJ) -o trans
	
#-fsanitize=address -fsanitize=bounds

obj/main.o: src/main.cpp src/global_conf.h
	$(CC) src/main.cpp -c -o obj/main.o

obj/logs.o: src/global_conf.h src/logs/errors_and_logs.cpp src/logs/errors_and_logs.h src/logs/errors.h src/logs/log_definitions.h src/include/errors.txt
	$(CC) src/logs/errors_and_logs.cpp -c -o obj/logs.o $(OPT_FLAG)

obj/general.o: src/global_conf.h src/general/general.cpp src/general/general.h 
	$(CC) src/general/general.cpp -c -o obj/general.o $(OPT_FLAG)

.PNONY: cleanup

cleanup:
	rm obj/*.o 
