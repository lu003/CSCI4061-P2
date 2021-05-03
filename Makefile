CFLAGS = -Wall -g
CC = gcc -pthread $(CFLAGS)

bl_server : bl_server.o server_funcs.o util.o bl_client.o simpio.o blather.h
	$(CC) -o bl_server bl_server.o server_funcs.o util.o
	$(CC) -o bl_client bl_client.o simpio.o
	@echo server is ready

bl_server.o : bl_server.c blather.h
	$(CC) -c bl_server.c

server_funcs.o : server_funcs.c blather.h
	$(CC) -c server_funcs.c

util.o : util.c blather.h
	$(CC) -c util.c

bl_client.o : bl_client.c blather.h
	$(CC) -c bl_client.c

simpio.o : simpio.c blather.h
	$(CC) -c simpio.c
clean :
		@echo Cleaning up object files
		rm -f *.o bl_server bl_client


include test_Makefile		# add at the END of Makefile
