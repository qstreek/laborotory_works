#!/bin/bash
PORT=12345
echo "Сервер запущен на порту $PORT"

while true; do
    echo "Ожидание сообщения..."
    message=$(nc -l -p $PORT)
    echo "Получено: $message"
    
    # Если нужно ответить клиенту
    echo "Ответ сервера" | nc localhost $PORT
done