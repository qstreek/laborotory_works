#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <libpq-fe.h>
#include <windows.h>

using namespace std;

template<typename T1, typename T2>
class Pair {
private:
    T1 first;
    T2 second;
public:
    Pair(T1 f, T2 s) : first(f), second(s) {}
    T1 getFirst() const { return first; }
    T2 getSecond() const { return second; }
};

class Database {
private:
    PGconn* connection;
    ofstream logFile;

    void logMessage(const string& message) {
        if (!logFile.is_open()) {
            logFile.open("log.txt", ios::app);
            if (!logFile.is_open()) {
                return;
            }
        }

        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &time);
        timeStr[24] = '\0';

        logFile << timeStr << " - " << message << endl;
        logFile.flush();
    }

public:
    Database() : connection(nullptr) {
        logFile.open("log.txt", ios::app);
        logMessage("Программа запущена");
    }

    ~Database() {
        if (connection) PQfinish(connection);
        if (logFile.is_open()) {
            logMessage("Программа завершена");
            logFile.close();
        }
    }

    bool connect(const string& connStr) {
        connection = PQconnectdb(connStr.c_str());
        if (PQstatus(connection) != CONNECTION_OK) {
            cout << "Ошибка подключения: " << PQerrorMessage(connection) << endl;
            return false;
        }
        logMessage("Подключение к БД успешно");
        return true;
    }

    bool executeSQL(const string& sql, bool suppressNotice = true) {
        if (!connection) return false;

        if (suppressNotice) {
            PQexec(connection, "SET client_min_messages TO WARNING;");
        }

        PGresult* res = PQexec(connection, sql.c_str());
        ExecStatusType status = PQresultStatus(res);

        if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
            string error = PQresultErrorMessage(res);
            bool isNotice = (error.find("NOTICE") != string::npos);
            if (!isNotice && !suppressNotice) {
                cout << "Ошибка SQL: " << error << endl;
            }
        }

        PQclear(res);

        if (suppressNotice) {
            PQexec(connection, "SET client_min_messages TO NOTICE;");
        }

        return true;
    }

    PGresult* query(const string& sql) {
        if (!connection) return nullptr;
        return PQexec(connection, sql.c_str());
    }

    void log(const string& msg) {
        logMessage(msg);
    }
};

class Category {
private:
    Database* db;
    int id;
    string name;

public:
    Category(Database* database, int i = 0, const string& n = "")
        : db(database), id(i), name(n) {
    }

    bool save() {
        if (!db) return false;

        string escapedName = name;
        size_t pos = 0;
        while ((pos = escapedName.find("'", pos)) != string::npos) {
            escapedName.replace(pos, 1, "''");
            pos += 2;
        }

        string sql = "INSERT INTO categories (category_name) VALUES ('" +
            escapedName + "') RETURNING id";

        PGresult* res = db->query(sql);
        if (res && PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
            id = stoi(PQgetvalue(res, 0, 0));
            PQclear(res);
            db->log("Добавлена категория: " + name + " (ID: " + to_string(id) + ")");
            return true;
        }

        if (res) PQclear(res);
        return false;
    }

    int getId() const { return id; }
    string getName() const { return name; }
};

class Product {
protected:
    Database* db;
    int id;
    string name;
    double price;
    int quantity;
    int categoryId;

public:
    Product(Database* database, int i = 0, const string& n = "",
        double p = 0.0, int q = 0, int catId = 0)
        : db(database), id(i), name(n), price(p), quantity(q), categoryId(catId) {
    }

    virtual bool save() {
        if (!db) return false;

        string escapedName = name;
        size_t pos = 0;
        while ((pos = escapedName.find("'", pos)) != string::npos) {
            escapedName.replace(pos, 1, "''");
            pos += 2;
        }

        string sql = "INSERT INTO products (name, price, quantity, category_id) VALUES ('" +
            escapedName + "', " +
            to_string(price) + ", " +
            to_string(quantity) + ", " +
            to_string(categoryId) + ") RETURNING id";

        PGresult* res = db->query(sql);
        if (res && PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
            id = stoi(PQgetvalue(res, 0, 0));
            PQclear(res);
            db->log("Добавлен продукт: " + name + " (ID: " + to_string(id) + ")");
            return true;
        }

        if (res) PQclear(res);
        return false;
    }

    virtual void display() const {
        cout << "ID: " << id << " | " << name
            << " | Цена: " << fixed << setprecision(2) << price
            << " | Кол-во: " << quantity
            << " | Кат.ID: " << categoryId << endl;
    }

    int getId() const { return id; }
    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }
    string getName() const { return name; }
    double getPrice() const { return price; }
};

class PhysicalProduct : public Product {
public:
    PhysicalProduct(Database* database, int i = 0, const string& n = "",
        double p = 0.0, int q = 0, int catId = 0)
        : Product(database, i, n, p, q, catId) {
    }

    void display() const override {
        cout << "[Физический] ID: " << id << " | " << name
            << " | Цена: " << fixed << setprecision(2) << price
            << " | Кол-во: " << quantity
            << " | Кат.ID: " << categoryId << endl;
    }
};

class DigitalProduct : public Product {
public:
    DigitalProduct(Database* database, int i = 0, const string& n = "",
        double p = 0.0, int q = 0, int catId = 0)
        : Product(database, i, n, p, q, catId) {
    }

    void display() const override {
        cout << "[Цифровой] ID: " << id << " | " << name
            << " | Цена: " << fixed << setprecision(2) << price
            << " | Кол-во: " << quantity
            << " | Кат.ID: " << categoryId << endl;
    }
};

class Sale {
private:
    Database* db;
    int id;
    int productId;
    int quantitySold;

public:
    Sale(Database* database, int i = 0, int pId = 0, int qty = 0)
        : db(database), id(i), productId(pId), quantitySold(qty) {
    }

    bool save() {
        if (!db) return false;

        string checkSql = "SELECT id, quantity FROM products WHERE id = " +
            to_string(productId);
        PGresult* res = db->query(checkSql);

        if (!res || PQntuples(res) == 0) {
            if (res) PQclear(res);
            cout << "Ошибка: Продукт не найден!" << endl;
            return false;
        }

        int productIdDB = stoi(PQgetvalue(res, 0, 0));
        int available = stoi(PQgetvalue(res, 0, 1));
        PQclear(res);

        if (available < quantitySold) {
            cout << "Ошибка: Недостаточно товара! Доступно: " << available << endl;
            return false;
        }

        string sql = "INSERT INTO sales (product_id, quantity_sold) VALUES (" +
            to_string(productIdDB) + ", " +
            to_string(quantitySold) + ") RETURNING id";

        res = db->query(sql);
        if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
            if (res) PQclear(res);
            return false;
        }

        id = stoi(PQgetvalue(res, 0, 0));
        PQclear(res);

        sql = "UPDATE products SET quantity = quantity - " +
            to_string(quantitySold) +
            " WHERE id = " + to_string(productIdDB);

        if (!db->executeSQL(sql, true)) {
            cout << "Ошибка обновления количества!" << endl;
            return false;
        }

        db->log("Продажа: продукт ID " + to_string(productIdDB) +
            ", количество " + to_string(quantitySold));
        return true;
    }
};

class Menu {
private:
    Database* db;

public:
    Menu(Database* database) : db(database) {}

    void run() {
        while (true) {
            cout << "\n=== СИСТЕМА УПРАВЛЕНИЯ ПРОДУКТАМИ ===" << endl;
            cout << "1. Добавить категорию" << endl;
            cout << "2. Добавить продукт" << endl;
            cout << "3. Добавить продажу" << endl;
            cout << "4. Показать все продукты" << endl;
            cout << "5. Показать все продажи" << endl;
            cout << "6. Продукты определенной категории" << endl;
            cout << "7. Общая выручка" << endl;
            cout << "8. Количество проданных единиц каждого продукта" << endl;
            cout << "9. Топ-5 самых продаваемых продуктов" << endl;
            cout << "0. Выход" << endl;
            cout << "Выбор: ";

            int choice;
            cin >> choice;

            if (choice == 0) {
                cout << "Выход из программы..." << endl;
                break;
            }

            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Неверный ввод!" << endl;
                continue;
            }

            cin.ignore();

            try {
                switch (choice) {
                case 1: addCategory(); break;
                case 2: addProduct(); break;
                case 3: addSale(); break;
                case 4: showProducts(); break;
                case 5: showSales(); break;
                case 6: showProductsByCategory(); break;
                case 7: showRevenue(); break;
                case 8: showSoldQuantities(); break;
                case 9: showTopProducts(); break;
                default: cout << "Неверный выбор!" << endl;
                }
            }
            catch (const exception& e) {
                cout << "Ошибка: " << e.what() << endl;
            }
        }
    }

private:
    void addCategory() {
        cout << "Название категории: ";
        string name;
        getline(cin, name);

        if (name.empty()) {
            cout << "Имя не может быть пустым!" << endl;
            return;
        }

        Category cat(db, 0, name);
        if (cat.save()) {
            cout << "Категория добавлена! ID: " << cat.getId() << endl;
        }
        else {
            cout << "Ошибка! Возможно категория уже существует." << endl;
        }
    }

    void addProduct() {
        cout << "Тип продукта (1-физический, 2-цифровой): ";
        int type;
        cin >> type;

        if (type != 1 && type != 2) {
            cout << "Неверный тип!" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        cin.ignore();

        cout << "Название: ";
        string name;
        getline(cin, name);

        cout << "Цена: ";
        double price;
        cin >> price;

        if (price <= 0) {
            cout << "Цена должна быть положительной!" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        cout << "Количество: ";
        int quantity;
        cin >> quantity;

        if (quantity < 0) {
            cout << "Количество не может быть отрицательным!" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        cout << "ID категории: ";
        int catId;
        cin >> catId;
        cin.ignore();

        bool success = false;

        if (type == 1) {
            PhysicalProduct prod(db, 0, name, price, quantity, catId);
            success = prod.save();
        }
        else {
            DigitalProduct prod(db, 0, name, price, quantity, catId);
            success = prod.save();
        }

        if (success) {
            cout << "Продукт добавлен!" << endl;
        }
        else {
            cout << "Ошибка! Проверьте ID категории." << endl;
        }
    }

    void addSale() {
        cout << "ID продукта: ";
        int prodId;
        cin >> prodId;

        if (prodId <= 0) {
            cout << "Неверный ID!" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        cout << "Количество: ";
        int quantity;
        cin >> quantity;

        if (quantity <= 0) {
            cout << "Количество должно быть положительным!" << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        cin.ignore();

        Sale sale(db, 0, prodId, quantity);
        if (sale.save()) {
            cout << "Продажа добавлена!" << endl;
        }
        else {
            cout << "Ошибка добавления продажи!" << endl;
        }
    }

    void showProducts() {
        string sql = "SELECT p.id, p.name, p.price, p.quantity, c.category_name "
            "FROM products p LEFT JOIN categories c ON p.category_id = c.id "
            "ORDER BY p.id";

        PGresult* res = db->query(sql);
        if (!res) {
            cout << "Ошибка запроса!" << endl;
            return;
        }

        int rows = PQntuples(res);
        cout << "\n=== СПИСОК ПРОДУКТОВ ===" << endl;

        if (rows == 0) {
            cout << "Нет продуктов" << endl;
        }
        else {
            for (int i = 0; i < rows; i++) {
                cout << "ID: " << PQgetvalue(res, i, 0)
                    << " | Название: " << PQgetvalue(res, i, 1)
                    << " | Цена: " << fixed << setprecision(2) << stod(PQgetvalue(res, i, 2))
                    << " | Кол-во: " << PQgetvalue(res, i, 3)
                    << " | Категория: " << (PQgetisnull(res, i, 4) ? "нет" : PQgetvalue(res, i, 4)) << endl;
            }
            cout << "Всего продуктов: " << rows << endl;
        }
        PQclear(res);
    }

    void showSales() {
        string sql = "SELECT s.id, p.name, s.quantity_sold, s.sale_date "
            "FROM sales s JOIN products p ON s.product_id = p.id "
            "ORDER BY s.sale_date DESC";

        PGresult* res = db->query(sql);
        if (!res) {
            cout << "Ошибка запроса!" << endl;
            return;
        }

        int rows = PQntuples(res);
        cout << "\n=== СПИСОК ПРОДАЖ ===" << endl;

        if (rows == 0) {
            cout << "Нет продаж" << endl;
        }
        else {
            for (int i = 0; i < rows; i++) {
                cout << "ID: " << PQgetvalue(res, i, 0)
                    << " | Продукт: " << PQgetvalue(res, i, 1)
                    << " | Кол-во: " << PQgetvalue(res, i, 2)
                    << " | Дата: " << PQgetvalue(res, i, 3) << endl;
            }
        }
        PQclear(res);
    }

    void showProductsByCategory() {
        cout << "Введите ID категории: ";
        int catId;
        cin >> catId;
        cin.ignore();

        string sql = "SELECT p.id, p.name, p.price, p.quantity, c.category_name "
            "FROM products p JOIN categories c ON p.category_id = c.id "
            "WHERE c.id = " + to_string(catId) + " ORDER BY p.name";

        PGresult* res = db->query(sql);
        if (!res) {
            cout << "Ошибка выполнения запроса!" << endl;
            return;
        }

        int rows = PQntuples(res);
        cout << "\n=== ПРОДУКТЫ КАТЕГОРИИ ===" << endl;

        if (rows == 0) {
            cout << "Нет продуктов в этой категории" << endl;
        }
        else {
            string categoryName = PQgetvalue(res, 0, 4);
            cout << "Категория: " << categoryName << " (ID: " << catId << ")" << endl;
            cout << "--------------------------------------" << endl;

            for (int i = 0; i < rows; i++) {
                cout << "ID: " << PQgetvalue(res, i, 0)
                    << " | Название: " << PQgetvalue(res, i, 1)
                    << " | Цена: " << fixed << setprecision(2) << stod(PQgetvalue(res, i, 2))
                    << " | Кол-во: " << PQgetvalue(res, i, 3) << endl;
            }
        }
        PQclear(res);
    }

    void showRevenue() {
        string sql = "SELECT SUM(p.price * s.quantity_sold) as revenue "
            "FROM sales s JOIN products p ON s.product_id = p.id";

        PGresult* res = db->query(sql);
        if (!res) {
            cout << "Ошибка запроса!" << endl;
            return;
        }

        double revenue = 0.0;
        if (PQntuples(res) > 0 && !PQgetisnull(res, 0, 0)) {
            revenue = stod(PQgetvalue(res, 0, 0));
        }

        cout << "\n=== ОБЩАЯ ВЫРУЧКА ===" << endl;
        cout << "Выручка: " << fixed << setprecision(2) << revenue << " руб." << endl;

        PQclear(res);
    }

    void showSoldQuantities() {
        string sql = "SELECT p.id, p.name, SUM(s.quantity_sold) as total_sold "
            "FROM sales s JOIN products p ON s.product_id = p.id "
            "GROUP BY p.id, p.name ORDER BY total_sold DESC";

        PGresult* res = db->query(sql);
        if (!res) {
            cout << "Ошибка запроса!" << endl;
            return;
        }

        int rows = PQntuples(res);
        cout << "\n=== КОЛИЧЕСТВО ПРОДАННЫХ ЕДИНИЦ ===" << endl;

        if (rows == 0) {
            cout << "Нет данных о продажах" << endl;
        }
        else {
            for (int i = 0; i < rows; i++) {
                cout << "ID: " << PQgetvalue(res, i, 0)
                    << " | Продукт: " << PQgetvalue(res, i, 1)
                    << " | Продано: " << PQgetvalue(res, i, 2) << " шт." << endl;
            }
        }
        PQclear(res);
    }

    void showTopProducts() {
        string sql = "SELECT p.name, SUM(s.quantity_sold) as total_sold "
            "FROM sales s JOIN products p ON s.product_id = p.id "
            "GROUP BY p.name ORDER BY total_sold DESC LIMIT 5";

        PGresult* res = db->query(sql);
        if (!res) {
            cout << "Ошибка запроса!" << endl;
            return;
        }

        int rows = PQntuples(res);
        cout << "\n=== ТОП-5 ПРОДУКТОВ ===" << endl;

        if (rows == 0) {
            cout << "Нет данных о продажах" << endl;
        }
        else {
            for (int i = 0; i < rows; i++) {
                cout << i + 1 << ". " << PQgetvalue(res, i, 0)
                    << " - " << PQgetvalue(res, i, 1) << " шт." << endl;
            }
        }
        PQclear(res);
    }
};

int main() {
    HWND hwnd = FindWindow(NULL, L"Система управления продуктами");
    if (hwnd) {
        cout << "Программа уже запущена! Закройте предыдущее окно." << endl;
        system("pause");
        return 1;
    }

    SetConsoleTitle(L"Система управления продуктами");

    Database db;

    string connStr = "host=localhost port=5432 dbname=products_db "
        "user=postgres password=667480410";

    if (!db.connect(connStr)) {
        cout << "\nОШИБКА ПОДКЛЮЧЕНИЯ К БАЗЕ ДАННЫХ!" << endl;
        system("pause");
        return 1;
    }

    db.executeSQL("SET client_min_messages TO WARNING;", true);

    db.executeSQL(
        "CREATE TABLE IF NOT EXISTS categories ("
        "    id SERIAL PRIMARY KEY,"
        "    category_name VARCHAR(100) NOT NULL UNIQUE"
        ")", true
    );

    db.executeSQL(
        "CREATE TABLE IF NOT EXISTS products ("
        "    id SERIAL PRIMARY KEY,"
        "    name VARCHAR(200) NOT NULL,"
        "    price DECIMAL(10, 2) NOT NULL,"
        "    quantity INTEGER DEFAULT 0,"
        "    category_id INTEGER REFERENCES categories(id)"
        ")", true
    );

    db.executeSQL(
        "CREATE TABLE IF NOT EXISTS sales ("
        "    id SERIAL PRIMARY KEY,"
        "    product_id INTEGER REFERENCES products(id),"
        "    sale_date DATE DEFAULT CURRENT_DATE,"
        "    quantity_sold INTEGER NOT NULL"
        ")", true
    );

    db.executeSQL("CREATE INDEX IF NOT EXISTS idx_categories_name ON categories(category_name)", true);
    db.executeSQL("CREATE INDEX IF NOT EXISTS idx_products_name ON products(name)", true);
    db.executeSQL("CREATE INDEX IF NOT EXISTS idx_sales_date ON sales(sale_date)", true);

    db.executeSQL("SET client_min_messages TO NOTICE;", true);

    PGresult* res = db.query("SELECT COUNT(*) as count FROM categories");
    if (res && PQntuples(res) > 0) {
        int count = stoi(PQgetvalue(res, 0, 0));
        if (count == 0) {
            db.executeSQL("INSERT INTO categories (category_name) VALUES "
                "('Электроника'), ('Одежда'), ('Книги'), ('Программы')", true);
            db.executeSQL("INSERT INTO products (name, price, quantity, category_id) VALUES "
                "('Наушники', 5000, 100, 1), "
                "('Телефон', 50000, 15, 1), "
                "('Футболка', 2000, 50, 2), "
                "('Учебник', 1000, 300, 3), "
                "('Винда10', 3000, 100, 4)", true);
        }
    }
    if (res) PQclear(res);

    Menu menu(&db);
    menu.run();

    system("pause");
    return 0;
}