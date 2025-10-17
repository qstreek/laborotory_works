#!/bin/bash
# 1. Создание файла
touch my_file.txt && echo "Файл создан"

# 2. Запись текста в файл
echo "Hello, Bash!" > my_file.txt && echo "Текст записан"

# 3. Чтение и вывод содержимого
echo "Содержимое файла:"
cat my_file.txt

# 4. Удаление файла
rm my_file.txt && echo "Файл удален"
