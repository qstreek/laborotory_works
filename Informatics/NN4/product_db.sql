CREATE TABLE categories (
    id SERIAL PRIMARY KEY,
    category_name VARCHAR(255) NOT NULL
);

CREATE TABLE products (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    price FLOAT NOT NULL,
    quantity INTEGER NOT NULL,
    category_id INTEGER REFERENCES categories(id) ON DELETE CASCADE
);

CREATE TABLE sales (
    id SERIAL PRIMARY KEY,
    product_id INTEGER REFERENCES products(id) ON DELETE CASCADE,
    sale_date DATE NOT NULL,
    quantity_sold INTEGER NOT NULL
);

CREATE INDEX idx_category_name ON categories (category_name);
CREATE INDEX idx_product_name ON products (name);
CREATE INDEX idx_sale_date ON sales (sale_date);

INSERT INTO categories (category_name) VALUES 
('Электроника'),
('Одежда'),
('Книги'),
('Программы');

INSERT INTO products (name, price, quantity, category_id) VALUES
('Наушники', 5000.00, 100, 1),
('Телефон', 50000.00, 15, 1),
('Футболка', 2000.00, 50, 2),
('Учебник', 1000.00, 300, 3),
('Винда10', 3000, 100, 4);