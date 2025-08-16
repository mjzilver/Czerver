#!/bin/bash

./bin/main &
PID=$!

cleanup() {
    echo "Killing process $PID"
    kill $PID 2>/dev/null
    wait $PID 2>/dev/null
    exit
}

trap cleanup SIGINT SIGTERM

while true; do
    leaks $PID
    sleep 5
done
