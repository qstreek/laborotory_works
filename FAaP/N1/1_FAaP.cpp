#include <iostream>
int main() {
	setlocale(LC_ALL, "RU");
	int aray[10];
	
	for (int i = 0; i < 10; i++) {
		std::cout << "Введите " << i + 1 << " число: ";
		std::cin >> aray[i];
	}
	std::cout << "Массив: ";
	for (int i = 0; i < 10; i++) {
		if (i != 9) {
			std::cout << aray[i] << "; ";
		}
		else {
			std::cout << aray[i] << std::endl;
		}
	}	
	int sum = 0;
	for (int i = 0; i < 10; i++) {
		sum += aray[i];
	}
	
	std::cout << "Сумма элементов массива: ";
	std::cout << sum << std::endl;
	int max = -1000000;
	for (int i = 0; i < 10; i++) {
		if (aray[i] > max) {
			max = aray[i];
		}
	}
	std::cout << "Максимальное число в массиве: " << max << std::endl;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9 - i; j++) {
			if (aray[j] < aray[j + 1]) {
				std::swap(aray[j], aray[j + 1]);
			}
		}
	}
	std::cout << "Отсортировочный массив: ";
	for (int i = 0; i < 10; i++) {
		if (i != 9) {
			std::cout << aray[i] << "; ";
		}
		else {
			std::cout << aray[i];
		}
	}
}