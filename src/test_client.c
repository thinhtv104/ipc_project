#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "protocol.h"
#include "ipc_channel.h"

/* =========================================================================
 * Dev 4 code: src/test_client.c
 * -------------------------------------------------------------------------
 * Nhiệm vụ:
 * 1. Viết test_client.c: Dùng fork() tạo 5-10 client giả lập.
 * 2. Gửi request dồn dập vào server để test tải (stress test).
 * ========================================================================= */

int main(void) {
    printf("=============================================\n");
    printf("        STRESS TEST CLIENT (Dev 4 Tool)      \n");
    printf("=============================================\n");

    enum { CLIENT_COUNT = 8 };
    static const char *commands[] = {
        "UPPER hello from client",
        "REVERSE parallel request",
        "STATS this is a stress test",
        "CALC 12 + 30",
    };

    int qid = ipc_init(0);
    if (qid < 0) {
        fprintf(stderr, "Khong the ket noi den kenh IPC.\n");
        return EXIT_FAILURE;
    }

    pid_t children[CLIENT_COUNT];
    for (int i = 0; i < CLIENT_COUNT; ++i) {
        children[i] = fork();
        if (children[i] < 0) {
            perror("fork");
            for (int j = 0; j < i; ++j) {
                waitpid(children[j], NULL, 0);
            }
            return EXIT_FAILURE;
        }

        if (children[i] == 0) {
            msg_packet_t request;
            msg_packet_t response;
            const char *command = commands[i % (sizeof(commands) / sizeof(commands[0]))];

            memset(&request, 0, sizeof(request));
            request.mtype = SERVER_MSG_TYPE;
            request.client_pid = getpid();
            request.status_code = STATUS_OK;
            strncpy(request.payload, command, sizeof(request.payload) - 1);

            if (ipc_send(qid, &request) < 0) {
                fprintf(stderr, "Client %d: gui that bai.\n", (int)getpid());
                _exit(EXIT_FAILURE);
            }
            if (ipc_recv(qid, &response, (long)getpid()) < 0) {
                fprintf(stderr, "Client %d: nhan that bai.\n", (int)getpid());
                _exit(EXIT_FAILURE);
            }

            printf("Client %d | %s | status=%d | %s\n",
                   (int)getpid(), command, response.status_code, response.payload);
            fflush(stdout);
            _exit(EXIT_SUCCESS);
        }
    }

    int failed = 0;
    for (int i = 0; i < CLIENT_COUNT; ++i) {
        int status;
        if (waitpid(children[i], &status, 0) < 0 ||
            !WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) {
            failed = 1;
        }
    }

    printf("Da hoan tat stress test voi %d client con.\n", CLIENT_COUNT);

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
