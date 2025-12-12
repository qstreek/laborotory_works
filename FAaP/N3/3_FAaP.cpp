#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <limits>

using namespace std;

struct Product {
    string name;
    double price;
    int quantity;
};

void displayMenu() {
    cout << "\n=== MENU UPRAVLENIA PRODUCT ===\n";
    cout << "1. Dobavit new products\n";
    cout << "2. Naiti producy po nazvaniu\n";
    cout << "3. Sortirovat po zene\n";
    cout << "4. Sortirovat po kol-vu\n";
    cout << "5. Dobavit odin product\n";
    cout << "6. Poisk productov po max zene\n";
    cout << "7. Pokazat all products\n";
    cout << "8. Exite\n";
    cout << "Vibirite deistvie (1-8): ";
}

// Chtenie all product on file
vector<Product> readProductsFromFile(const string& filename) {
    vector<Product> products;
    ifstream file(filename);

    if (!file.is_open()) {
        return products;
    }

    Product product;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string name, priceStr, quantityStr;

        if (getline(ss, name, ';') && getline(ss, priceStr, ';') && getline(ss, quantityStr)) {
            product.name = name;
            product.price = stod(priceStr);
            product.quantity = stoi(quantityStr);
            products.push_back(product);
        }
    }

    file.close();
    return products;
}

// Zapis product v file
void writeProductsToFile(const string& filename, const vector<Product>& products) {
    ofstream file(filename);

    if (!file.is_open()) {
        cout << "error open file" << endl;
        return;
    }

    for (const auto& product : products) {
        file << product.name << ";" << product.price << ";" << product.quantity << endl;
    }

    file.close();
}

// +product (Ex. 1)
void addProducts() {
    vector<Product> products = readProductsFromFile("products.txt");
    ofstream outputFile("output.txt", ios::app);

    cout << "\n=== DOBAVLENIE NEW PRODUCT ===\n";
    cout << "VVODITE DANNIE O PRODUCT.\n";

    int count = 0;
    while (true) {
        Product product;

        cout << "\nProduct #" << (products.size() + count + 1) << ":\n";

        cout << "Nazvanie: ";
        getline(cin, product.name);

        if (product.name.empty()) {
            cout << "Zavershenie vvoda..." << endl;
            break;
        }

        while (true) {
            cout << "Price: ";
            string priceStr;
            getline(cin, priceStr);

            try {
                product.price = stod(priceStr);
                if (product.price >= 0) break;
                else cout << "nado >=0: ";
            }
            catch (...) {
                cout << "Nevernii fomat. Poprobuite snovo: ";
            }
        }

        while (true) {
            cout << "Kol-vo: ";
            string quantityStr;
            getline(cin, quantityStr);

            try {
                product.quantity = stoi(quantityStr);
                if (product.quantity >= 0) break;
                else cout << "Nado >=0: ";
            }
            catch (...) {
                cout << "Nevernii fomat. Poprobuite snovo: ";
            }
        }

        products.push_back(product);
        count++;

        outputFile << "Dobavlen product: " << product.name
            << ", Price: " << product.price
            << ", Kol-vo: " << product.quantity << endl;
    }

    writeProductsToFile("products.txt", products);

    cout << "Dobavleno products: " << count << endl;
    outputFile << "Vsego dobavleno: " << count << "\n" << endl;
    outputFile.close();
}

void searchProductByName() {
    vector<Product> products = readProductsFromFile("products.txt");
    ofstream outputFile("output.txt", ios::app);

    if (products.empty()) {
        cout << "file pust or netu" << endl;
        outputFile << "Pustoi file.\n" << endl;
        outputFile.close();
        return;
    }

    cout << "\n=== poisk po nazvaniu ===\n";
    cout << "vvedite nazvanie: ";

    string searchName;
    getline(cin, searchName);

    bool found = false;

        outputFile << "poisk po nazvaniu: " << searchName << endl;

    for (const auto& product : products) {
        string productNameLower = product.name;
        string searchNameLower = searchName;

        // preobrazuem k nijnemu registru dlia sravnenia
        transform(productNameLower.begin(), productNameLower.end(), productNameLower.begin(), ::tolower);
        transform(searchNameLower.begin(), searchNameLower.end(), searchNameLower.begin(), ::tolower);

        if (productNameLower.find(searchNameLower) != string::npos) {
            cout << "\nNaiden product:\n";
            cout << "Nazvanie: " << product.name << endl;
            cout << "Price: " << fixed << setprecision(2) << product.price << endl;
            cout << "Kol-vo: " << product.quantity << endl;

            outputFile << "Naiden: " << product.name
                << ", Price: " << product.price
                << ", Kol-vo: " << product.quantity << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "Product \"" << searchName << "\" ne naiden." << endl;
        outputFile << "Product ne naiden.\n" << endl;
    }
    else {
        outputFile << "End poiska.\n" << endl;
    }

    outputFile.close();
}

void sortProductsByPrice() {
    vector<Product> products = readProductsFromFile("products.txt");
    ofstream outputFile("output.txt", ios::app);

    if (products.empty()) {
        cout << "file pust or netu" << endl;
        outputFile << "Pustoi file.\n" << endl;
        outputFile.close();
        return;
    }

    // po vozrastaniu
    sort(products.begin(), products.end(),
        [](const Product& a, const Product& b) {
            return a.price < b.price;
        });

    writeProductsToFile("products.txt", products);

    cout << "\n=== PRODUCTS OTSORTIROVANI PO PRICE ===\n";
    outputFile << "Sortirovka po vozrastaniu:" << endl;

    for (const auto& product : products) {
        cout << product.name << " - Price: " << fixed << setprecision(2)
            << product.price << " - Kol-vo: " << product.quantity << endl;

        outputFile << product.name << ", Price: " << product.price
            << ", Kol-vo: " << product.quantity << endl;
    }

    cout << "\nsave in file products.txt" << endl;
    outputFile << "end sortirovki.\n" << endl;
    outputFile.close();
}

void sortProductsByQuantity() {
    vector<Product> products = readProductsFromFile("products.txt");
    ofstream outputFile("output.txt", ios::app);

    if (products.empty()) {
        cout << "pust or netu" << endl;
        outputFile << "pustoi file.\n" << endl;
        outputFile.close();
        return;
    }

    // po ubivaniu
    sort(products.begin(), products.end(),
        [](const Product& a, const Product& b) {
            return a.quantity > b.quantity;
        });

    writeProductsToFile("products.txt", products);

    cout << "\n=== PRODUCTS OTSORTIROVANI PO kol-vo ===\n";
    outputFile << "kol-vo po ubivaniu:" << endl;

    for (const auto& product : products) {
        cout << product.name << " - kol-vo: " << product.quantity
            << " - Price: " << fixed << setprecision(2) << product.price << endl;

        outputFile << product.name << ", Kol-vo: " << product.quantity
            << ", Price: " << product.price << endl;
    }

    cout << "\nsave in file products.txt" << endl;
    outputFile << "end sortirovki.\n" << endl;
    outputFile.close();
}

void addSingleProduct() {
    vector<Product> products = readProductsFromFile("products.txt");
    ofstream outputFile("output.txt", ios::app);

    cout << "\n=== DOBOVLENIE ODNOGO PRODUCT ===\n";

    Product product;

    cout << "Nazvanie: ";
    getline(cin, product.name);

    while (true) {
        cout << "Price: ";
        string priceStr;
        getline(cin, priceStr);

        try {
            product.price = stod(priceStr);
            if (product.price >= 0) break;
            else cout << "Nado >=0: ";
        }
        catch (...) {
            cout << "Ne tot format: ";
        }
    }

        while (true) {
            cout << "Kol-vo: ";
            string quantityStr;
            getline(cin, quantityStr);

            try {
                product.quantity = stoi(quantityStr);
                if (product.quantity >= 0) break;
                else cout << "Nado >=0: ";
            }
            catch (...) {
                cout << "Ne tot format: ";
            }
        }

    products.push_back(product);

    writeProductsToFile("products.txt", products);

    cout << "\nDobavlen!" << endl;
    cout << "Nazvanie: " << product.name << endl;
    cout << "Price: " << fixed << setprecision(2) << product.price << endl;
    cout << "Kol-vo: " << product.quantity << endl;

    outputFile << "Dobavlen: " << product.name
        << ", Price: " << product.price
        << ", Kol-vo: " << product.quantity << "\n" << endl;
    outputFile.close();
}

void searchProductsByMaxPrice() {
    vector<Product> products = readProductsFromFile("products.txt");
    ofstream outputFile("output.txt", ios::app);

    if (products.empty()) {
        cout << "Pust or netu!" << endl;
        outputFile << "poisk v pustom file.\n" << endl;
        outputFile.close();
        return;
    }

    cout << "\n=== POISK PO MAX PRICE ===\n";

    double maxPrice;
    while (true) {
        cout << "Max price: ";
        string priceStr;
        getline(cin, priceStr);

        try {
            maxPrice = stod(priceStr);
            if (maxPrice >= 0) break;
            else cout << "Nado >=0: ";
        }
        catch (...) {
            cout << "Netot format: ";
        }
    }

    cout << "\nProducts with price <= " << maxPrice << ":\n";
    outputFile << "Products with price <= " << maxPrice << ":" << endl;

    bool found = false;
    int count = 0;

    for (const auto& product : products) {
        if (product.price <= maxPrice) {
            cout << "Nazvanie: " << product.name
                << ", Price: " << fixed << setprecision(2) << product.price
                << ", Kol-vo: " << product.quantity << endl;

            outputFile << product.name << ", Price: " << product.price
                << ", Kol-vo: " << product.quantity << endl;

            found = true;
            count++;
        }
    }

    if (!found) {
        cout << "Product with price <= " << maxPrice << " netu." << endl;
        outputFile << "Ne naiden." << endl;
    }
    else {
        cout << "\nNaideno products: " << count << endl;
    }

    outputFile << "End. Naideno: " << count << " products.\n" << endl;
    outputFile.close();
}

void showAllProducts() {
    vector<Product> products = readProductsFromFile("products.txt");

    if (products.empty()) {
        cout << "File pust or netu!" << endl;
        return;
    }

    cout << "\n=== ALL PRODUCTS ===\n";
    cout << left << setw(20) << "nazvanie"
        << setw(10) << "Price"
        << setw(15) << "Kol-vo" << endl;
    cout << string(45, '-') << endl;

    for (const auto& product : products) {
        cout << left << setw(20) << product.name
            << setw(10) << fixed << setprecision(2) << product.price
            << setw(15) << product.quantity << endl;
    }

    cout << "\nAll: " << products.size() << endl;
}

int main() {

    ofstream clearFile("output.txt");
    clearFile << "=== LOG PROGRAMM ===\n" << endl;
    clearFile.close();

    cout << "=== PROGRAMM UPRAVLENIZ PRODUCTS ===\n";

    int choice;
    string input;
    ofstream outputFile;

    do {
        displayMenu();
        getline(cin, input);

        try {
            choice = stoi(input);
        }
        catch (...) {
            choice = 0;
        }

            switch (choice) {
            case 1:
                addProducts();
                break;
            case 2:
                searchProductByName();
                break;
            case 3:
                sortProductsByPrice();
                break;
            case 4:
                sortProductsByQuantity();
                break;
            case 5:
                addSingleProduct();
                break;
            case 6:
                searchProductsByMaxPrice();
                break;
            case 7:
                showAllProducts();
                break;
            case 8:
                cout << "\nEXITE" << endl;

                outputFile.open("output.txt", ios::app);
                outputFile << "=== END PROGRAMM ===\n" << endl;
                outputFile.close();

                break;
            default:
                cout << "NADO OT 1 DO 8." << endl;
            }

        if (choice != 8) {
            cout << "\nPres Enter...";
            getline(cin, input);
        }

    } while (choice != 8);

    return 0;
}