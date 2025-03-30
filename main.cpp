#include <iostream>
#include <string>
#include <fstream>
// Added for dynamic waitlist

using namespace std;

const int TOTAL_SEATS = 150;
const string RESERVATION_FILE = "reservations.dat";
const string WAITLIST_FILE = "waitlist.dat";

// Structure to hold flight information
struct Flight {
    string date;
};

// Structure to hold passenger information
struct Passenger {
    string name;
    int seatNumber;
    int priority;
};

// Overload the output stream operator for writing to file
ofstream& operator<<(ofstream& os, const Passenger& p) {
    os << p.name << endl;
    os << p.seatNumber << endl;
    os << p.priority << endl;
    return os;
}

// Overload the input stream operator for reading from file
ifstream& operator>>(ifstream& is, Passenger& p) {
    getline(is, p.name);
    is >> p.seatNumber;
    is.ignore(1000, '\n');
    is >> p.priority;
    is.ignore(1000, '\n');
    return is;
}

// Class to manage the waitlist using a linked list
class Waitlist {
private:
    struct Node {
        Passenger data;
        Node* next;
    } *front, *rear;
public:
    Waitlist();
    ~Waitlist();
    void addToWaitlist(const string& name, int priority);
    Passenger getNextWaitlist();
    bool isEmpty() const;
    void loadWaitlist();
    void saveWaitlist() const;
};

Waitlist::Waitlist() : front(nullptr), rear(nullptr) {}

Waitlist::~Waitlist() {
    Node* current = front;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    front = rear = nullptr;
}

void Waitlist::addToWaitlist(const string& name, int priority) {
    Node* newNode = new Node{ {name, -1, priority}, nullptr };
    if (!rear) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    cout << name << " added to the waitlist with priority " << priority << endl;
}

Passenger Waitlist::getNextWaitlist() {
    if (!front) {
        return {"", -1, 0};
    }
    Node* temp = front;
    Passenger passenger = temp->data;
    front = front->next;
    if (!front) {
        rear = nullptr;
    }
    delete temp;
    return passenger;
}

bool Waitlist::isEmpty() const {
    return !front;
}

void Waitlist::loadWaitlist() {
    ifstream file(WAITLIST_FILE, ios::binary);
    if (file.is_open()) {
        while (file.peek() != EOF) {
            Passenger p;
            file >> p; // Use the overloaded operator>>
            Node* newNode = new Node{{p.name, p.seatNumber, p.priority}, nullptr};
            if (!rear) {
                front = rear = newNode;
            } else {
                rear->next = newNode;
                rear = newNode;
            }
        }
        file.close();
    }
}

void Waitlist::saveWaitlist() const {
    ofstream file(WAITLIST_FILE, ios::binary);
    if (file.is_open()) {
        Node* current = front;
        while (current) {
            file << current->data; // Use the overloaded operator<<
            current = current->next;
        }
        file.close();
    }
}

// Function to load reservations from file
void loadReservations(Passenger seats[]) {
    ifstream file(RESERVATION_FILE, ios::binary);
    if (file.is_open()) {
        for (int i = 0; i < TOTAL_SEATS; ++i) {
            file >> seats[i];
        }
        file.close();
    }
}

// Function to save reservations to file
void saveReservations(const Passenger seats[]) {
    ofstream file(RESERVATION_FILE, ios::binary);
    if (file.is_open()) {
        for (int i = 0; i < TOTAL_SEATS; ++i) {
            file << seats[i];
        }
        file.close();
    }
}
// Class to manage the airline system
class AirlineSystem {
private:
    Passenger seats[150];
    Waitlist waitlist;
    Flight currentFlight;
    //void loadReservations();
    //void saveReservations() const;
    void assignFromWaitlist(int seatNumber);
    bool isFull() const;
public:
    AirlineSystem();
    ~AirlineSystem();
    void reserveSeat(const string& name, int priority = 0);
    void postponeFlight(const string& newDate);
    void displaySeats() const;
    bool canSeatPassenger() const;
    void displayCapacity() const;
    const Flight& getCurrentFlight() const { return currentFlight; }
};

AirlineSystem::AirlineSystem() : currentFlight({"Not Scheduled"}) {
    for (int i = 0; i < 150; i++) {
        seats[i] = {"", -1, 0};
    }
    loadReservations(seats);
    waitlist.loadWaitlist();
    cout << "Current Flight Date: " << currentFlight.date << endl;
}

AirlineSystem::~AirlineSystem() {
    saveReservations(seats);
    waitlist.saveWaitlist();
}



bool AirlineSystem::isFull() const {
    for (int i = 0; i < 150; ++i) {
        if (seats[i].seatNumber == -1) {
            return false;
        }
    }
    return true;
}

bool AirlineSystem::canSeatPassenger() const {
    return !isFull();
}

void AirlineSystem::reserveSeat(const string& name, int priority) {
    if (!canSeatPassenger()) {
        cout << "Flight is full for " << currentFlight.date << ". Cannot book a seat at this time." << endl;
        waitlist.addToWaitlist(name, priority);
        return;
    }
    for (int i = 0; i < 150; i++) {
        if (seats[i].seatNumber == -1) {
            seats[i] = {name, i + 1, priority};
            cout << "Seat booked successfully for Flight on " << currentFlight.date << ", Seat Number: " << i + 1 << endl;
            return;
        }
    }
    cout << "Error: No available seats found despite capacity check." << endl;
    waitlist.addToWaitlist(name, priority);
}

void AirlineSystem::assignFromWaitlist(int seatNumber) {
    if (!waitlist.isEmpty()) {
        Passenger next = waitlist.getNextWaitlist();
        if (next.name != "") {
            seats[seatNumber - 1] = {next.name, seatNumber, next.priority};
            cout << "Assigning waitlisted passenger " << next.name << " to seat " << seatNumber << " for Flight on " << currentFlight.date << endl;
        }
    }
}

void AirlineSystem::postponeFlight(const string& newDate) {
    cout << "Flight on " << currentFlight.date << " has been postponed to " << newDate << "." << endl;
    currentFlight.date = newDate;
}

void AirlineSystem::displaySeats() const {
    cout << "\nSeat Reservations for Flight on " << currentFlight.date << ":" << endl;
    for (int i = 0; i < 150; i++) {
        cout << "Seat " << i + 1 << ": "
            << (seats[i].seatNumber == -1 ? "Available" : seats[i].name)
            << " (Priority: " << seats[i].priority << ")" << endl;
    }
}

void AirlineSystem::displayCapacity() const {
    int reservedSeats = 0;
    for (int i = 0; i < 150; ++i) {
        if (seats[i].seatNumber != -1) {
            reservedSeats++;
        }
    }
    int availableSeats = 150 - reservedSeats;
    cout << "\nFlight Capacity for " << currentFlight.date << ":" << endl;
    cout << "Total Seats: " << 150 << endl;
    cout << "Reserved Seats: " << reservedSeats << endl;
    cout << "Available Seats: " << availableSeats << endl;
}

int main() {
    AirlineSystem airline;
    int choice, priority;
    string name, newFlightDate;

    do {
        cout << "\nAirline Reservation System\n";
        cout << "Current Flight Date: " << airline.getCurrentFlight().date << endl;
        cout << "1. Reserve Seat\n2. Postpone Flight\n3. Display Seats\n4. Check Flight Capacity\n5. Display Reserved and Available Count\n6. Exit\nEnter choice: ";
        if (cin >> choice) {
            cin.ignore(1000, '\n');

            if (choice == 1) {
                cout << "Enter passenger name: ";
                getline(cin, name);
                cout << "Enter priority (0 for normal): ";
                if (cin >> priority) {
                    cin.ignore(1000, '\n');
                    airline.reserveSeat(name, priority);
                } else {
                    cout << "Invalid priority. Assuming normal priority (0)." << endl;
                    cin.clear();
                    cin.ignore(1000, '\n');
                    airline.reserveSeat(name, 0);
                }
            } else if (choice == 2) {
                cout << "Enter the new date for the flight (e.g.,YYYY-MM-DD): ";
                getline(cin, newFlightDate);
                airline.postponeFlight(newFlightDate);
            } else if (choice == 3) {
                airline.displaySeats();
            } else if (choice == 4) {
                if (airline.canSeatPassenger()) {
                    cout << "There are available seats on the flight scheduled for " << airline.getCurrentFlight().date << "." << endl;
                } else {
                    cout << "The flight on " << airline.getCurrentFlight().date << " is currently full." << endl;
                }
            } else if (choice == 5) {
                airline.displayCapacity();
            } else if (choice == 6) {
                cout << "Exiting system." << endl;
                break;
            } else {
                cout << "Invalid choice. Please enter a number between 1 and 6." << endl;
            }
        } else {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
        }
    } while (true);
    return 0;
}


