/* Machine-readable, single-request adapter for the desktop UI. */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "ipc_channel.h"
static void expired(int sig) { (void)sig; _exit(124); }
int main(int argc, char **argv) {
    if (argc != 2 || strlen(argv[1]) >= MAX_PAYLOAD_SIZE) {
        fprintf(stderr, "Yeu cau phai ngan hon 512 byte.\n"); return 1;
    }
    signal(SIGALRM, expired);
    int qid = ipc_init(0);
    if (qid < 0) return 1;
    msg_packet_t req = {0}, res = {0};
    req.mtype = SERVER_MSG_TYPE;
    req.client_pid = getpid();
    memcpy(req.payload, argv[1], strlen(argv[1]) + 1);
    if (ipc_send(qid, &req) < 0) return 1;
    alarm(5);
    if (ipc_recv(qid, &res, (long)getpid()) < 0) return 1;
    alarm(0);
    printf("%d\n%s\n", res.status_code, res.payload);
    return 0;
}
