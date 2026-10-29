import sys
import time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import tkinter as tk
from gui import App

root = tk.Tk()
app = App(root)
def settle():
    deadline = time.monotonic() + 20
    while time.monotonic() < deadline:
        root.update()
        if not app.busy:
            return
        time.sleep(0.02)
    raise AssertionError("UI request did not finish")
try:
    for service, args, expected in [
        ('CALC', '12 + 30', '42'),
        ('UPPER', 'hello world', 'HELLO WORLD'),
        ('REVERSE', 'abc', 'cba'),
        ('STATS', 'one two', 'length=7 words=2'),
    ]:
        app.choose_service(service)
        app.entry.delete(0, 'end')
        app.entry.insert(0, args)
        app.submit()
        settle()
        assert app.output.get('1.0', 'end').strip() == expected, service
    app.entry.delete(0, 'end')
    app.service.set('CALC')
    app.entry.insert(0, '1 / 0')
    app.submit()
    settle()
    assert 'Có lỗi' in app.result_title.get()
    app.stress_test()
    settle()
    assert 'Thành công' in app.result_title.get()
    assert len(app.rows) == 6
    assert app.total_metric.get() == "06"
    assert app.success_metric.get() == "83%"
    assert app.latency_metric.get().endswith(" ms")
    print('GUI_SMOKE_PASS: 4 services, error display, 8 clients, history, responsive event loop')
finally:
    root.destroy()
