#include <iostream>
int main() {
    setlocale(LC_ALL, "rus");
    int num;
    std::cout << "������� �����:";
    std::cin >> num;
    int result = 1;
    if (num >= 0) {
        while (num > 0) {
            result *= num;
            num--;
        }
        std::cout << "���������:" << result;
    }
    else if (num < 0) {
        std::cout << "����� ������ ���� �������������!";
    }
    return 0;
}