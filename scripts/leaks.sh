#!/bin/bash

./bin/main &
PID=$!

cleanup() {
    echo "Killing process $PID"
    kill $PID
    wait $PID
    exit
}

trap cleanup SIGINT SIGTERM

while true; do
    leaks $PID
    sleep 5
done
