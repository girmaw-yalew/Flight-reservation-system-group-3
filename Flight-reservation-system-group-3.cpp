#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

const int TOTAL_SEATS = 10;

struct Passenger {
    string name;
    int seatNumber;
    int priority;
};

struct Node {
    Passenger data;
    Node* next;
};

class Queue {
private:
    struct QNode {
        Passenger data;
        QNode* next;
    } *front, *rear;
public:
    Queue() : front(nullptr), rear(nullptr) {}
    ~Queue() {
        while (!isEmpty()) dequeue();
    }
    void enqueue(Passenger p) {
        QNode* temp = new QNode{p, nullptr};
        if (!rear) front = rear = temp;
        else {
            rear->next = temp;
            rear = temp;
        }
    }
    Passenger dequeue() {
        if (isEmpty()) return {"", -1, 0};
        QNode* temp = front;
        Passenger p = temp->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        return p;
    }
    bool isEmpty() { return !front; }
};

class Stack {
private:
    struct SNode {
        Passenger data;
        SNode* next;
    } *top;
public:
    Stack() : top(nullptr) {}
    ~Stack() {
        while (!isEmpty()) pop();
    }
    void push(Passenger p) {
        SNode* temp = new SNode{p, top};
        top = temp;
    }
    Passenger pop() {
        if (isEmpty()) return {"", -1, 0};
        SNode* temp = top;
        Passenger p = temp->data;
        top = top->next;
        delete temp;
        return p;
    }
    bool isEmpty() { return !top; }
};

class Waitlist {
private:
    Queue waitQueue;
    Stack waitStack;
public:
    void addToWaitlist(string name, int priority) {
        Passenger newPassenger = {name, -1, priority};
        waitQueue.enqueue(newPassenger);
        waitStack.push(newPassenger);
        cout << name << " added to the waitlist with priority " << priority << endl;
    }
    Passenger getNextWaitlist() {
        return waitQueue.dequeue();
    }
    bool isEmpty() { return waitQueue.isEmpty(); }
};

class AirlineSystem {
private:
    Passenger seats[TOTAL_SEATS];
    Waitlist waitlist;
public:
    AirlineSystem() {
        for (int i = 0; i < TOTAL_SEATS; i++) {
            seats[i] = {"", -1, 0};
        }
    }
    
    void reserveSeat(string name, int priority = 0) {
        for (int i = 0; i < TOTAL_SEATS; i++) {
            if (seats[i].seatNumber == -1) {
                seats[i] = {name, i + 1, priority};
                cout << "Seat booked successfully! Seat Number: " << i + 1 << endl;
                return;
            }
        }
        cout << "No seats available. Adding to waitlist..." << endl;
        waitlist.addToWaitlist(name, priority);
    }

    void cancelSeat(int seatNumber) {
        if (seatNumber < 1 || seatNumber > TOTAL_SEATS || seats[seatNumber - 1].seatNumber == -1) {
            cout << "Invalid or unbooked seat." << endl;
            return;
        }
        cout << "Seat " << seatNumber << " canceled for " << seats[seatNumber - 1].name << endl;
        seats[seatNumber - 1] = {"", -1, 0};
        if (!waitlist.isEmpty()) {
            Passenger next = waitlist.getNextWaitlist();
            seats[seatNumber - 1] = {next.name, seatNumber, next.priority};
            cout << "Assigning waitlisted passenger " << next.name << " to seat " << seatNumber << endl;
        }
    }

    void displaySeats() {
        cout << "\nSeat Reservations:" << endl;
        for (int i = 0; i < TOTAL_SEATS; i++) {
            cout << "Seat " << i + 1 << ": "
                 << (seats[i].seatNumber == -1 ? "Available" : seats[i].name)
                 << " (Priority: " << seats[i].priority << ")" << endl;
        }
    }
};

int main() {
    AirlineSystem airline;
    int choice, seatNumber, priority;
    string name;
    do {
        cout << "\nAirline Reservation System\n";
        cout << "1. Reserve Seat\n2. Cancel Seat\n3. Display Seats\n4. Exit\nEnter choice: ";
        cin >> choice;
        cin.ignore();
        if (choice == 1) {
            cout << "Enter passenger name: "; getline(cin, name);
            cout << "Enter priority (0 for normal): "; cin >> priority;
            airline.reserveSeat(name, priority);
        } else if (choice == 2) {
            cout << "Enter seat number to cancel: "; cin >> seatNumber;
            airline.cancelSeat(seatNumber);
        } else if (choice == 3) {
            airline.displaySeats();
        }
    } while (choice != 4);
    return 0;
}
