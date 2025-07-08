#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

// Date Helper
time_t getCurrentDate() {
    return time(0);
}

time_t parseDate(const string& dateStr) {
    int year, month, day;
    char dash1, dash2;
    istringstream ss(dateStr);
    ss >> year >> dash1 >> month >> dash2 >> day;

    if (ss.fail() || dash1 != '-' || dash2 != '-' ||
        year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
        throw invalid_argument("Invalid date format. Use YYYY-MM-DD.");
    }

    struct tm tmStruct = {};
    tmStruct.tm_year = year - 1900;
    tmStruct.tm_mon = month - 1;
    tmStruct.tm_mday = day;
    tmStruct.tm_hour = 0;
    tmStruct.tm_min = 0;
    tmStruct.tm_sec = 0;
    tmStruct.tm_isdst = -1;

    return mktime(&tmStruct);
}

bool isExpired(time_t expiryDate) {
    return expiryDate <= getCurrentDate();
}


class Product {
public:
    string name;
    double price;
    int quantity;
    bool is_expirable = false;
    bool is_shippable = false;
    double weight = 0.0; // kg
    time_t expiry_date;

    // Non-expirable product
    Product(string name, double price, int quantity)
        : name(name), price(price), quantity(quantity) {}

    // Expirable product
    Product(string name, double price, int quantity, const string& expiryDateStr)
        : name(name), price(price), quantity(quantity),
          is_expirable(true), expiry_date(parseDate(expiryDateStr)) {}

    bool hasExpired() const {
        return is_expirable && ::isExpired(expiry_date);
    }
};


class Customer {
public:
    string name;
    double balance;

    Customer(string name, double balance)
        : name(name), balance(balance) {}

    void deduct(double amount) {
        balance -= amount;
    }

    double getBalance() const {
        return balance;
    }
};


class CartItem {
public:
    Product* product;
    int quantity;

    CartItem(Product* p, int qty) : product(p), quantity(qty) {}
};

class Cart {
public:
    vector<CartItem> items;

    void add(Product* product, int quantity) {
        if (quantity > product->quantity) {
            cout << "Error: Not enough stock for " << product->name << endl;
            return;
        }
        items.push_back(CartItem(product, quantity));
    }

    void remove(int index) {
        if (index >= 0 && index < items.size()) {
            items.erase(items.begin() + index);
        }
    }

    void clear() {
        items.clear();
    }

    vector<CartItem> getItems() {
        return items;
    }
};

pair<double, map<string, double>> ship(vector<pair<string, double>> shippables) {
    map<string, double> grouped;
    double totalWeight = 0.0;

    for (auto& [name, w] : shippables) {
        grouped[name] += w;
        totalWeight += w;
    }

    double shippingFee = totalWeight * 10; // $10 per kg
    return {shippingFee, grouped};
}

void checkout(Customer& customer, Cart& cart);

void handleExpiredProduct(Customer& customer, Cart& cart, int index) {
    cout << "Product '" << cart.items[index].product->name << "' is expired!" << endl;
    cout << "Do you want to:\n";
    cout << "1. Remove it from cart\n";
    cout << "2. Replace it with another product\n";
    cout << "Enter choice (1 or 2): ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        cart.remove(index);
        cout << "Removed expired product.\n";
        checkout(customer, cart); // Retry checkout
    } else if (choice == 2) {
        cout << "Available products:\n";
        cout << "1. Cheese\n";
        cout << "2. Biscuits\n";
        cout << "3. TV\n";
        cout << "4. Laptop\n";
        cout << "5. Headphones\n";
        cout << "6. Mobile Scratch Card\n";
        cout << "7. E-Book Voucher\n";
        cout << "8. Online Course Access Code\n";
        cout << "Select replacement product (1-8): ";
        int prodChoice;
        cin >> prodChoice;

        static Product cheese("Cheese", 5.0, 10, "2025-12-31");
        static Product biscuits("Biscuits", 3.0, 20, "2025-12-31");
        static Product tv("TV", 500, 3);
        static Product laptop("Laptop", 1200, 5);
        static Product headphones("Headphones", 100, 10);
        static Product scratchCard("Mobile Scratch Card", 10, 100);
        static Product eBookVoucher("E-Book Voucher", 8, 200);
        static Product courseCode("Online Course Access Code", 50, 80);

        Product* newProduct = nullptr;

        switch (prodChoice) {
            case 1: newProduct = &cheese; break;
            case 2: newProduct = &biscuits; break;
            case 3: newProduct = &tv; break;
            case 4: newProduct = &laptop; break;
            case 5: newProduct = &headphones; break;
            case 6: newProduct = &scratchCard; break;
            case 7: newProduct = &eBookVoucher; break;
            case 8: newProduct = &courseCode; break;
            default:
                cout << "Invalid choice. Skipping replacement.\n";
                return;
        }

        int qty;
        cout << "Enter quantity: ";
        cin >> qty;

        cart.items[index] = CartItem(newProduct, qty);
        cout << "Replaced expired product.\n";
        checkout(customer, cart); // Recalculate
    } else {
        cout << "Invalid choice. Try again.\n";
        handleExpiredProduct(customer, cart, index);
    }
}

void checkout(Customer& customer, Cart& cart) {
    if (cart.items.empty()) {
        cout << "Error: Cart is empty!" << endl;
        return;
    }

    double subtotal = 0.0;
    vector<pair<string, double>> shippables;

    for (size_t i = 0; i < cart.items.size(); ++i) {
        Product* p = cart.items[i].product;
        int qty = cart.items[i].quantity;

        if (p->hasExpired()) {
            handleExpiredProduct(customer, cart, i);
            return;
        }

        if (p->quantity < qty) {
            cout << "Error: Not enough stock for '" << p->name << "'." << endl;
            return;
        }

        subtotal += p->price * qty;

        if (p->is_shippable) {
            shippables.push_back({p->name, p->weight});
        }
    }

    auto [shippingFee, shippedList] = ship(shippables);
    double totalAmount = subtotal + shippingFee;

    if (customer.getBalance() < totalAmount) {
        cout << "Error: Insufficient balance!" << endl;
        return;
    }


    if (!shippedList.empty()) {
        cout << "\n** Shipment notice **\n";
        for (auto& [name, w] : shippedList) {
            cout << "1x " << name << " " << fixed << setprecision(0) << w * 1000 << "g\n";
        }
        cout << "Total package weight " << fixed << setprecision(1) << (shippingFee / 10) << "kg\n";
    }

    // Print receipt
    cout << "\n** Checkout receipt **\n";
    for (auto item : cart.items) {
        cout << item.quantity << "x " << item.product->name << " "
             << fixed << setprecision(0) << item.product->price * item.quantity << endl;
    }
    cout << "----------------------\n";
    cout << "Subtotal " << fixed << setprecision(0) << subtotal << endl;
    cout << "Shipping " << fixed << setprecision(0) << shippingFee << endl;
    cout << "Amount " << fixed << setprecision(0) << totalAmount << endl;

    customer.deduct(totalAmount);
    cout << "Customer balance after payment: " << fixed << setprecision(0) << customer.getBalance() << endl;

    cart.clear();
}

int main() {
    try {
        // Only define products that are actually added to cart
        Product cheese("Cheese", 5.0, 10, "2026-01-01");
        cheese.is_shippable = true;
        cheese.weight = 0.2;

        Product biscuits("Biscuits", 3.0, 20, "2025-05-01"); // Expired
        biscuits.is_shippable = true;
        biscuits.weight = 0.7;

        Product tv("TV", 500, 3);
        tv.is_shippable = true;
        tv.weight = 7.0;

        Product scratchCard("Mobile Scratch Card", 10, 100);
        Product eBookVoucher("E-Book Voucher", 8, 200);
        Product courseCode("Online Course Access Code", 50, 80);

        // Create customer
        Customer customer("Ahmed", 1500);

        // Add items to cart
        Cart cart;
        cart.add(&cheese, 2);
        cart.add(&biscuits, 1); 
        cart.add(&tv, 1); 
        cart.add(&scratchCard, 1);
        cart.add(&eBookVoucher, 1);
        cart.add(&courseCode, 1);

        // Checkout
        checkout(customer, cart);

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
