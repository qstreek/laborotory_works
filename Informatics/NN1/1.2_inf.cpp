#include <iostream>
int main() {
    setlocale(LC_ALL, "rus");
    int num;
    std::cout << "Введите число:";
    std::cin >> num;
    int result = 1;
    if (num >= 0) {
        while (num > 0) {
            result *= num;
            num--;
        }
        std::cout << "Результат:" << result;
    }
    else if (num < 0) {
        std::cout << "Число должно быть положительным!";
    }
    return 0;
}