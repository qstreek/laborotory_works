#include <iostream>
int main(){
	setlocale(LC_ALL, "rus");
	int speed;
	std::cout << "��������:";
	std::cin >> speed;
	if (speed < 60)
		std::cout << "�������� � �������� �����.";
	else if (speed >= 60 && speed <= 80)
		std::cout << "�� ������������� � ���������� ��������.";
	else
		std::cout << "���������� ��������!";
	return 0;
}