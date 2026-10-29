#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "protocol.h"
#include "ipc_channel.h"

/* =========================================================================
 * Dev 4 code: src/client.c
 * -------------------------------------------------------------------------
 * Nhiệm vụ:
 * 1. Viết client.c: Giao diện CLI nhập lệnh.
 * 2. Gửi tin qua module Dev 1 (ipc_send).
 * 3. Chờ và in kết quả phản hồi từ Server (ipc_recv).
 * ========================================================================= */

static void timeout_handler(int sig) {
    (void)sig;
    static const char message[] = "Server khong phan hoi trong 5 giay.\n";
    ssize_t written = write(STDERR_FILENO, message, sizeof(message) - 1);
    (void)written;
    _exit(EXIT_FAILURE);
}

int main(void) {
    signal(SIGALRM, timeout_handler);
    printf("=============================================\n");
    printf("           IPC CLIENT (Dev 4 CLI)            \n");
    printf("=============================================\n");

    int qid = ipc_init(0);
    if (qid < 0) {
        fprintf(stderr, "Khong the ket noi den kenh IPC.\n");
        return EXIT_FAILURE;
    }

    printf("Nhap lenh (go 'quit' de thoat):\n");

    char input[MAX_PAYLOAD_SIZE];
    for (;;) {
        msg_packet_t request;
        msg_packet_t response;

        printf("> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            putchar('\n');
            break;
        }

        if (!strchr(input, '\n') && !feof(stdin)) {
            int ch = getchar();
            if (ch != '\n' && ch != EOF) {
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                fprintf(stderr, "Lenh qua dai (toi da 511 byte).\n");
                continue;
            }
        }
        input[strcspn(input, "\r\n")] = '\0';
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            break;
        }
        if (input[0] == '\0') {
            fprintf(stderr, "Lenh khong duoc rong.\n");
            continue;
        }

        memset(&request, 0, sizeof(request));
        request.mtype = SERVER_MSG_TYPE;
        request.client_pid = getpid();
        request.status_code = STATUS_OK;
        memcpy(request.payload, input, strlen(input) + 1);

        if (ipc_send(qid, &request) < 0) {
            fprintf(stderr, "Gui yeu cau that bai.\n");
            continue;
        }

        alarm(5);
        if (ipc_recv(qid, &response, (long)getpid()) < 0) {
            alarm(0);
            fprintf(stderr, "Nhan phan hoi that bai.\n");
            continue;
        }

        alarm(0);
        printf("[%d] %s\n", response.status_code, response.payload);
    }

    return 0;
}
