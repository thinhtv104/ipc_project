# Hệ thống IPC Client - Server (ipc_project)

Dự án bài tập lớn xây dựng hệ thống Client - Server giao tiếp liên tiến trình (IPC - Inter-Process Communication) bằng ngôn ngữ C trên môi trường Linux/Unix, hỗ trợ đa tiến trình và kiểm thử tải đồng thời.

---

## 📁 Cấu trúc thư mục dự án

```
ipc_project/
├── include/
│   ├── protocol.h           # FILE DUY NHẤT CHỐNG SỬA: Định nghĩa struct chung (đóng băng)
│   ├── ipc_channel.h        # Module 1 (Dev 1 làm chủ)
│   └── service_handler.h    # Module 2 (Dev 2 làm chủ)
├── src/
│   ├── ipc_channel.c        # Dev 1 code
│   ├── service_handler.c    # Dev 2 code
│   ├── server.c             # Dev 3 code
│   ├── client.c             # Dev 4 code
│   └── test_client.c        # Dev 4 code (tool test đa tiến trình bằng C)
├── Makefile                 # Tạo sẵn từ đầu, không sửa thêm
├── .gitignore
└── README.md
```

---

## 👥 Bảng phân công nhiệm vụ chi tiết

| Thành viên | File chịu trách nhiệm | Header phụ thuộc | Nhiệm vụ chính |
| :--- | :--- | :--- | :--- |
| **Dev 1** | `include/ipc_channel.h`<br>`src/ipc_channel.c` | `protocol.h` | • Viết các hàm wrapper IPC (POSIX Message Queue hoặc System V).<br>• `ipc_init()`, `ipc_send()`, `ipc_recv()`, `ipc_destroy()`.<br>• Bắt tín hiệu `SIGINT` để dọn sạch IPC trong RAM. |
| **Dev 2** | `include/service_handler.h`<br>`src/service_handler.c` | `protocol.h` | • Viết bộ parser chuỗi lệnh (Tokenize chuỗi từ client).<br>• Cài đặt logic: `CALC` (tính toán), `UPPER`, `REVERSE`, `STATS`.<br>• Hàm chính: `int handle_service(const char *in, char *out);` |
| **Dev 3** | `src/server.c` | `protocol.h`<br>`ipc_channel.h`<br>`service_handler.h` | • Viết hàm `main()` của Server.<br>• Gọi hàm của Dev 1 để tạo kênh lắng nghe.<br>• Vòng lặp nhận tin, xử lý đa tiến trình (`fork()` hoặc tuần tự), gọi hàm Dev 2 để xử lý, rồi gửi phản hồi. |
| **Dev 4** | `src/client.c`<br>`src/test_client.c` | `protocol.h`<br>`ipc_channel.h` | • Viết `client.c`: Giao diện CLI nhập lệnh, gửi tin qua Dev 1, chờ in kết quả.<br>• Viết `test_client.c`: Dùng `fork()` tạo 5–10 client giả lập gửi request dồn dập vào server để test tải. |

---

## 🌿 Quy trình phối hợp làm việc trên Git (Tránh xung đột)

1. **Clone repository về máy**:
   ```bash
   git clone https://github.com/thinhtv104/ipc_project.git
   cd ipc_project
   ```

2. **Mỗi bạn tạo và làm việc trên nhánh riêng**:
   * Dev 1: `git checkout -b dev1-ipc`
   * Dev 2: `git checkout -b dev2-service`
   * Dev 3: `git checkout -b dev3-server`
   * Dev 4: `git checkout -b dev4-client`

3. **Sau khi hoàn thành phần việc của mình, đẩy nhánh lên GitHub**:
   ```bash
   git add .
   git commit -m "feat: hoan thanh nhiem vu module cua dev X"
   git push -u origin <ten-nhanh-cua-ban>
   ```

4. **Tạo Pull Request (PR)** trên GitHub để nhóm cùng review và merge vào nhánh `main`.

---

## 🛠️ Hướng dẫn biên dịch và chạy

* **Biên dịch toàn bộ hệ thống**:
  ```bash
  make
  ```

* **Chạy Server (Terminal 1)**:
  ```bash
  ./server
  ```

* **Chạy Client tương tác (Terminal 2, 3...)**:
  ```bash
  ./client
  ```

* **Chạy kiểm thử tải đa tiến trình**:
  ```bash
  ./test_client
  ```

* **Dọn dẹp các file build**:
  ```bash
  make clean
  ```
