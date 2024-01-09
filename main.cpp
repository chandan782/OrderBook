#include <iostream>
#include<map>

using namespace std;

struct new_order
{
    uint64_t order_id;
    char  side; 
    int32_t price;
    uint32_t quantity;
};
struct change_order
{
    uint64_t order_id;
    int32_t new_price;
    uint32_t new_quantity;
};

struct cancel_order
{
    uint64_t order_id;
};

class OrderBook
{
    map<uint64_t, new_order> ordersById; // order id to orders mapping
    map<int32_t, uint32_t> buyPrices; // buy order price to quantity mapping
    map<int32_t, uint32_t> sellPrices; // sell order price to quantity mapping

public:
    void OnNewOrder(const new_order &order) {
        ordersById[order.order_id] = order;

        // Aggregate volume if order at the same price exists
        if (order.side == 'B')
        {
            buyPrices[order.price] += order.quantity;
        }
        else if (order.side == 'S')
        {
            sellPrices[order.price] += order.quantity;
        }
    }

    void OnChangeOrder(const change_order &order){
        map<uint64_t, new_order>::iterator orderByIdIterator = ordersById.find(order.order_id);

        // Check whether change request with order_id exist or not
        if(orderByIdIterator != ordersById.end()) {
            if(orderByIdIterator->second.side == 'B') {
                buyPrices[orderByIdIterator->second.price] -= orderByIdIterator->second.quantity;
                if(buyPrices[orderByIdIterator->second.price] == 0) {
                    buyPrices.erase(buyPrices.find(orderByIdIterator->second.price));
                }
                buyPrices[order.new_price] += order.new_quantity;
            }
            else if(orderByIdIterator->second.side == 'S') {
                sellPrices[orderByIdIterator->second.price] -= orderByIdIterator->second.quantity;
                if(sellPrices[orderByIdIterator->second.price] == 0) {
                    sellPrices.erase(sellPrices.find(orderByIdIterator->second.price));
                }
                sellPrices[order.new_price] += order.new_quantity;
            }

            orderByIdIterator->second.price = order.new_price;
            orderByIdIterator->second.quantity = order.new_quantity;
        }
    }

    void OnCancelOrder(const cancel_order &order){
        map<uint64_t, new_order>::iterator orderByIdIterator = ordersById.find(order.order_id);

        // Check whether cancel request with order_id exist or not
        if(orderByIdIterator != ordersById.end()) {
            if(orderByIdIterator->second.side == 'B') {
                buyPrices[orderByIdIterator->second.price] -= orderByIdIterator->second.quantity;
                if(buyPrices[orderByIdIterator->second.price] == 0) {
                    buyPrices.erase(buyPrices.find(orderByIdIterator->second.price));
                }
            }
            else if(orderByIdIterator->second.side == 'S') {
                sellPrices[orderByIdIterator->second.price] -= orderByIdIterator->second.quantity;
                if(sellPrices[orderByIdIterator->second.price] == 0) {
                    sellPrices.erase(sellPrices.find(orderByIdIterator->second.price));
                }
            }

            ordersById.erase(orderByIdIterator);
        }
    }

    void Print(){
        // Print Buys
        cout << "Buys" << endl;
        cout << "Price" << "\t" << "Quantity" << endl;
        for(map<int32_t, uint32_t>::reverse_iterator it = buyPrices.rbegin(); it != buyPrices.rend(); ++it) {
            cout << it -> first << "\t" << it -> second << endl;
        }

        // Print Sells
        cout << "Sells" << endl;
        cout << "Price" << "\t" << "Quantity" << endl;
        for(map<int32_t, uint32_t>::iterator it = sellPrices.begin(); it != sellPrices.end(); ++it) {
            cout << it -> first << "\t" << it -> second << endl;
        }
    }
};



int main() {

    // Create orderbook instance
    OrderBook orderBook;

    // Add new orders
    new_order order1 = {1, 'B', 100, 10000};
    new_order order2 = {2, 'B', 99, 2000};
    new_order order3 = {3, 'B', 98, 1000};
    new_order order4 = {4, 'S', 102, 7000};
    new_order order5 = {5, 'S', 103, 2000};
    new_order order6 = {6, 'S', 104, 9000};

    orderBook.OnNewOrder(order1);
    orderBook.OnNewOrder(order2);
    orderBook.OnNewOrder(order3);
    orderBook.OnNewOrder(order4);
    orderBook.OnNewOrder(order5);
    orderBook.OnNewOrder(order6);

    cout << "Print all placed orders:" << endl;
    orderBook.Print();

    // Call change order
    change_order changeorder1 = {2, 100, 1999};
    orderBook.OnChangeOrder(changeorder1);

    cout << "Print requested change order response:" << endl;
    orderBook.Print();

    // Call cancel order
    cancel_order cancelorder1 = {3};
    orderBook.OnCancelOrder(cancelorder1);

    
    cout << "Print requested cancel order response:" << endl;
    orderBook.Print();

    return 0;
}