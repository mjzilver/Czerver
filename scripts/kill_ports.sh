#!/bin/bash

for port in {8080..8085}; do
    PID=$(lsof -t -i :"$port")
    
    if [ -n "$PID" ]; then
        echo "Killing process $PID on port $port"
        kill -9 $PID
    else
        echo "No process found on port $port"
    fi
done
