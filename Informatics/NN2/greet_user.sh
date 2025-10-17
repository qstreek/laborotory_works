#!/bin/bash
if [ $# -lt 1 ]; then
	echo "Ошибка: необходимо ввести имя"
	exit 1
fi
echo "Привет, $1" 