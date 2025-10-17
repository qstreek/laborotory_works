#!/bin/bash

SERVER="localhost"
PORT=12345
echo "Клиент запущен. Подключение к $SERVER:$PORT"
while true; do
    read -p "Сообщение: " message
    echo "$message" | nc -w 3 "$SERVER" "$PORT"
    echo "Сообщение отправлено"
done