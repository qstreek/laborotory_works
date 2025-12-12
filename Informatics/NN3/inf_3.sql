
CREATE TABLE categories (
	id SERIAL PRIMARY KEY,
	category_name VARCHAR(200)
);

CREATE TABLE products (
	id SERIAL PRIMARY KEY,
	name VARCHAR(200) NOT NULL,
	price NUMERIC(10, 2) NOT NULL CHECK (price >= 0),
	quantity INTEGER NOT NULL CHECK (quantity >= 0),
	category_id INTEGER,
	FOREIGN KEY (category_id) REFERENCES categories(id) ON DELETE SET NULL	
);

CREATE TABLE sales (
	id SERIAL PRIMARY KEY,
	product_id INTEGER NOT NULL,
    sale_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    quantity_sold INTEGER NOT NULL CHECK (quantity_sold > 0),
    FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE
);

INSERT INTO categories (category_name) VALUES
('Овощи'),
('Фрукты'),
('Молочные продукты'),
('Напитки'),
('Хлебобулочные изделия');

INSERT INTO products (name, price, quantity, category_id) VALUES
('Помидоры', 120.50, 100, 1),
('Огурцы', 80.00, 150, 1),
('Яблоки', 90.00, 200, 2),
('Бананы', 110.00, 80, 2),
('Молоко', 75.00, 50, 3),
('Сыр', 350.00, 30, 3),
('Кола', 95.00, 120, 4),
('Хлеб белый', 45.00, 40, 5),
('Картофель', 40.00, 300, 1),
('Апельсины', 130.00, 60, 2);

INSERT INTO sales (product_id, sale_date, quantity_sold) VALUES
(1, '2000-01-15 10:30:00', 5),
(3, '2001-01-15 11:15:00', 10),
(5, '2020-01-16 09:45:00', 3),
(1, '2000-01-17 14:20:00', 8),
(2, '2025-01-17 16:40:00', 12),
(6, '2022-01-18 12:10:00', 2),
(3, '2021-01-19 10:00:00', 15),
(8, '2024-01-19 17:30:00', 7),
(4, '2024-01-20 13:25:00', 6);

SELECT p.id, p.name, p.price, p.quantity, c.category_name
FROM products p
JOIN categories c ON p.category_id = c.id
WHERE c.category_name = 'Овощи';

SELECT p.id, p.name, p.price, p.quantity, c.category_name
FROM products p
JOIN categories c ON p.category_id = c.id
WHERE p.quantity < 50;

SELECT 
    SUM(s.quantity_sold * p.price) AS total_sales_amount
FROM sales s
JOIN products p ON s.product_id = p.id
WHERE s.sale_date >= '2000-01-15' 
  AND s.sale_date < '2025-01-19';

UPDATE products p
SET quantity = quantity - COALESCE(
    (SELECT SUM(quantity_sold) 
     FROM sales s 
     WHERE s.product_id = p.id), 
    0
);