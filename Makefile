CC    = gcc 

OBJ   = obj/main.o obj/logs.o obj/general.o obj/bintrans.o

FLAGS = 

#================================================

GLOBAL_DEP 		= src/global_conf.h 

#================================================

GENERAL_DEP  	= src/general/general.cpp 			\
				  src/general/general.h 			\
				  src/general/general_conf.h 

MAIN_DEP 	 	= src/main.cpp

LOG_DEP 	 	= src/logs/errors_and_logs.cpp		\
				  src/logs/errors_and_logs.h 		\
				  src/logs/errors.h 				\
				  src/logs/log_definitions.h 		\
				  src/include/errors.txt

BINTRANS_DEP 	= src/bintrans/bintrans.cpp 		\
				  src/bintrans/bintrans.h 			\
				  src/bintrans/bintrans_conf.h 

#================================================

all: global

global: $(OBJ) 
	$(CC) $(OBJ) -o bintrans $(FLAGS)
	
#-fsanitize=address -fsanitize=bounds

#================================================

obj/main.o: 	$(GLOBAL_DEP) $(MAIN_DEP)
	$(CC) src/main.cpp 					-c -o obj/main.o 	 $(FLAGS)

obj/logs.o: 	$(GLOBAL_DEP) $(LOG_DEP)
	$(CC) src/logs/errors_and_logs.cpp  -c -o obj/logs.o     $(FLAGS)

obj/general.o: 	$(GLOBAL_DEP) $(GLOBAL_DEP)
	$(CC) src/general/general.cpp 		-c -o obj/general.o  $(FLAGS)

obj/bintrans.o:	$(GLOBAL_DEP) $(BINTRANS_DEP) 
	$(CC) src/bintrans/bintrans.cpp 	-c -o obj/bintrans.o $(FLAGS)

#================================================

.PNONY: cleanup

cleanup:
	rm obj/*.o 

#================================================
