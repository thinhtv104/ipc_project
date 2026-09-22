#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <sys/types.h>
#include <unistd.h>

/* =========================================================================
 * FILE DUY NHẤT CHỐNG SỬA: ĐỊNH NGHĨA STRUCT CHUNG (ĐÓNG BĂNG GIAO THỨC)
 * -------------------------------------------------------------------------
 * Quy ước chung cho cả 4 thành viên (Dev 1, Dev 2, Dev 3, Dev 4).
 * Không tự ý thay đổi cấu trúc này để tránh làm vỡ hệ thống giao tiếp.
 * ========================================================================= */

#define IPC_PROJECT_KEY_PATH "/tmp"
#define IPC_PROJECT_PROJ_ID  'I'

#define MAX_PAYLOAD_SIZE 512
#define SERVER_MSG_TYPE  1L    /* Client gửi đến Server với mtype = 1 */

/* Mã trạng thái phản hồi từ Server */
#define STATUS_OK             0
#define STATUS_ERROR_SYNTAX   1
#define STATUS_ERROR_CALC     2
#define STATUS_ERROR_UNKNOWN  3

/* Cấu trúc thông điệp chuẩn (System V / POSIX compatible) */
typedef struct {
    long mtype;                         /* Kiểu thông điệp: 1 cho Server; client_pid cho Client */
    pid_t client_pid;                   /* PID của Client gửi tin để Server định tuyến phản hồi */
    int status_code;                    /* Mã trạng thái: STATUS_OK hoặc mã lỗi tương ứng */
    char payload[MAX_PAYLOAD_SIZE];     /* Chuỗi lệnh từ client hoặc chuỗi kết quả từ server */
} msg_packet_t;

#endif /* PROTOCOL_H */
