// ============================================================
//   Online Shopping System — C++ OOP Project
//   Features: Auth, Products, Cart, Orders, Payments
// ============================================================

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <memory>
#include <sstream>
#include <limits>

using namespace std;

// ─── Utility ──────────────────────────────────────────────
string currentTimestamp() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now));
    return string(buf);
}

string generateOrderId() {
    static int counter = 1000;
    return "ORD-" + to_string(++counter);
}

void pause() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void clearScreen() {
    cout << string(50, '\n');
}

void printDivider(char c = '-', int width = 60) {
    cout << "  " << string(width, c) << "\n";
}

void printHeader(const string& title) {
    clearScreen();
    printDivider('=');
    cout << "  ◆  " << title << "\n";
    printDivider('=');
    cout << "\n";
}

// ─── Enums ────────────────────────────────────────────────
enum class Category { Electronics, Clothing, Books, Food, Sports, Other };
enum class OrderStatus { Pending, Confirmed, Shipped, Delivered, Cancelled };
enum class PaymentMethod { CreditCard, DebitCard, BankTransfer, CashOnDelivery };

string categoryToString(Category c) {
    switch (c) {
        case Category::Electronics: return "Electronics";
        case Category::Clothing:    return "Clothing";
        case Category::Books:       return "Books";
        case Category::Food:        return "Food";
        case Category::Sports:      return "Sports";
        default:                    return "Other";
    }
}

string orderStatusToString(OrderStatus s) {
    switch (s) {
        case OrderStatus::Pending:    return "Pending";
        case OrderStatus::Confirmed:  return "Confirmed";
        case OrderStatus::Shipped:    return "Shipped";
        case OrderStatus::Delivered:  return "Delivered";
        case OrderStatus::Cancelled:  return "Cancelled";
        default:                      return "Unknown";
    }
}

string paymentMethodToString(PaymentMethod p) {
    switch (p) {
        case PaymentMethod::CreditCard:     return "Credit Card";
        case PaymentMethod::DebitCard:      return "Debit Card";
        case PaymentMethod::BankTransfer:   return "Bank Transfer";
        case PaymentMethod::CashOnDelivery: return "Cash on Delivery";
        default:                            return "Unknown";
    }
}

// ─── Product ──────────────────────────────────────────────
class Product {
private:
    int id;
    string name;
    string description;
    double price;
    int stock;
    Category category;
    double discountPercent; // 0–100

public:
    Product(int id, const string& name, const string& desc,
            double price, int stock, Category cat, double discount = 0.0)
        : id(id), name(name), description(desc),
          price(price), stock(stock), category(cat), discountPercent(discount) {}

    int    getId()          const { return id; }
    string getName()        const { return name; }
    string getDescription() const { return description; }
    double getPrice()       const { return price; }
    double getDiscountedPrice() const {
        return price * (1.0 - discountPercent / 100.0);
    }
    int    getStock()       const { return stock; }
    Category getCategory()  const { return category; }
    double getDiscount()    const { return discountPercent; }

    void setPrice(double p)    { price = p; }
    void setStock(int s)       { stock = s; }
    void setDiscount(double d) { discountPercent = d; }
    void setName(const string& n) { name = n; }

    bool reduceStock(int qty) {
        if (stock >= qty) { stock -= qty; return true; }
        return false;
    }
    void restoreStock(int qty) { stock += qty; }

    void display(bool compact = false) const {
        if (compact) {
            cout << "  [" << setw(3) << id << "] "
                 << left << setw(25) << name
                 << " $" << fixed << setprecision(2) << getDiscountedPrice();
            if (discountPercent > 0)
                cout << " (was $" << price << ", -" << (int)discountPercent << "%)";
            cout << "  Stock: " << stock
                 << "  [" << categoryToString(category) << "]\n";
        } else {
            printDivider();
            cout << "  Product ID   : " << id << "\n"
                 << "  Name         : " << name << "\n"
                 << "  Description  : " << description << "\n"
                 << "  Category     : " << categoryToString(category) << "\n"
                 << "  Price        : $" << fixed << setprecision(2) << price << "\n";
            if (discountPercent > 0)
                cout << "  Discount     : " << (int)discountPercent << "% OFF  → $"
                     << getDiscountedPrice() << "\n";
            cout << "  Stock        : " << stock << " units\n";
        }
    }
};

// ─── CartItem ─────────────────────────────────────────────
struct CartItem {
    Product* product;
    int quantity;
    double priceAtAdd; // snapshot price

    CartItem(Product* p, int qty)
        : product(p), quantity(qty), priceAtAdd(p->getDiscountedPrice()) {}

    double subtotal() const { return priceAtAdd * quantity; }
};

// ─── Payment (Polymorphism) ────────────────────────────────
class Payment {
protected:
    double amount;
    bool   paid;

public:
    Payment(double amt) : amount(amt), paid(false) {}
    virtual ~Payment() {}

    virtual bool processPayment()      = 0;
    virtual string getMethodName()     = 0;
    virtual void   displayReceipt()    = 0;

    bool isPaid() const { return paid; }
    double getAmount() const { return amount; }
};

class CreditCardPayment : public Payment {
    string cardNumber;
    string cardHolder;
public:
    CreditCardPayment(double amt, const string& num, const string& holder)
        : Payment(amt), cardNumber(num), cardHolder(holder) {}

    bool processPayment() override {
        cout << "\n  Processing Credit Card payment...\n";
        cout << "  Verifying card **** **** **** " << cardNumber.substr(cardNumber.size()-4) << " ...\n";
        paid = true;
        cout << "  ✔  Payment of $" << fixed << setprecision(2) << amount << " approved!\n";
        return true;
    }
    string getMethodName() override { return "Credit Card"; }
    void displayReceipt() override {
        cout << "  Payment Method : Credit Card\n"
             << "  Card Holder    : " << cardHolder << "\n"
             << "  Card Number    : **** **** **** " << cardNumber.substr(cardNumber.size()-4) << "\n"
             << "  Amount Charged : $" << fixed << setprecision(2) << amount << "\n";
    }
};

class DebitCardPayment : public Payment {
    string cardNumber;
public:
    DebitCardPayment(double amt, const string& num)
        : Payment(amt), cardNumber(num) {}

    bool processPayment() override {
        cout << "\n  Processing Debit Card payment...\n";
        paid = true;
        cout << "  ✔  Payment of $" << fixed << setprecision(2) << amount << " successful!\n";
        return true;
    }
    string getMethodName() override { return "Debit Card"; }
    void displayReceipt() override {
        cout << "  Payment Method : Debit Card\n"
             << "  Card Number    : **** " << cardNumber.substr(cardNumber.size()-4) << "\n"
             << "  Amount Debited : $" << fixed << setprecision(2) << amount << "\n";
    }
};

class BankTransferPayment : public Payment {
    string bankName;
    string accountNumber;
public:
    BankTransferPayment(double amt, const string& bank, const string& acc)
        : Payment(amt), bankName(bank), accountNumber(acc) {}

    bool processPayment() override {
        cout << "\n  Initiating Bank Transfer...\n";
        cout << "  Bank: " << bankName << " | Account: ****" << accountNumber.substr(accountNumber.size()-4) << "\n";
        paid = true;
        cout << "  ✔  Transfer of $" << fixed << setprecision(2) << amount << " completed!\n";
        return true;
    }
    string getMethodName() override { return "Bank Transfer"; }
    void displayReceipt() override {
        cout << "  Payment Method : Bank Transfer\n"
             << "  Bank           : " << bankName << "\n"
             << "  Account        : ****" << accountNumber.substr(accountNumber.size()-4) << "\n"
             << "  Amount         : $" << fixed << setprecision(2) << amount << "\n";
    }
};

class CashOnDeliveryPayment : public Payment {
    string deliveryAddress;
public:
    CashOnDeliveryPayment(double amt, const string& addr)
        : Payment(amt), deliveryAddress(addr) {}

    bool processPayment() override {
        cout << "\n  Cash on Delivery selected.\n";
        cout << "  Order will be delivered to: " << deliveryAddress << "\n";
        paid = true;
        cout << "  ✔  COD order confirmed! Pay $" << fixed << setprecision(2) << amount << " on delivery.\n";
        return true;
    }
    string getMethodName() override { return "Cash on Delivery"; }
    void displayReceipt() override {
        cout << "  Payment Method : Cash on Delivery\n"
             << "  Address        : " << deliveryAddress << "\n"
             << "  Amount Due     : $" << fixed << setprecision(2) << amount << "\n";
    }
};

// ─── Order ────────────────────────────────────────────────
struct Order {
    string          orderId;
    string          userId;
    vector<CartItem>items;
    double          totalAmount;
    OrderStatus     status;
    string          placedAt;
    string          shippingAddress;
    shared_ptr<Payment> payment;

    Order(const string& uid, const vector<CartItem>& items,
          double total, const string& addr, shared_ptr<Payment> pay)
        : orderId(generateOrderId()), userId(uid), items(items),
          totalAmount(total), status(OrderStatus::Confirmed),
          placedAt(currentTimestamp()), shippingAddress(addr), payment(pay) {}

    void display() const {
        printDivider();
        cout << "  Order ID       : " << orderId << "\n"
             << "  Placed At      : " << placedAt << "\n"
             << "  Status         : " << orderStatusToString(status) << "\n"
             << "  Ship To        : " << shippingAddress << "\n";
        cout << "\n  Items:\n";
        for (auto& item : items) {
            cout << "    • " << left << setw(22) << item.product->getName()
                 << " x" << item.quantity
                 << "  @ $" << fixed << setprecision(2) << item.priceAtAdd
                 << "  = $" << item.subtotal() << "\n";
        }
        cout << "\n";
        if (payment) payment->displayReceipt();
        printDivider();
        cout << "  TOTAL          : $" << fixed << setprecision(2) << totalAmount << "\n";
    }
};

// ─── User (Base) / Admin / Customer ───────────────────────
class User {
protected:
    string username;
    string password;
    string email;
    string fullName;
    bool   isAdminUser;

public:
    User(const string& u, const string& p, const string& e, const string& name, bool admin = false)
        : username(u), password(p), email(e), fullName(name), isAdminUser(admin) {}
    virtual ~User() {}

    string getUsername() const { return username; }
    string getFullName() const { return fullName; }
    string getEmail()    const { return email; }
    bool   isAdmin()     const { return isAdminUser; }

    bool authenticate(const string& u, const string& p) const {
        return username == u && password == p;
    }

    virtual void displayProfile() const {
        cout << "  Username : " << username << "\n"
             << "  Name     : " << fullName << "\n"
             << "  Email    : " << email << "\n"
             << "  Role     : " << (isAdminUser ? "Administrator" : "Customer") << "\n";
    }
    virtual void showMenu() = 0;
};

// ─── Forward declare system ───────────────────────────────
class ShoppingSystem;

// ─── Cart ─────────────────────────────────────────────────
class Cart {
    vector<CartItem> items;

public:
    void addItem(Product* p, int qty) {
        for (auto& item : items) {
            if (item.product->getId() == p->getId()) {
                item.quantity += qty;
                item.priceAtAdd = p->getDiscountedPrice();
                cout << "  ✔  Updated quantity for '" << p->getName() << "' to " << item.quantity << "\n";
                return;
            }
        }
        items.emplace_back(p, qty);
        cout << "  ✔  Added '" << p->getName() << "' x" << qty << " to cart.\n";
    }

    bool removeItem(int productId) {
        auto it = find_if(items.begin(), items.end(),
            [productId](const CartItem& ci){ return ci.product->getId() == productId; });
        if (it != items.end()) {
            it->product->restoreStock(it->quantity);
            items.erase(it);
            return true;
        }
        return false;
    }

    void clear() {
        for (auto& item : items) item.product->restoreStock(item.quantity);
        items.clear();
    }

    void display() const {
        if (items.empty()) { cout << "  Your cart is empty.\n"; return; }
        cout << "\n";
        printDivider();
        cout << "  SHOPPING CART\n";
        printDivider();
        double total = 0;
        for (auto& item : items) {
            cout << "  • " << left << setw(24) << item.product->getName()
                 << " x" << setw(3) << item.quantity
                 << " @ $" << fixed << setprecision(2) << item.priceAtAdd
                 << "  =  $" << item.subtotal() << "\n";
            total += item.subtotal();
        }
        printDivider();
        cout << "  TOTAL : $" << fixed << setprecision(2) << total << "\n\n";
    }

    double getTotal() const {
        double t = 0;
        for (auto& i : items) t += i.subtotal();
        return t;
    }

    bool isEmpty() const { return items.empty(); }
    const vector<CartItem>& getItems() const { return items; }
    int size() const { return (int)items.size(); }
};

// ─── Customer ─────────────────────────────────────────────
class Customer : public User {
    Cart            cart;
    vector<Order>   orders;
    string          address;

public:
    Customer(const string& u, const string& p, const string& e,
             const string& name, const string& addr = "")
        : User(u, p, e, name, false), address(addr) {}

    Cart& getCart() { return cart; }
    vector<Order>& getOrders() { return orders; }
    string getAddress() const { return address; }
    void   setAddress(const string& a) { address = a; }

    void addOrder(const Order& o) { orders.push_back(o); }

    void displayOrders() const {
        if (orders.empty()) { cout << "  No orders yet.\n"; return; }
        for (auto& o : orders) o.display();
    }

    void displayProfile() const override {
        User::displayProfile();
        cout << "  Address  : " << (address.empty() ? "(not set)" : address) << "\n"
             << "  Orders   : " << orders.size() << "\n";
    }

    void showMenu() override; // defined after ShoppingSystem
};

// ─── Admin ────────────────────────────────────────────────
class Admin : public User {
public:
    Admin(const string& u, const string& p, const string& e, const string& name)
        : User(u, p, e, name, true) {}

    void showMenu() override; // defined after ShoppingSystem
};

// ─── Shopping System ──────────────────────────────────────
class ShoppingSystem {
    vector<Product>   products;
    vector<Customer*> customers;
    vector<Admin*>    admins;
    User*             loggedIn;
    int               nextProductId;

public:
    ShoppingSystem() : loggedIn(nullptr), nextProductId(1) {
        seedData();
    }

    ~ShoppingSystem() {
        for (auto c : customers) delete c;
        for (auto a : admins)    delete a;
    }

    // ── Seed ──────────────────────────────────────────────
    void seedData() {
        admins.push_back(new Admin("admin", "admin123", "admin@shop.com", "Admin User"));

        customers.push_back(new Customer("alice", "alice123", "alice@email.com",
                                         "Alice Smith", "42 Maple Street, Dhaka"));
        customers.push_back(new Customer("bob",   "bob123",   "bob@email.com",
                                         "Bob Johnson", "7 Pine Road, Chittagong"));

        products = {
            {nextProductId++, "Laptop Pro 15",    "High-perf laptop, 16GB RAM",   999.99,  15, Category::Electronics, 10},
            {nextProductId++, "Wireless Mouse",   "Ergonomic, 2.4GHz",             29.99,  50, Category::Electronics,  0},
            {nextProductId++, "USB-C Hub",        "7-in-1 multiport adapter",       39.99,  30, Category::Electronics,  5},
            {nextProductId++, "Running Shoes",    "Lightweight trail runners",       89.99,  20, Category::Sports,      15},
            {nextProductId++, "Yoga Mat",         "Non-slip, 6mm thick",            24.99,  40, Category::Sports,       0},
            {nextProductId++, "C++ Primer",       "5th Ed, comprehensive guide",    49.99,  25, Category::Books,        0},
            {nextProductId++, "Clean Code",       "A handbook of agile craftsmanship",34.99,18, Category::Books,        0},
            {nextProductId++, "Cotton T-Shirt",   "100% organic, unisex",           19.99,  60, Category::Clothing,    20},
            {nextProductId++, "Denim Jacket",     "Classic fit, blue denim",        69.99,  15, Category::Clothing,     0},
            {nextProductId++, "Organic Coffee",   "Ethiopian blend, 500g",          14.99, 100, Category::Food,         0},
            {nextProductId++, "Dark Chocolate",   "70% cocoa, 200g bar",             7.99,  80, Category::Food,        10},
            {nextProductId++, "Mechanical KB",    "Cherry MX Blue switches",        119.99,  12, Category::Electronics, 0},
        };
    }

    // ── Product access ────────────────────────────────────
    vector<Product>& getProducts() { return products; }

    Product* findProduct(int id) {
        for (auto& p : products)
            if (p.getId() == id) return &p;
        return nullptr;
    }

    // ── Auth ──────────────────────────────────────────────
    bool login(const string& u, const string& p) {
        for (auto a : admins)
            if (a->authenticate(u, p)) { loggedIn = a; return true; }
        for (auto c : customers)
            if (c->authenticate(u, p)) { loggedIn = c; return true; }
        return false;
    }

    void logout() { loggedIn = nullptr; }
    User* getLoggedIn() { return loggedIn; }

    bool registerCustomer(const string& u, const string& p,
                          const string& e, const string& name, const string& addr) {
        for (auto c : customers)
            if (c->getUsername() == u) return false;
        for (auto a : admins)
            if (a->getUsername() == u) return false;
        customers.push_back(new Customer(u, p, e, name, addr));
        return true;
    }

    // ── Product display ───────────────────────────────────
    void displayProducts(const vector<Product*>& list) {
        if (list.empty()) { cout << "  No products found.\n"; return; }
        cout << "\n  " << string(62, '-') << "\n";
        cout << "  " << left << setw(5) << "ID"
             << setw(26) << "Name"
             << setw(12) << "Price"
             << setw(8)  << "Stock"
             << "Category\n";
        cout << "  " << string(62, '-') << "\n";
        for (auto p : list) p->display(true);
        cout << "  " << string(62, '-') << "\n";
    }

    void showAllProducts() {
        vector<Product*> all;
        for (auto& p : products) all.push_back(&p);
        displayProducts(all);
    }

    void searchProducts(const string& keyword) {
        string kw = keyword;
        transform(kw.begin(), kw.end(), kw.begin(), ::tolower);
        vector<Product*> results;
        for (auto& p : products) {
            string n = p.getName(); transform(n.begin(), n.end(), n.begin(), ::tolower);
            string d = p.getDescription(); transform(d.begin(), d.end(), d.begin(), ::tolower);
            if (n.find(kw) != string::npos || d.find(kw) != string::npos)
                results.push_back(&p);
        }
        cout << "\n  Search results for \"" << keyword << "\":\n";
        displayProducts(results);
    }

    void filterByCategory(Category cat) {
        vector<Product*> results;
        for (auto& p : products)
            if (p.getCategory() == cat) results.push_back(&p);
        cout << "\n  Products in [" << categoryToString(cat) << "]:\n";
        displayProducts(results);
    }

    void filterByPrice(double minP, double maxP) {
        vector<Product*> results;
        for (auto& p : products) {
            double dp = p.getDiscountedPrice();
            if (dp >= minP && dp <= maxP) results.push_back(&p);
        }
        cout << "\n  Products $" << minP << " – $" << maxP << ":\n";
        displayProducts(results);
    }

    void showDiscountedProducts() {
        vector<Product*> results;
        for (auto& p : products)
            if (p.getDiscount() > 0) results.push_back(&p);
        cout << "\n  Products on discount:\n";
        displayProducts(results);
    }

    // ── Cart + Checkout ───────────────────────────────────
    void addToCart(Customer* cust, int productId, int qty) {
        Product* p = findProduct(productId);
        if (!p)              { cout << "  ✘  Product not found.\n"; return; }
        if (p->getStock() < qty) { cout << "  ✘  Insufficient stock.\n"; return; }
        p->reduceStock(qty);
        cust->getCart().addItem(p, qty);
    }

    void checkout(Customer* cust) {
        Cart& cart = cust->getCart();
        if (cart.isEmpty()) { cout << "  ✘  Your cart is empty.\n"; return; }

        cart.display();
        double total = cart.getTotal();

        string addr = cust->getAddress();
        cout << "  Shipping address: " << (addr.empty() ? "(none)" : addr) << "\n";
        cout << "  Enter new address (or press Enter to keep): ";
        string newAddr;
        cin.ignore(); getline(cin, newAddr);
        if (!newAddr.empty()) { addr = newAddr; cust->setAddress(addr); }
        if (addr.empty()) { cout << "  ✘  Address required.\n"; return; }

        // Payment selection
        cout << "\n  Select payment method:\n"
             << "  [1] Credit Card\n"
             << "  [2] Debit Card\n"
             << "  [3] Bank Transfer\n"
             << "  [4] Cash on Delivery\n"
             << "  > ";
        int choice; cin >> choice;

        shared_ptr<Payment> payment;
        string cardNum, holder, bank, acc;

        switch (choice) {
            case 1:
                cout << "  Card number (16 digits): "; cin >> cardNum;
                cout << "  Card holder name: "; cin.ignore(); getline(cin, holder);
                payment = make_shared<CreditCardPayment>(total, cardNum, holder);
                break;
            case 2:
                cout << "  Debit card number: "; cin >> cardNum;
                payment = make_shared<DebitCardPayment>(total, cardNum);
                break;
            case 3:
                cout << "  Bank name: "; cin.ignore(); getline(cin, bank);
                cout << "  Account number: "; getline(cin, acc);
                payment = make_shared<BankTransferPayment>(total, bank, acc);
                break;
            default:
                payment = make_shared<CashOnDeliveryPayment>(total, addr);
        }

        if (!payment->processPayment()) {
            cout << "  ✘  Payment failed.\n"; return;
        }

        Order order(cust->getUsername(), cart.getItems(), total, addr, payment);
        cust->addOrder(order);
        cart.getItems(); // snapshot already taken in Order ctor
        // clear without restoring (stock already consumed)
        // We must clear items without restoring stock
        // Use a workaround: drain items manually
        vector<CartItem> cleared = cart.getItems();
        // temporarily zero qtys to skip restoreStock
        // Actually simplest: just wipe using internal clear
        // We'll do this by manually emptying (items made public via friend or accessor)
        // Since Cart doesn't expose mutable vector, we call removeItem but skip restore
        // Solution: add a clearWithoutRestore method conceptually:
        // For this self-contained file, let's call the existing clear() but first zero stock-tracking
        // Best approach here: just call clear() – it restores stock, which is wrong after checkout.
        // Fix: since order creation uses snapshot, we just call the hacky workaround below.
        // We already reduced stock when addToCart was called. Cart::clear() would restore.
        // Simplest fix: create Order first (done), then restore stock amounts manually:
        // Actually let's just NOT call clear() and instead leave cart logically cleared via a flag.
        // For this project, let's add a direct clear-without-restore to Cart by rebuilding items vector.
        // The cleanest solution without changing Cart signature: use clear() and re-reduce stock.
        for (auto& item : cleared) item.product->reduceStock(0); // no-op but valid call
        // Just mark cart items empty by clearing them properly
        // We'll add a forceClear method conceptually inline:
        const_cast<vector<CartItem>&>(cart.getItems()); // read-only — won't compile modification
        // FINAL APPROACH: Add forceClear to Cart class ← already done if we redesign slightly.
        // For this self-contained demo, we'll just note items and force-quit.
        // Let's just call clear() after restoring order items stock — logically consistent:
        // i.e. we accept stock is briefly double-reduced then re-restored; net effect is 0 which is wrong.
        // CORRECT SIMPLE APPROACH: revert all stock during clear(), then re-deduct for the order placed.
        cart.clear(); // restores stock
        // Re-deduct for the placed order:
        for (auto& item : order.items) item.product->reduceStock(item.quantity);

        cout << "\n";
        printDivider('=');
        cout << "  ◆  ORDER PLACED SUCCESSFULLY!\n";
        printDivider('=');
        order.display();
    }

    // ── Admin: add product ────────────────────────────────
    void adminAddProduct() {
        string name, desc;
        double price; int stock, catInt; double discount;

        cout << "  Product Name: "; cin.ignore(); getline(cin, name);
        cout << "  Description : "; getline(cin, desc);
        cout << "  Price ($)   : "; cin >> price;
        cout << "  Stock       : "; cin >> stock;
        cout << "  Category    :\n"
             << "  [1] Electronics  [2] Clothing  [3] Books\n"
             << "  [4] Food         [5] Sports    [6] Other\n"
             << "  > "; cin >> catInt;
        cout << "  Discount (%) : "; cin >> discount;

        Category cat = static_cast<Category>(catInt - 1);
        products.emplace_back(nextProductId++, name, desc, price, stock, cat, discount);
        cout << "  ✔  Product added (ID: " << nextProductId-1 << ").\n";
    }

    void adminUpdateProduct() {
        int id; cout << "  Product ID to update: "; cin >> id;
        Product* p = findProduct(id);
        if (!p) { cout << "  ✘  Not found.\n"; return; }
        p->display();
        cout << "\n  [1] Name  [2] Price  [3] Stock  [4] Discount  > ";
        int ch; cin >> ch;
        switch (ch) {
            case 1: { string n; cout << "  New name: "; cin.ignore(); getline(cin, n); p->setName(n); break; }
            case 2: { double pr; cout << "  New price: "; cin >> pr; p->setPrice(pr); break; }
            case 3: { int st; cout << "  New stock: "; cin >> st; p->setStock(st); break; }
            case 4: { double d; cout << "  New discount (%): "; cin >> d; p->setDiscount(d); break; }
        }
        cout << "  ✔  Updated.\n";
    }

    void adminRemoveProduct() {
        int id; cout << "  Product ID to remove: "; cin >> id;
        auto it = find_if(products.begin(), products.end(),
                          [id](const Product& p){ return p.getId() == id; });
        if (it == products.end()) { cout << "  ✘  Not found.\n"; return; }
        cout << "  Removing: " << it->getName() << " — confirm? [y/n] ";
        char c; cin >> c;
        if (c == 'y') { products.erase(it); cout << "  ✔  Removed.\n"; }
    }

    void adminListUsers() {
        cout << "\n  ADMINS:\n";
        for (auto a : admins)
            cout << "    • " << a->getUsername() << "  (" << a->getEmail() << ")\n";
        cout << "\n  CUSTOMERS (" << customers.size() << "):\n";
        for (auto c : customers) {
            cout << "    • " << left << setw(15) << c->getUsername()
                 << setw(25) << c->getEmail()
                 << " Orders: " << c->getOrders().size() << "\n";
        }
    }

    void adminViewOrders() {
        bool any = false;
        for (auto c : customers) {
            if (!c->getOrders().empty()) {
                cout << "\n  Customer: " << c->getFullName() << " (" << c->getUsername() << ")\n";
                c->displayOrders();
                any = true;
            }
        }
        if (!any) cout << "  No orders placed yet.\n";
    }

    void adminUpdateOrderStatus() {
        string oid; cout << "  Order ID: "; cin >> oid;
        for (auto c : customers) {
            for (auto& o : c->getOrders()) {
                if (o.orderId == oid) {
                    cout << "  Current status: " << orderStatusToString(o.status) << "\n"
                         << "  [1] Pending  [2] Confirmed  [3] Shipped  [4] Delivered  [5] Cancelled\n  > ";
                    int ch; cin >> ch;
                    const_cast<Order&>(o).status = static_cast<OrderStatus>(ch - 1);
                    cout << "  ✔  Status updated.\n";
                    return;
                }
            }
        }
        cout << "  ✘  Order not found.\n";
    }
};

// ─── Customer Menu ────────────────────────────────────────
void Customer::showMenu() {
    // Menu handled in main loop; this is a hook for polymorphism
}

void Admin::showMenu() {
    // Same
}

// ─── Main ─────────────────────────────────────────────────
ShoppingSystem sys;

void customerMenu(Customer* cust) {
    while (true) {
        printHeader("CUSTOMER MENU  •  Welcome, " + cust->getFullName());
        cout << "  [1]  Browse All Products\n"
             << "  [2]  Search Products\n"
             << "  [3]  Filter by Category\n"
             << "  [4]  Filter by Price Range\n"
             << "  [5]  View Discounted Products\n"
             << "  [6]  View Product Details\n"
             << "  [7]  Add to Cart\n"
             << "  [8]  View Cart\n"
             << "  [9]  Remove from Cart\n"
             << "  [10] Checkout\n"
             << "  [11] My Orders\n"
             << "  [12] My Profile\n"
             << "  [0]  Logout\n\n"
             << "  > ";
        int ch; cin >> ch;

        if (ch == 0) break;
        switch (ch) {
            case 1: printHeader("ALL PRODUCTS"); sys.showAllProducts(); pause(); break;
            case 2: {
                string kw; cout << "  Search: "; cin.ignore(); getline(cin, kw);
                sys.searchProducts(kw); pause(); break;
            }
            case 3: {
                cout << "  [1] Electronics  [2] Clothing  [3] Books  [4] Food  [5] Sports  [6] Other\n  > ";
                int c; cin >> c;
                sys.filterByCategory(static_cast<Category>(c-1)); pause(); break;
            }
            case 4: {
                double mn, mx;
                cout << "  Min price: "; cin >> mn;
                cout << "  Max price: "; cin >> mx;
                sys.filterByPrice(mn, mx); pause(); break;
            }
            case 5: sys.showDiscountedProducts(); pause(); break;
            case 6: {
                int id; cout << "  Product ID: "; cin >> id;
                auto p = sys.findProduct(id);
                if (p) p->display(); else cout << "  ✘  Not found.\n";
                pause(); break;
            }
            case 7: {
                sys.showAllProducts();
                int id, qty;
                cout << "  Product ID: "; cin >> id;
                cout << "  Quantity  : "; cin >> qty;
                sys.addToCart(cust, id, qty); pause(); break;
            }
            case 8: printHeader("YOUR CART"); cust->getCart().display(); pause(); break;
            case 9: {
                cust->getCart().display();
                int id; cout << "  Product ID to remove: "; cin >> id;
                if (cust->getCart().removeItem(id)) cout << "  ✔  Removed.\n";
                else cout << "  ✘  Item not found in cart.\n";
                pause(); break;
            }
            case 10: printHeader("CHECKOUT"); sys.checkout(cust); pause(); break;
            case 11: printHeader("MY ORDERS"); cust->displayOrders(); pause(); break;
            case 12: printHeader("MY PROFILE"); cust->displayProfile(); pause(); break;
            default: cout << "  Invalid option.\n"; pause();
        }
    }
}

void adminMenu(Admin* admin) {
    while (true) {
        printHeader("ADMIN PANEL  •  " + admin->getFullName());
        cout << "  [1]  View All Products\n"
             << "  [2]  Add Product\n"
             << "  [3]  Update Product\n"
             << "  [4]  Remove Product\n"
             << "  [5]  List All Users\n"
             << "  [6]  View All Orders\n"
             << "  [7]  Update Order Status\n"
             << "  [8]  View Discounted Products\n"
             << "  [0]  Logout\n\n"
             << "  > ";
        int ch; cin >> ch;

        if (ch == 0) break;
        switch (ch) {
            case 1: printHeader("ALL PRODUCTS"); sys.showAllProducts(); pause(); break;
            case 2: printHeader("ADD PRODUCT"); sys.adminAddProduct(); pause(); break;
            case 3: printHeader("UPDATE PRODUCT"); sys.adminUpdateProduct(); pause(); break;
            case 4: printHeader("REMOVE PRODUCT"); sys.adminRemoveProduct(); pause(); break;
            case 5: printHeader("USER LIST"); sys.adminListUsers(); pause(); break;
            case 6: printHeader("ALL ORDERS"); sys.adminViewOrders(); pause(); break;
            case 7: printHeader("UPDATE ORDER"); sys.adminUpdateOrderStatus(); pause(); break;
            case 8: sys.showDiscountedProducts(); pause(); break;
            default: cout << "  Invalid option.\n"; pause();
        }
    }
}

void mainMenu() {
    while (true) {
        printHeader("ONLINE SHOPPING SYSTEM");
        cout << "  [1]  Login\n"
             << "  [2]  Register\n"
             << "  [0]  Exit\n\n"
             << "  > ";
        int ch; cin >> ch;

        if (ch == 0) {
            cout << "\n  Thank you for visiting. Goodbye!\n\n";
            break;
        }
        if (ch == 1) {
            string u, p;
            cout << "  Username: "; cin >> u;
            cout << "  Password: "; cin >> p;
            if (sys.login(u, p)) {
                User* user = sys.getLoggedIn();
                cout << "  ✔  Welcome, " << user->getFullName() << "!\n";
                pause();
                if (user->isAdmin())
                    adminMenu(dynamic_cast<Admin*>(user));
                else
                    customerMenu(dynamic_cast<Customer*>(user));
                sys.logout();
            } else {
                cout << "  ✘  Invalid credentials.\n"; pause();
            }
        } else if (ch == 2) {
            string u, p, e, name, addr;
            cout << "  Username : "; cin >> u;
            cout << "  Password : "; cin >> p;
            cout << "  Email    : "; cin >> e;
            cout << "  Full Name: "; cin.ignore(); getline(cin, name);
            cout << "  Address  : "; getline(cin, addr);
            if (sys.registerCustomer(u, p, e, name, addr))
                cout << "  ✔  Registered! You can now login.\n";
            else
                cout << "  ✘  Username already taken.\n";
            pause();
        }
    }
}

int main() {
    cout << "\n";
    printDivider('=');
    cout << "  ◆◆  ONLINE SHOPPING SYSTEM  ◆◆\n";
    cout << "  Built with C++ OOP Principles\n";
    printDivider('=');
    cout << "\n  Demo accounts:\n"
         << "  Admin    → username: admin   password: admin123\n"
         << "  Customer → username: alice   password: alice123\n"
         << "  Customer → username: bob     password: bob123\n\n";
    pause();
    mainMenu();
    return 0;
}
