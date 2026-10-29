#!/usr/bin/env python3
"""Desktop client. All business logic and IPC remain in the C programs."""
import queue
import subprocess
import threading
import time
from pathlib import Path
import tkinter as tk
from tkinter import ttk, messagebox

ROOT = Path(__file__).resolve().parent
BG, PANEL, INK, MUTED, BLUE = '#f4f6fb', '#ffffff', '#18243b', '#7a879c', '#5064ee'
EXAMPLES = {'CALC': '12 + 30', 'UPPER': 'hello world', 'REVERSE': 'parallel request',
            'STATS': 'this is a stress test', 'HELP': ''}


def request(command):
    result = subprocess.run([str(ROOT / 'gui_client'), command], cwd=ROOT,
                            capture_output=True, text=True, timeout=7)
    if result.returncode == 124:
        raise RuntimeError('Server không phản hồi trong 5 giây.')
    if result.returncode:
        raise RuntimeError('Không kết nối được server. Hãy bật server hoặc kiểm tra hàng đợi.\n'
                           + result.stderr.strip())
    code, sep, payload = result.stdout.partition('\n')
    if not sep or not code.isdigit():
        raise RuntimeError('Phản hồi từ chương trình C không hợp lệ.')
    return int(code), payload.rstrip('\n')


class App:
    def __init__(self, window):
        self.window, self.events = window, queue.Queue()
        self.server = None
        self.server_log = None
        self.busy = False
        self.rows = {}
        self.serial = 0
        self.successes = 0
        window.title('IPC Studio — Không gian làm việc')
        window.geometry('1120x700')
        window.minsize(980, 640)
        window.configure(bg=BG)
        style = ttk.Style()
        style.theme_use('clam')
        style.configure('TFrame', background=BG)
        style.configure('Card.TFrame', background=PANEL)
        style.configure('TLabel', background=BG, foreground=INK, font=('DejaVu Sans', 10))
        style.configure('Card.TLabel', background=PANEL)
        style.configure('Muted.TLabel', background=PANEL, foreground=MUTED, font=('DejaVu Sans', 9))
        style.configure('TButton', font=('DejaVu Sans', 9, 'bold'), padding=(12, 10),
                        background='#edf0f8', foreground='#52617a', borderwidth=0)
        style.layout('TButton', [('Button.padding', {'sticky': 'nswe', 'children': [
            ('Button.label', {'sticky': 'nswe'})]})])
        style.map('TButton', background=[('active', '#e1e6f3'), ('disabled', '#f0f2f6')],
                  foreground=[('disabled', '#a8b1c1')])
        style.configure('Primary.TButton', background=BLUE, foreground='white')
        style.map('Primary.TButton', background=[('active', '#4053d8'), ('disabled', '#bac2f6')],
                  foreground=[('disabled', '#f4f5ff')])
        style.configure('TEntry', fieldbackground='#f6f8fc', foreground=INK,
                        bordercolor='#e5e9f2', lightcolor='#e5e9f2', darkcolor='#e5e9f2', padding=10)
        style.map('TEntry', bordercolor=[('focus', BLUE)])
        style.configure('Treeview', rowheight=32, font=('DejaVu Sans', 9), background=PANEL,
                        fieldbackground=PANEL, foreground='#465572', borderwidth=0)
        style.configure('Treeview.Heading', font=('DejaVu Sans', 8, 'bold'), padding=(10, 10),
                        background='#f6f8fc', foreground='#7a879c', borderwidth=0, relief='flat')
        style.map('Treeview', background=[('selected', '#edf0ff')], foreground=[('selected', '#3545aa')])
        style.configure('Vertical.TScrollbar', background='#d8deea', troughcolor=PANEL,
                        borderwidth=0, arrowsize=10)

        sidebar = tk.Frame(window, bg='#131d35', width=184)
        sidebar.pack(side='left', fill='y')
        sidebar.pack_propagate(False)
        brand = tk.Frame(sidebar, bg='#131d35')
        brand.pack(fill='x', padx=20, pady=(27, 8))
        tk.Label(brand, text='iP', bg=BLUE, fg='white', font=('DejaVu Sans', 16, 'bold'),
                 padx=7, pady=5).pack(side='left')
        tk.Label(brand, text='  IPC Studio', bg='#131d35', fg='white',
                 font=('DejaVu Sans', 11, 'bold')).pack(side='left')
        tk.Label(sidebar, text='CLIENT–SERVER LAB', bg='#131d35', fg='#7787a8',
                 font=('DejaVu Sans', 7, 'bold')).pack(anchor='w', padx=21, pady=(3, 34))
        tk.Label(sidebar, text='DỊCH VỤ', bg='#131d35', fg='#6f7f9d',
                 font=('DejaVu Sans', 8, 'bold')).pack(anchor='w', padx=23, pady=(0, 12))
        self.service = tk.StringVar(value='CALC')
        self.nav = {}
        for key, icon, label in [('CALC', '＋', 'Tính toán'), ('UPPER', 'Aa', 'Chuyển chữ hoa'),
                                 ('REVERSE', '↔', 'Đảo chuỗi'), ('STATS', '▤', 'Thống kê'),
                                 ('HELP', '?', 'Hướng dẫn')]:
            button = tk.Button(sidebar, text=f'{icon}   {label}', anchor='w', relief='flat',
                               borderwidth=0, highlightthickness=0, cursor='hand2', padx=14, pady=12,
                               font=('DejaVu Sans', 9), command=lambda name=key: self.choose_service(name))
            button.pack(fill='x', padx=12, pady=3)
            self.nav[key] = button
        bottom = tk.Frame(sidebar, bg='#1c2945', padx=13, pady=13)
        bottom.pack(side='bottom', fill='x', padx=14, pady=20)
        tk.Label(bottom, text='●  MÔI TRƯỜNG CỤC BỘ', bg='#1c2945', fg='#9fb3db',
                 font=('DejaVu Sans', 7, 'bold')).pack(anchor='w')
        tk.Label(bottom, text='Linux / System V IPC', bg='#1c2945', fg='#7085ac',
                 font=('DejaVu Sans', 8)).pack(anchor='w', pady=(6, 0))

        shell = ttk.Frame(window, padding=(24, 22, 24, 14))
        shell.pack(side='left', fill='both', expand=True)
        header = ttk.Frame(shell)
        header.pack(fill='x', pady=(0, 18))
        titles = ttk.Frame(header)
        titles.pack(side='left')
        ttk.Label(titles, text='Không gian làm việc', font=('DejaVu Sans', 20, 'bold')).pack(anchor='w')
        ttk.Label(titles, text='Gửi yêu cầu, theo dõi phản hồi và kiểm thử trong một nơi.',
                  foreground=MUTED, font=('DejaVu Sans', 9)).pack(anchor='w', pady=(5, 0))
        tk.Label(header, text='  ●  MESSAGE QUEUE  ', bg='#e9edff', fg='#5a6bcb',
                 font=('DejaVu Sans', 8, 'bold'), padx=6, pady=7).pack(side='right', pady=5)

        metrics = ttk.Frame(shell)
        metrics.pack(fill='x', pady=(0, 16))
        self.total_metric = tk.StringVar(value='00')
        self.success_metric = tk.StringVar(value='—')
        self.latency_metric = tk.StringVar(value='—')
        for index, (title, value, accent) in enumerate([
                ('TỔNG YÊU CẦU', self.total_metric, BLUE),
                ('TỶ LỆ THÀNH CÔNG', self.success_metric, '#149d81'),
                ('PHẢN HỒI GẦN NHẤT', self.latency_metric, '#d28c37')]):
            metrics.columnconfigure(index, weight=1, uniform='metrics')
            metric = tk.Frame(metrics, bg=PANEL, highlightbackground='#e7ebf3', highlightthickness=1)
            metric.grid(row=0, column=index, sticky='nsew', padx=(0 if index == 0 else 6, 0 if index == 2 else 6))
            tk.Frame(metric, bg=accent, width=3).pack(side='left', fill='y')
            inner = tk.Frame(metric, bg=PANEL, padx=14, pady=12)
            inner.pack(fill='both', expand=True)
            tk.Label(inner, text=title, bg=PANEL, fg=MUTED, font=('DejaVu Sans', 7, 'bold')).pack(anchor='w')
            tk.Label(inner, textvariable=value, bg=PANEL, fg=INK, font=('DejaVu Sans', 19, 'bold')).pack(anchor='w', pady=(5, 0))

        bar = ttk.Frame(shell, style='Card.TFrame', padding=(12, 9))
        bar.pack(fill='x', pady=(0, 16))
        self.connection = tk.StringVar(value='● Chưa kiểm tra kết nối')
        ttk.Label(bar, textvariable=self.connection, style='Muted.TLabel', font=('DejaVu Sans', 8)).pack(side='left')
        self.start = ttk.Button(bar, text='Bật server', command=self.start_server, style='Primary.TButton')
        self.start.pack(side='right', padx=(6, 0))
        self.stop = ttk.Button(bar, text='Dừng', command=self.stop_server, state='disabled')
        self.stop.pack(side='right', padx=(6, 0))
        self.check = ttk.Button(bar, text='Kiểm tra', command=self.probe)
        self.check.pack(side='right')

        workspace = ttk.Frame(shell)
        workspace.pack(fill='x', pady=(0, 16))
        workspace.columnconfigure(0, weight=1, uniform='workspace')
        workspace.columnconfigure(1, weight=1, uniform='workspace')
        card = ttk.Frame(workspace, style='Card.TFrame', padding=18)
        card.grid(row=0, column=0, sticky='nsew', padx=(0, 8))
        self.service_title = tk.StringVar(value='Tính toán')
        ttk.Label(card, text='01  /  YÊU CẦU', style='Muted.TLabel', font=('DejaVu Sans', 8, 'bold')).pack(anchor='w')
        ttk.Label(card, textvariable=self.service_title, style='Card.TLabel',
                  font=('DejaVu Sans', 14, 'bold')).pack(anchor='w', pady=(9, 5))
        self.hint = tk.StringVar()
        ttk.Label(card, textvariable=self.hint, style='Muted.TLabel', wraplength=320).pack(anchor='w', pady=(0, 12))
        self.entry = ttk.Entry(card, font=('DejaVu Sans', 11))
        self.entry.pack(fill='x')
        self.entry.insert(0, EXAMPLES['CALC'])
        actions = ttk.Frame(card, style='Card.TFrame')
        actions.pack(fill='x', pady=(12, 0))
        self.send = ttk.Button(actions, text='Gửi yêu cầu  →', style='Primary.TButton', command=self.submit)
        self.send.pack(side='left')
        ttk.Label(actions, text='Ctrl + Enter', style='Muted.TLabel', font=('DejaVu Sans', 8)).pack(side='right', padx=4)
        result = tk.Frame(workspace, bg='#17233c', padx=18, pady=17)
        result.grid(row=0, column=1, sticky='nsew', padx=(8, 0))
        tk.Label(result, text='02  /  PHẢN HỒI', bg='#17233c', fg='#8395b7',
                 font=('DejaVu Sans', 8, 'bold')).pack(anchor='w')
        self.result_title = tk.StringVar(value='Sẵn sàng nhận kết quả')
        tk.Label(result, textvariable=self.result_title, bg='#17233c', fg='#a5b4fc',
                 font=('DejaVu Sans', 10, 'bold')).pack(anchor='w', pady=(11, 8))
        output_frame = tk.Frame(result, bg='#17233c')
        output_frame.pack(fill='both', expand=True)
        self.output = tk.Text(output_frame, height=5, width=24, wrap='word', relief='flat',
                              bg='#17233c', fg='#e6edff', insertbackground='white',
                              font=('DejaVu Sans Mono', 11), state='disabled', borderwidth=0,
                              highlightthickness=0)
        style.configure('Dark.Vertical.TScrollbar', background='#34435f', troughcolor='#17233c', borderwidth=0, arrowsize=9)
        output_scroll = ttk.Scrollbar(output_frame, orient='vertical', command=self.output.yview, style='Dark.Vertical.TScrollbar')
        self.output.configure(yscrollcommand=output_scroll.set)
        self.output.pack(side='left', fill='both', expand=True)
        output_scroll.pack(side='right', fill='y')
        self.show('Kết quả sẽ xuất hiện tại đây.\nChọn dịch vụ và gửi yêu cầu đầu tiên.')

        lower = ttk.Frame(shell, style='Card.TFrame', padding=(14, 12))
        lower.pack(fill='both', expand=True)
        tools = ttk.Frame(lower, style='Card.TFrame')
        tools.pack(fill='x', pady=(0, 10))
        ttk.Label(tools, text='Lịch sử hoạt động', style='Card.TLabel', font=('DejaVu Sans', 11, 'bold')).pack(side='left')
        self.stress = ttk.Button(tools, text='↗  Kiểm thử 8 client', command=self.stress_test)
        self.stress.pack(side='right')
        tree_frame = ttk.Frame(lower, style='Card.TFrame')
        tree_frame.pack(fill='both', expand=True)
        self.tree = ttk.Treeview(tree_frame, columns=('time', 'request', 'status', 'ms'), show='headings', height=3)
        for col, label, width in [('time', 'THỜI GIAN', 85), ('request', 'YÊU CẦU', 290),
                                  ('status', 'TRẠNG THÁI', 120), ('ms', 'ĐỘ TRỄ', 85)]:
            self.tree.heading(col, text=label, anchor='w')
            self.tree.column(col, width=width, minwidth=70, stretch=(col == 'request'), anchor='w')
        self.tree.tag_configure('ok', foreground='#16866d')
        self.tree.tag_configure('error', foreground='#cb5364')
        scrollbar = ttk.Scrollbar(tree_frame, orient='vertical', command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        self.tree.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        self.tree.bind('<<TreeviewSelect>>', self.select_history)
        ttk.Label(shell, text='511 byte / yêu cầu  ·  UPPER và REVERSE hiện hỗ trợ ASCII  ·  Chọn một dòng lịch sử để xem lại',
                  foreground=MUTED, font=('DejaVu Sans', 8)).pack(anchor='w', pady=(10, 0))
        self.refresh_service()
        window.bind('<Control-Return>', lambda event: self.submit())
        window.protocol('WM_DELETE_WINDOW', self.close)
        window.after(100, self.poll)

    def choose_service(self, name):
        self.service.set(name)
        self.example()
        self.refresh_service()

    def refresh_service(self):
        labels = {'CALC': ('Tính toán', 'Nhập phép tính, ví dụ: 12 + 30'),
                  'UPPER': ('Chuyển chữ hoa', 'Nhập văn bản cần chuyển sang chữ hoa.'),
                  'REVERSE': ('Đảo chuỗi', 'Nhập chuỗi cần đảo ngược thứ tự.'),
                  'STATS': ('Thống kê văn bản', 'Đếm số byte và số từ trong văn bản.'),
                  'HELP': ('Hướng dẫn sử dụng', 'Xem danh sách lệnh được server hỗ trợ.')}
        name = self.service.get()
        title, hint = labels[name]
        self.service_title.set(title)
        self.hint.set(hint)
        for key, button in self.nav.items():
            button.configure(bg='#293760' if key == name else '#131d35',
                             fg='#cbd3ff' if key == name else '#92a1be',
                             activebackground='#293760', activeforeground='white')

    def show(self, text):
        self.output.configure(state='normal')
        self.output.delete('1.0', 'end')
        self.output.insert('1.0', text)
        self.output.configure(state='disabled')

    def example(self, event=None):
        self.entry.delete(0, 'end')
        self.entry.insert(0, EXAMPLES[self.service.get()])

    def background(self, kind, label, fn):
        if self.busy:
            return
        self.busy = True
        for widget in (self.send, self.check, self.stress):
            widget.configure(state='disabled')
        self.result_title.set('Đang xử lý…')
        def work():
            begin = time.monotonic()
            try:
                code, text = fn()
            except Exception as exc:
                code, text = -1, str(exc)
            self.events.put((kind, label, code, text, (time.monotonic() - begin) * 1000))
        threading.Thread(target=work, daemon=True).start()

    def submit(self):
        if self.busy:
            return
        args = self.entry.get().strip()
        command = self.service.get() + (' ' + args if args else '')
        if len(command.encode('utf-8')) > 511:
            messagebox.showwarning('Yêu cầu quá dài', 'Tổng lệnh và nội dung tối đa 511 byte.')
            return
        if self.service.get() == 'CALC' and not args:
            messagebox.showwarning('Thiếu phép tính', 'Ví dụ: 12 + 30')
            return
        self.background('request', command, lambda: request(command))

    def probe(self):
        self.background('probe', 'HELP / kiểm tra kết nối', lambda: request('HELP'))

    def start_server(self):
        if self.server is not None or self.busy:
            return
        try:
            self.server_log = open(ROOT / 'gui-server.log', 'a', encoding='utf-8')
            self.server = subprocess.Popen([str(ROOT / 'server')], cwd=ROOT,
                                           stdout=self.server_log, stderr=subprocess.STDOUT)
            self.start.configure(state='disabled')
            self.stop.configure(state='normal')
            self.connection.set('● Đang khởi động server…')
            self.window.after(500, self.verify_start)
        except OSError as exc:
            self.release_log()
            self.server = None
            messagebox.showerror('Không thể bật server', str(exc))

    def verify_start(self):
        if self.server is None:
            return
        if self.server.poll() is not None:
            self.server = None
            self.release_log()
            self.start.configure(state='normal')
            self.stop.configure(state='disabled')
            self.show('Không tạo được server. Có thể đã có server khác hoặc hàng đợi cũ. Xem gui-server.log.')
        self.probe()

    def release_log(self):
        if self.server_log:
            self.server_log.close()
            self.server_log = None

    def stop_server(self):
        if self.server:
            self.server.terminate()
            try:
                self.server.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self.show('Server chưa dừng. Hãy kiểm tra tiến trình trong terminal.')
                return
            self.server = None
        self.release_log()
        self.start.configure(state='normal')
        self.stop.configure(state='disabled')
        self.connection.set('● Server do giao diện quản lý đã dừng')

    def stress_test(self):
        def run():
            result = subprocess.run([str(ROOT / 'test_client')], cwd=ROOT, capture_output=True,
                                    text=True, timeout=15, start_new_session=True)
            return (0 if result.returncode == 0 else -1), result.stdout + result.stderr
        self.background('stress', 'Kiểm thử đồng thời 8 client', run)

    def poll(self):
        while not self.events.empty():
            kind, label, code, text, ms = self.events.get_nowait()
            self.busy = False
            for widget in (self.send, self.check, self.stress):
                widget.configure(state='normal')
            self.result_title.set(('Thành công' if code == 0 else 'Có lỗi') + f'  •  {ms:.0f} ms')
            self.show(text)
            if kind != 'stress':
                self.connection.set('● Đã nhận phản hồi từ server' if code >= 0 else '● Không nhận được phản hồi')
            self.serial += 1
            if code == 0:
                self.successes += 1
            self.total_metric.set(f'{self.serial:02d}')
            self.success_metric.set(f'{self.successes / self.serial:.0%}')
            self.latency_metric.set(f'{ms:.0f} ms')
            key = str(self.serial)
            self.rows[key] = text
            self.tree.insert('', 0, iid=key, values=(time.strftime('%H:%M:%S'), label,
                             'Thành công' if code == 0 else f'Lỗi ({code})', f'{ms:.0f} ms'),
                             tags=('ok' if code == 0 else 'error',))
            if len(self.rows) > 200:
                oldest = next(iter(self.rows))
                del self.rows[oldest]
                self.tree.delete(oldest)
        if self.server and self.server.poll() is not None:
            self.server = None
            self.release_log()
            self.start.configure(state='normal')
            self.stop.configure(state='disabled')
            self.connection.set('● Server đã dừng')
        self.window.after(100, self.poll)

    def select_history(self, event=None):
        selected = self.tree.selection()
        if selected:
            self.show(self.rows[selected[0]])
            self.result_title.set('Chi tiết yêu cầu đã chọn')

    def close(self):
        # Only terminate a server launched by this UI; external servers remain running.
        self.stop_server()
        self.window.destroy()


if __name__ == '__main__':
    window = tk.Tk()
    App(window)
    window.mainloop()
