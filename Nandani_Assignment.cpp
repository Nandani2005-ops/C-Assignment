#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <memory>

using namespace std;

// Abstract class Advisory with a pure virtual function
class Advisory {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisory() = default;
};

// Base class Crop inheriting from Advisory
class Crop : public Advisory {
protected:
    string name;
    double area;          // in acres
    string season;
    double yieldPerAcre;  // tons per acre

    // Static member to track the number of active Crop objects
    static int cropCount;

public:
    // Default constructor
    Crop() : name("Unknown"), area(0.0), season("Unknown"), yieldPerAcre(0.0) {
        ++cropCount;
    }

    // Parameterized constructor with validation
    Crop(const string& n, double a, const string& s, double y)
        : name(n), area(a), season(s), yieldPerAcre(y) {
        if (a <= 0) {
            throw invalid_argument("Area must be greater than 0 acres!");
        }
        if (y < 0) {
            throw invalid_argument("Yield per acre cannot be negative!");
        }
        ++cropCount;
    }

    // Prevent copying so the static object count remains accurate
    Crop(const Crop&) = delete;
    Crop& operator=(const Crop&) = delete;

    virtual ~Crop() {
        --cropCount;
    }

    // Getters
    string getName() const { return name; }
    double getArea() const { return area; }
    string getSeason() const { return season; }
    double getYieldPerAcre() const { return yieldPerAcre; }

    static int getCropCount() {
        return cropCount;
    }

    // Runtime polymorphism: calculate total yield
    virtual double calculateTotalYield() const {
        return area * yieldPerAcre;
    }

    virtual void display() const {
        cout << left << setw(15) << name
             << setw(10) << area
             << setw(12) << season
             << setw(15) << yieldPerAcre
             << setw(15) << calculateTotalYield() << '\n';
    }

    // Operator overloading: compare crops by total yield
    bool operator>(const Crop& other) const {
        return calculateTotalYield() > other.calculateTotalYield();
    }

    bool operator<(const Crop& other) const {
        return calculateTotalYield() < other.calculateTotalYield();
    }

    // Save common crop data to a file
    virtual void saveToFile(ofstream& outFile) const {
        outFile << quoted(name) << ' ' << area << ' '
                << quoted(season) << ' ' << yieldPerAcre << ' ';
    }
};

int Crop::cropCount = 0;

// Derived class 1: CerealCrop
class CerealCrop : public Crop {
private:
    string fertilizerType;

public:
    CerealCrop() : Crop(), fertilizerType("Standard") {}

    CerealCrop(const string& n, double a, const string& s, double y,
               const string& f)
        : Crop(n, a, s, y), fertilizerType(f) {}

    void giveAdvice() const override {
        cout << "[Advisory - Cereal Crop (" << name << ")]: Apply "
             << fertilizerType
             << " fertilizer and maintain optimal field drainage during "
             << season << ".\n";
    }

    void display() const override {
        cout << left << setw(15) << "[Cereal]";
        Crop::display();
        cout << "                  -> Fertilizer Type: "
             << fertilizerType << '\n';
    }

    void saveToFile(ofstream& outFile) const override {
        outFile << "CEREAL ";
        Crop::saveToFile(outFile);
        outFile << quoted(fertilizerType) << '\n';
    }
};

// Derived class 2: VegetableCrop
class VegetableCrop : public Crop {
private:
    string irrigationFrequency;

public:
    VegetableCrop() : Crop(), irrigationFrequency("Daily") {}

    VegetableCrop(const string& n, double a, const string& s, double y,
                  const string& i)
        : Crop(n, a, s, y), irrigationFrequency(i) {}

    void giveAdvice() const override {
        cout << "[Advisory - Vegetable Crop (" << name
             << ")]: Set irrigation frequency to "
             << irrigationFrequency
             << " to prevent root damage.\n";
    }

    void display() const override {
        cout << left << setw(15) << "[Vegetable]";
        Crop::display();
        cout << "                  -> Irrigation Frequency: "
             << irrigationFrequency << '\n';
    }

    void saveToFile(ofstream& outFile) const override {
        outFile << "VEGETABLE ";
        Crop::saveToFile(outFile);
        outFile << quoted(irrigationFrequency) << '\n';
    }
};

// Save crop data to a file
void saveCropData(const vector<unique_ptr<Crop>>& crops,
                  const string& filename) {
    ofstream outFile(filename);

    if (!outFile) {
        cerr << "Error: Could not open file for writing!\n";
        return;
    }

    for (const auto& crop : crops) {
        crop->saveToFile(outFile);
    }

    cout << "\n[File Handling]: Crop data successfully saved to "
         << filename << '\n';
}

// Load and display crop data from a file
void loadCropData(const string& filename) {
    ifstream inFile(filename);

    if (!inFile) {
        cerr << "Error: Could not open file for reading!\n";
        return;
    }

    cout << "\n--- Retrieved Crop Data from File ---\n";

    string type, name, season, extra;
    double area, yieldPerAcre;

    while (inFile >> type >> quoted(name) >> area >> quoted(season)
                  >> yieldPerAcre >> quoted(extra)) {
        if (type == "CEREAL") {
            cout << "Cereal Crop -> Name: " << name
                 << ", Area: " << area
                 << " acres, Season: " << season
                 << ", Yield/Acre: " << yieldPerAcre
                 << ", Fertilizer: " << extra << '\n';
        } else if (type == "VEGETABLE") {
            cout << "Vegetable Crop -> Name: " << name
                 << ", Area: " << area
                 << " acres, Season: " << season
                 << ", Yield/Acre: " << yieldPerAcre
                 << ", Irrigation: " << extra << '\n';
        }
    }
}

int main() {
    // unique_ptr automatically deletes crops when the vector is destroyed
    vector<unique_ptr<Crop>> farmCrops;

    cout << "=========================================================\n";
    cout << "          SMART FARM MANAGEMENT SYSTEM                   \n";
    cout << "=========================================================\n";

    // Demonstration of exception handling
    try {
        cout << "\nAttempting to add valid and invalid crops...\n";

        farmCrops.push_back(make_unique<CerealCrop>(
            "Wheat", 50.5, "Rabi", 3.2, "Nitrogen-rich"));

        farmCrops.push_back(make_unique<VegetableCrop>(
            "Tomato", 15.0, "Kharif", 8.5, "Alternate Days"));

        farmCrops.push_back(make_unique<CerealCrop>(
            "Rice", 40.0, "Kharif", 4.0, "Phosphorus"));

        // Uncomment to test exception handling:
        // farmCrops.push_back(make_unique<VegetableCrop>(
        //     "Potato", -5.0, "Rabi", 6.0, "Daily"));

    } catch (const exception& e) {
        cout << "[Exception Caught] " << e.what() << '\n';
    }

    cout << "\nTotal active crops managed: "
         << Crop::getCropCount() << "\n\n";

    // Runtime polymorphism: display crop details
    cout << left << setw(15) << "Type"
         << setw(15) << "Name"
         << setw(10) << "Area"
         << setw(12) << "Season"
         << setw(15) << "Yield/Acre"
         << setw(15) << "Total Yield" << '\n';
    cout << string(82, '-') << '\n';

    for (const auto& crop : farmCrops) {
        crop->display();
    }

    // Display farming advice
    cout << "\n--- Farming Advisories ---\n";
    for (const auto& crop : farmCrops) {
        crop->giveAdvice();
    }

    // Operator overloading demonstration
    cout << "\n--- Yield Comparison (Operator Overloading) ---\n";

    if (farmCrops.size() >= 2) {
        if (*farmCrops[0] > *farmCrops[1]) {
            cout << farmCrops[0]->getName()
                 << " has a higher total yield than "
                 << farmCrops[1]->getName() << ".\n";
        } else {
            cout << farmCrops[0]->getName()
                 << " has a lower or equal total yield compared to "
                 << farmCrops[1]->getName() << ".\n";
        }
    } else {
        cout << "At least two crops are required for comparison.\n";
    }

    // File handling demonstration
    const string filename = "farm_data.txt";
    saveCropData(farmCrops, filename);
    loadCropData(filename);

    // No manual delete is required; unique_ptr handles memory cleanup.
    return 0;
}
