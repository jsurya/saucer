
#  CMPUT 379,  Assignment 3, Fall 2013
#  Due Saturday, Nov 30 2013 (submit by 11:59pm)
#  (threads & synchronization & locking)
#  
#  Submitted by: Jessica Surya


WARN := -Wall -Wextra 

make all:
	@echo 
	gcc saucer.c -lcurses -lpthread -o saucer
run:
	./saucer

%.o: %.c
	gcc -c -o $@ $<

clean:
	rm -f *.o 

# Check dependencies: gcc -MM *.c
# Paste here:

saucer.o: saucer.c saucer.h

