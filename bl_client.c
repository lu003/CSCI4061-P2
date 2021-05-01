#include "blather.h"


// user thread{
//   repeat:
//     read input using simpio
//     when a line is ready
//     create a mesg_t with the line and write it to the to-server FIFO
//   until end of input
//   print "End of Input, Departing"
//   write a DEPARTED mesg_t into to-server
//   cancel the server thread

// server thread{
//   repeat:
//     read a mesg_t from to-client FIFO
//     print appropriate response to terminal with simpio
//   until a SHUTDOWN mesg_t is read
//   cancel the user thread
simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;

client_t client_actual;
client_t *client = &client_actual;

pthread_t user_thread;          // thread managing user input
pthread_t background_thread;

// Worker thread to manage user input
// user thread{
//   repeat:
//     read input using simpio
//     when a line is ready
//     create a mesg_t with the line and write it to the to-server FIFO
//   until end of input
//   print "End of Input, Departing"
//   write a DEPARTED mesg_t into to-server
//   cancel the server thread
void *user_worker(void *arg){
  while(1){
  	if(simpio->end_of_input == 1){
  		break;
  	}
    simpio_reset(simpio);
    iprintf(simpio, "");                                          // print prompt
    while(!simpio->line_ready && !simpio->end_of_input){          // read until line is complete
      simpio_get_char(simpio);
    }
    if(simpio->line_ready){
      mesg_t msg = {};
      msg.kind = 10;
      strcpy(msg.name,client->name);
      strcpy(msg.body,simpio->buf);
      write(client->to_server_fd,&msg,sizeof(mesg_t));
    }
  }
  iprintf(simpio, "End of Input, Departing\n");
  mesg_t depart = {};
  strcpy(depart.name,client->name);
  depart.kind = 30;
  pthread_cancel(background_thread); // kill the background thread
  write(client->to_server_fd,&depart,sizeof(mesg_t)); 
  return NULL;
}

// Worker thread to listen to the info from the server.
// server thread{
//   repeat:
//     read a mesg_t from to-client FIFO
//     print appropriate response to terminal with simpio
//   until a SHUTDOWN mesg_t is read
//   cancel the user thread
void *background_worker(void *arg){
  while(1){
    mesg_t mesg = {};
    read(client->to_client_fd,&mesg,sizeof(mesg_t));
    if(mesg.kind == 40){
    	iprintf(simpio, "!!! server is shutting down !!!\n");
    	return NULL;
    }else if(mesg.kind == 20){
    	iprintf(simpio, "%s\n",mesg.body);
    }else if(mesg.kind == 30){
    	iprintf(simpio, "-- %s DEPARTED --\n",mesg.name);
    }else{
    	iprintf(simpio, "[%s]: %s\n",mesg.name,mesg.body);
    }
  }
  pthread_cancel(user_thread);
  return NULL;
}



// read name of server and name of user from command line args
// create to-server and to-client FIFOs
// write a join_t request to the server FIFO
// start a user thread to read input
// start a server thread to listen to the server
// wait for threads to return
// restore standard terminal output
int main(int argc, char *argv[]){


	strcpy(client->name,argv[2]);
	int pid = getppid();
	sprintf(client->to_server_fname, "toServer%d", pid);
	sprintf(client->to_client_fname, "toClient%d", pid);

	mkfifo(client->to_server_fname, S_IRUSR | S_IWUSR);
	mkfifo(client->to_client_fname, S_IRUSR | S_IWUSR);
	client->to_server_fd = open(client->to_server_fname,O_RDWR);
	client->to_client_fd = open(client->to_client_fname,O_RDWR);

	join_t joinReq = {};
	strcpy(joinReq.name,client->name);
	strcpy(joinReq.to_server_fname,client->to_server_fname); 
	strcpy(joinReq.to_client_fname,client->to_client_fname);

	int joinFd = open(argv[1], O_WRONLY);
	write(joinFd, &joinReq, sizeof(join_t));


	char prompt[MAXNAME];
	snprintf(prompt, MAXNAME, "%s>> ", client->name); // create a prompt string
	simpio_set_prompt(simpio, prompt);         // set the prompt
	simpio_reset(simpio);                      // initialize io
	simpio_noncanonical_terminal_mode();       // set the terminal into a compatible mode

	pthread_create(&user_thread,   NULL, user_worker,   NULL);     // start user thread to read input
	pthread_create(&background_thread, NULL, background_worker, NULL);
	pthread_join(user_thread, NULL);
	pthread_join(background_thread, NULL);
	  
	simpio_reset_terminal_mode();
	printf("\n");                 // newline just to make returning to the terminal prettier
	return 0;
}