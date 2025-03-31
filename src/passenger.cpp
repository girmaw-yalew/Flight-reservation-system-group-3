#include <iostream>
#include <string>
#include <fstream>

using namespace std;

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
    is.ignore(1000, '\n'); // Ignore the newline after seat number
    is >> p.priority;
    is.ignore(1000, '\n'); // Ignore the newline after priority
    return is;
}

// Function to collect passenger information from the user
Passenger getPassengerInfo() {
    Passenger p;
    cout << "Enter passenger name: ";
    getline(cin, p.name);
    
    cout << "Enter seat number: ";
    while (!(cin >> p.seatNumber)) {
        cout << "Invalid input. Please enter a valid seat number: ";
        cin.clear(); // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
    }

    cout << "Enter priority (1 for high, 2 for medium, 3 for low): ";
    while (!(cin >> p.priority) || (p.priority < 1 || p.priority > 3)) {
        cout << "Invalid input. Please enter a priority (1-3): ";
        cin.clear(); // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore remaining newline
    return p;
}

// Function to save passenger information to a file
void savePassengerToFile(const Passenger& p, const string& filename) {
    ofstream ofs(filename, ios::app); // Open file in append mode
    if (!ofs) {
        cerr << "Error opening file for writing." << endl;
        return;
    }
    ofs << p; // Use overloaded operator to write passenger info
}

// Function to load and display passenger information from a file
void loadPassengersFromFile(const string& filename) {
    ifstream ifs(filename);
    if (!ifs) {
        cerr << "Error opening file for reading." << endl;
        return;
    }
    
    Passenger p;
    while (ifs >> p) { // Read passengers until end of file
        cout << "Name: " << p.name << ", Seat Number: " << p.seatNumber 
             << ", Priority: " << p.priority << endl;
    }
}

int main() {
    string filename = "passengers.txt";
    
    char choice;
    do {
        cout << "1. Add Passenger\n2. View Passengers\n3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore remaining newline
        
        switch (choice) {
            case '1': {
                Passenger p = getPassengerInfo();
                savePassengerToFile(p, filename);
                cout << "Passenger added successfully!" << endl;
                break;
            }
            case '2':
                loadPassengersFromFile(filename);
                break;
            case '3':
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
        
    } while (choice != '3');

    return 0;
}
