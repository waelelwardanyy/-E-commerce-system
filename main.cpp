#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;

// =============================
// Date Helper
// =============================
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

// =============================
// Product Class
// =============================
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

// =============================
// Customer Class
// =============================
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

// =============================
// Cart Classes
// =============================
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

// =============================
// Shipping Service
// =============================
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

// =============================
// Checkout Function
// =============================
void checkout(Customer& customer, Cart& cart);

// Handle expired item interaction
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
        checkout(customer, cart); // Recalculate
    } else if (choice == 2) {
        cout << "Available products:\n";
        cout << "1. Cheese\n";
        cout << "2. Biscuits\n";
        cout << "3. Yogurt\n";
        cout << "4. Milk Carton\n";
        cout << "5. Frozen Pizza\n";
        cout << "6. Fresh Juice\n";
        cout << "7. TV\n";
        cout << "8. Laptop\n";
        cout << "9. Headphones\n";
        cout << "10. Gaming Console\n";
        cout << "11. Microwave\n";
        cout << "12. Camera\n";
        cout << "13. Mobile Scratch Card\n";
        cout << "14. Netflix Subscription Code\n";
        cout << "15. E-Book Voucher\n";
        cout << "16. Google Play Gift Card\n";
        cout << "17. Steam Wallet Credit\n";
        cout << "18. Online Course Access Code\n";
        cout << "Select replacement product (1-18): ";
        int prodChoice;
        cin >> prodChoice;

        Product* newProduct = nullptr;

        // Define all products again inside scope
        Product cheese("Cheese", 5.0, 10, "2025-12-31");
        cheese.is_shippable = true;
        cheese.weight = 0.2;

        Product biscuits("Biscuits", 3.0, 20, "2025-12-31");
        biscuits.is_shippable = true;
        biscuits.weight = 0.7;

        Product yogurt("Yogurt", 2.0, 30, "2025-12-31");
        yogurt.is_shippable = true;
        yogurt.weight = 0.15;

        Product milkCarton("Milk Carton", 2.5, 15, "2025-12-31");
        milkCarton.is_shippable = true;
        milkCarton.weight = 1.0;

        Product frozenPizza("Frozen Pizza", 6.0, 8, "2025-12-31");
        frozenPizza.is_shippable = true;
        frozenPizza.weight = 0.5;

        Product freshJuice("Fresh Juice", 4.0, 12, "2025-12-31");
        freshJuice.is_shippable = true;
        freshJuice.weight = 1.2;

        Product tv("TV", 500, 3);
        tv.is_shippable = true;
        tv.weight = 7.0;

        Product laptop("Laptop", 1200, 5);
        laptop.is_shippable = true;
        laptop.weight = 2.5;

        Product headphones("Headphones", 100, 10);
        headphones.is_shippable = true;
        headphones.weight = 0.3;

        Product gamingConsole("Gaming Console", 400, 4);
        gamingConsole.is_shippable = true;
        gamingConsole.weight = 4.0;

        Product microwave("Microwave", 100, 2);
        microwave.is_shippable = true;
        microwave.weight = 6.5;

        Product camera("Camera", 300, 3);
        camera.is_shippable = true;
        camera.weight = 1.0;

        Product scratchCard("Mobile Scratch Card", 10, 100);

        Product netflixCode("Netflix Subscription Code", 15, 50);

        Product eBookVoucher("E-Book Voucher", 8, 200);

        Product googlePlayCard("Google Play Gift Card", 20, 150);

        Product steamCredit("Steam Wallet Credit", 25, 100);

        Product courseCode("Online Course Access Code", 50, 80);

        switch (prodChoice) {
            case 1: newProduct = &cheese; break;
            case 2: newProduct = &biscuits; break;
            case 3: newProduct = &yogurt; break;
            case 4: newProduct = &milkCarton; break;
            case 5: newProduct = &frozenPizza; break;
            case 6: newProduct = &freshJuice; break;
            case 7: newProduct = &tv; break;
            case 8: newProduct = &laptop; break;
            case 9: newProduct = &headphones; break;
            case 10: newProduct = &gamingConsole; break;
            case 11: newProduct = &microwave; break;
            case 12: newProduct = &camera; break;
            case 13: newProduct = &scratchCard; break;
            case 14: newProduct = &netflixCode; break;
            case 15: newProduct = &eBookVoucher; break;
            case 16: newProduct = &googlePlayCard; break;
            case 17: newProduct = &steamCredit; break;
            case 18: newProduct = &courseCode; break;
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

    // Print shipment notice
    if (!shippedList.empty()) {
        cout << "\n** Shipment notice **" << endl;
        for (auto& [name, w] : shippedList) {
            cout << "1x " << name << " " << fixed << setprecision(0) << w * 1000 << "g" << endl;
        }
        cout << "Total package weight " << fixed << setprecision(1) << (shippingFee / 10) << "kg" << endl;
    }

    // Print receipt
    cout << "\n** Checkout receipt **" << endl;
    for (auto item : cart.items) {
        cout << item.quantity << "x " << item.product->name << " "
             << fixed << setprecision(0) << item.product->price * item.quantity << endl;
    }

    cout << "----------------------" << endl;
    cout << "Subtotal " << fixed << setprecision(0) << subtotal << endl;
    cout << "Shipping " << fixed << setprecision(0) << shippingFee << endl;
    cout << "Amount " << fixed << setprecision(0) << totalAmount << endl;

    customer.deduct(totalAmount);
    cout << "Customer balance after payment: " << fixed << setprecision(0) << customer.getBalance() << endl;

    cart.clear();
}

// =============================
// Main Function
// =============================
int main() {
    try {
        // Create products manually like this
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

        // Create customer
        Customer customer("Ahmed", 1500); // Only $150 — insufficient for full purchase

        // Add to cart
        Cart cart;
        cart.add(&cheese, 2);
        cart.add(&biscuits, 1);
        cart.add(&tv, 1);
        cart.add(&scratchCard, 1);

        // Checkout
        checkout(customer, cart);

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}