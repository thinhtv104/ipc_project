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
├── Makefile                 # Build và kiểm thử
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


## Bản hoàn thiện: chạy và kiểm thử

Yêu cầu: Linux (Ubuntu, Debian hoặc WSL2), GCC, Make và Bash. System V
Message Queue không chạy trực tiếp bằng trình biên dịch Windows.

```bash
make
# Terminal 1
./server
# Terminal 2
./client
```

Trong client, nhập `HELP` để xem lệnh, `quit` hoặc `exit` để thoát:

```text
CALC 12 + 30
CALC 10 % 3
UPPER hello world
REVERSE hello
STATS this is a stress test
```

Lệnh không phân biệt chữ hoa/thường. `CALC` hỗ trợ số thực hữu hạn;
`%` yêu cầu hai giá trị nguyên, số chia khác 0. Dữ liệu thừa, chia cho 0
và kết quả vượt giới hạn đều trả lỗi. `UPPER`, `REVERSE`, `STATS` hiện
xử lý theo byte/ASCII; chưa hỗ trợ chuyển hoa hoặc đảo chữ tiếng Việt UTF-8.
Mỗi yêu cầu tối đa 511 byte. Client thoát nếu server không trả lời trong 5 giây.

```bash
make test       # Kiểm thử dịch vụ, CLI, 40 client, server trùng và dọn hàng đợi
make clean
```

Có thể chạy `make test` trên Ubuntu để kiểm tra toàn bộ phần C và IPC.
Cấu hình GitHub Actions chưa được tải lên vì quyền đăng nhập hiện tại không
cho phép tạo workflow. Bộ kiểm thử tải kiểm tra cả mã trạng thái và nội dung
phản hồi; mỗi tiến trình kiểm thử có giới hạn 10 giây.

### Quản lý tài nguyên

- Hàng đợi chỉ cho tài khoản hiện tại truy cập (0600).
- Server tạo hàng đợi độc quyền; server thứ hai sẽ báo lỗi.
- Giới hạn 32 worker đồng thời; thu hồi tiến trình con đã kết thúc.
- Chỉ server cha xóa hàng đợi khi nhận Ctrl+C hoặc SIGTERM.
- Gửi không chặn: hàng đợi đầy sẽ trả lỗi thay vì treo tiến trình.
- Kiểm tra kích thước gói tin, chuỗi kết thúc và PID trước khi xử lý.

Sau khi server bị `kill -9`, hàng đợi có thể còn tồn tại. Dùng `ipcs -q`
để xác định đúng hàng đợi của dự án, sau đó `ipcrm -q <qid>` trước khi chạy lại.
Không xóa hàng đợi của ứng dụng khác. Khóa hiện vẫn dùng `/tmp` và project ID
`I` để giữ nguyên giao thức gốc, nên chỉ chạy một bản dự án trên mỗi máy.

### Phạm vi và giới hạn

Cấu trúc `protocol.h` được giữ nguyên để tương thích với các module cũ.
Định tuyến phản hồi vẫn dựa vào PID, mỗi client chỉ gửi một yêu cầu tại một
thời điểm. Nếu client bị dừng trước khi đọc phản hồi, phản hồi có thể còn trong
hàng đợi tới khi server tắt. Đây là ứng dụng IPC cục bộ phục vụ học tập, chưa
có cơ chế xác thực từng thông điệp hoặc ID yêu cầu độc lập.


## Giao diện IPC Studio

Trên Ubuntu, cài `sudo apt install python3-tk`, sau đó chạy `make gui`.
Giao diện có chọn dịch vụ, ví dụ nhập liệu, kết quả, lịch sử 200 yêu cầu gần
nhất và kiểm thử 8 client. Ctrl+Enter để gửi. Giao diện không thay thế logic C:
`gui.py` gọi `gui_client`, dùng đúng Message Queue và server C hiện có.

Nút Bật server tạo một server do giao diện quản lý. Dừng server hoặc đóng
cửa sổ sẽ dừng server này; server được mở từ terminal khác không bị dừng.
Nếu server đã chạy, chỉ cần nhấn Kiểm tra kết nối. Log server của giao diện
lưu tại `gui-server.log`. Các thao tác gửi và kiểm thử chạy nền để cửa sổ
vẫn phản hồi. Trạng thái kết nối phản ánh lần kiểm tra/gửi gần nhất.
