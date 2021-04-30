#include "blather.h"

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

    server_t server = {};
    server_start(&server,argv[1],S_IRUSR | S_IWUSR);
     while(1){

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
}
