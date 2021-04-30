CFLAGS = -Wall -g
CC = gcc $(CFLAGS)

bl_server : bl_server.o server_funcs.o util.o blather.h
	$(CC) -o bl_server bl_server.o server_funcs.o util.o
	@echo server is ready

bl_server.o : bl_server.c blather.h
	$(CC) -c bl_server.c

server_funcs.o : server_funcs.c blather.h
	$(CC) -c server_funcs.c

util.o : util.c blather.h
	$(CC) -c util.c

clean :
		@echo Cleaning up object files
		rm -f *.o commando
#nclude test_Makefile		# add at the END of Makefile
