#include "blather.h"

// REPEAT:
//   check all sources
//   handle a join request if one is ready
//   for each client{
//     if the client is ready handle data from it
//   }
// }

int main (int argc, char* argv[]){
    if(argc == 0){
        return;
    }

    server_t server = {};
    server_start(&server,argv[1],O_RDONLY | O_WRONLY);
     while(1){

        server_check_sources(&server);

        if (server_join_ready(&server)){
            server_handle_join(&server);
        }

        for(int i = 0; i < server.n_clients; i++){
            if(server_client_ready(&server, i)){
                server_handle_client(&server, i);
            }
        }

     }
}
