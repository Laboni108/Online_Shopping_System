# Online Shopping System — C++ OOP Project

A fully functional console-based online shopping system demonstrating core
Object-Oriented Programming principles in C++17.

---

## Features

### User Authentication
- Login / Logout with credential validation
- New customer registration
- Role-based access (Admin vs Customer)

### Product Management (Admin)
- Add, update, remove products
- Set discount percentages per product
- View all products, discounted items

### Shopping (Customer)
- Browse all products in a tabular view
- Search by keyword (name + description)
- Filter by category (Electronics, Clothing, Books, Food, Sports, Other)
- Filter by price range
- View products on discount

### Shopping Cart
- Add items (with real-time stock deduction)
- Remove items (stock restored)
- View cart with subtotals and grand total

### Checkout & Orders
- Shipping address management
- Four polymorphic payment methods:
  - Credit Card (card number + holder name)
  - Debit Card
  - Bank Transfer (bank name + account)
  - Cash on Delivery
- Automatic order ID generation
- Order history per customer

### Order Management (Admin)
- View all orders across all customers
- Update order status: Pending → Confirmed → Shipped → Delivered / Cancelled

---

## OOP Concepts Applied

| Concept         | Where Used |
|----------------|------------|
| **Encapsulation** | Product, Cart, Order — private fields with public getters/setters |
| **Inheritance**   | `User` → `Customer`, `User` → `Admin` |
| **Polymorphism**  | `Payment` → `CreditCardPayment`, `DebitCardPayment`, `BankTransferPayment`, `CashOnDeliveryPayment`; virtual `showMenu()` on User subclasses |
| **Abstraction**   | `Payment` is a pure abstract base class |
| **Smart Pointers**| `shared_ptr<Payment>` for safe memory management in Orders |
| **STL Containers**| `vector`, `map`, `algorithm` (find_if, transform) |

---

## Build & Run

### Requirements
- g++ with C++17 support (GCC 7+ or Clang 5+)

### Compile
```bash
make
# or manually:
g++ -std=c++17 -Wall -O2 -o shopping_system main.cpp
```

### Run
```bash
./shopping_system
# or:
make run
```

---

## Demo Accounts

| Role     | Username | Password   |
|----------|----------|------------|
| Admin    | admin    | admin123   |
| Customer | alice    | alice123   |
| Customer | bob      | bob123     |

---

## Project Structure

```
shopping_system/
├── main.cpp        ← All source code (single-file, self-contained)
├── Makefile        ← Build automation
└── README.md       ← This file
```

All classes are in `main.cpp` for portability. In a production setup they
would be split into `.h` header files and `.cpp` implementation files.

---

## Class Diagram (Simplified)

```
User (abstract-like)
├── Customer  ──has──▶  Cart  ──has──▶  CartItem  ──ref──▶  Product
└── Admin

Payment (abstract)
├── CreditCardPayment
├── DebitCardPayment
├── BankTransferPayment
└── CashOnDeliveryPayment

Order ──ref──▶  CartItem[]  +  Payment

ShoppingSystem
├── vector<Product>
├── vector<Customer*>
├── vector<Admin*>
└── User* loggedIn
```
