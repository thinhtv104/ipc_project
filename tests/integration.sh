#!/usr/bin/env bash
set -euo pipefail
./server > server-test.log 2>&1 &
server_pid=$!
trap 'kill -TERM "$server_pid" 2>/dev/null || true; wait "$server_pid" 2>/dev/null || true; rm -f server-test.log' EXIT
sleep 0.3
kill -0 "$server_pid"
if ./server > /dev/null 2>&1; then
    echo "FAIL: second server accepted"; exit 1
fi
printf 'CALC 12 + 30\nquit\n' | timeout 10 ./client | grep -F '[0] 42'
for round in 1 2 3 4 5; do
    timeout 15 ./test_client
done
kill -TERM "$server_pid"
wait "$server_pid"
if printf 'quit\n' | ./client >/dev/null 2>&1; then
    echo "FAIL: queue survived shutdown"; exit 1
fi
echo 'Integration tests passed'
