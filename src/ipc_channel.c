#include "ipc_channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

static int qid_global = -1;
static void cleanup(int sig) { (void)sig; if (qid_global >= 0) msgctl(qid_global, IPC_RMID, 0); _exit(0); }
int ipc_init(int is_server) {
    key_t key = ftok(IPC_PROJECT_KEY_PATH, IPC_PROJECT_PROJ_ID);
    if (key == (key_t)-1) return -1;
    qid_global = msgget(key, 0666 | (is_server ? IPC_CREAT : 0));
    if (qid_global >= 0 && is_server) signal(SIGINT, cleanup);
    return qid_global;
}

int ipc_send(int qid, const msg_packet_t *msg) {
    return (qid >= 0 && msg) ? msgsnd(qid, msg, sizeof(*msg) - sizeof(msg->mtype), 0) : -1;
}

int ipc_recv(int qid, msg_packet_t *msg, long target_type) {
    return (qid >= 0 && msg && target_type > 0) ? msgrcv(qid, msg, sizeof(*msg) - sizeof(msg->mtype), target_type, 0) : -1;
}

void ipc_destroy(int qid) {
    if (qid >= 0) msgctl(qid, IPC_RMID, 0);
}
