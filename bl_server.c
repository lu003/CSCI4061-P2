#include "blather.h"


server_t server = {};
int interrupted = 0;

void handler(int sig_num){
    interrupted = 1;
}

// REPEAT:
//   check all sources
//   handle a join request if one is ready
//   for each client{
//     if the client is ready handle data from it
//   }
// }

int main (int argc, char* argv[]){
    if(argc < 2){
        return -1;
    }

    server_start(&server,argv[1],S_IRUSR | S_IWUSR);
    struct sigaction my_sa = {.sa_handler = handler};

    sigaction(SIGINT, &my_sa, NULL);
    sigaction(SIGTERM, &my_sa, NULL);
    
    while(interrupted == 0){

        server_check_sources(&server);

        if (server_join_ready(&server) == 1){
            server_handle_join(&server);
        }

        for(int i = 0; i < server.n_clients; i++){
            if(server_client_ready(&server, i) == 1){
                server_handle_client(&server, i);
            }
        }

     }
     server_shutdown(&server);
     exit(0);
}
