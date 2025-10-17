#include <iostream>
int main(){
	setlocale(LC_ALL, "rus");
	int speed;
	std::cout << "Скорость:";
	std::cin >> speed;
	if (speed < 60)
		std::cout << "Скорость в пределах нормы.";
	else if (speed >= 60 && speed <= 80)
		std::cout << "Вы приближаетесь к превышению скорости.";
	else
		std::cout << "Превышение скорости!";
	return 0;
}