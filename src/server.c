#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "protocol.h"
#include "ipc_channel.h"
#include "service_handler.h"
#include <sys/wait.h>

int main(void) {
    printf("=============================================\n");
    printf("           IPC SERVER (Dev 3 Main)           \n");
    printf("=============================================\n");

    int qid=ipc_init(1); if(qid<0)return EXIT_FAILURE; signal(SIGCHLD,SIG_IGN);
    for(;;){msg_packet_t req; if(ipc_recv(qid,&req,SERVER_MSG_TYPE)<0)break; if(!fork()){msg_packet_t res={0};res.mtype=req.client_pid;res.client_pid=req.client_pid;res.status_code=handle_service(req.payload,res.payload);ipc_send(qid,&res);_exit(0);}}

    return 0;
}
