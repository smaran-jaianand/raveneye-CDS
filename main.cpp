#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>  // <--- this fixes numeric_limits error

using namespace std;

struct Connection {
    int targetId;
    string type;
    int weight;
};

struct Criminal {
    int id{};
    string name;
    string tag;
    int age{};
    string location;
    vector<Connection> connections;
};

class RavenEye {
private:
    vector<Criminal> records;
    unordered_map<int, vector<Connection>> network;
    int nextId = 1;

    unordered_map<string,int> weightMap = {
        {"family", 10}, {"partner", 9}, {"gang member", 8},
        {"associate", 5}, {"acquaintance", 4}, {"informant", 3}, {"rival", 2}
    };

    string toLower(const string& s) {
        string res = s;
        transform(res.begin(), res.end(), res.begin(), ::tolower);
        return res;
    }

    int getConnectionWeight(const string& type) {
        auto it = weightMap.find(toLower(type));
        return it != weightMap.end() ? it->second : 5;
    }

    string quote(const string& s) { return "\"" + s + "\""; }

    string escapeQuotes(const string& s) {
        string res;
        for (char c : s) {
            if (c == '"') res += "\"\"";
            else res += c;
        }
        return res;
    }

public:
    void addCriminal() {
        Criminal c;
        c.id = nextId++;
        cout << "\nEnter Name: ";
        cin.ignore();
        getline(cin, c.name);
        cout << "Enter Tag (e.g., Suspect / Gang Leader / Informant): ";
        getline(cin, c.tag);
        cout << "Enter Age: ";
        cin >> c.age;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Location: ";
        getline(cin, c.location);
        records.push_back(c);
        cout << "\n[+] Criminal added with ID: " << c.id << endl;
    }

    void addNonCriminal() {
        Criminal c;
        c.id = nextId++;
        cout << "\nEnter Name: ";
        cin.ignore();
        getline(cin, c.name);
        cout << "Enter Tag (e.g., Civilian, Govt, Confidential): ";
        getline(cin, c.tag);
        cout << "Enter Age: ";
        cin >> c.age;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter Location: ";
        getline(cin, c.location);
        records.push_back(c);
        cout << "\n[+] " << c.tag << " added with ID: " << c.id << endl;
    }

    void addConnection() {
        int id1, id2;
        string connectionType;

        cout << "\nEnter ID of first person: ";
        cin >> id1;
        cout << "Enter ID of second person: ";
        cin >> id2;
        if (id1 == id2) { cout << "[!] Cannot connect to self.\n"; return; }
        if (id1 <= 0 || id2 <= 0 || id1 > records.size() || id2 > records.size()) {
            cout << "[!] Invalid IDs.\n"; return;
        }

        cout << "\nConnection Types: Family, Partner, Gang Member, Associate, Acquaintance, Informant, Rival\n";
        cout << "Enter Connection Type: ";
        cin.ignore();
        getline(cin, connectionType);
        int weight = getConnectionWeight(connectionType);

        // Prevent duplicate connection
        for (auto &c : network[id1]) if (c.targetId == id2) {
            cout << "[!] Connection already exists.\n"; return;
        }

        Connection conn1 = {id2, connectionType, weight};
        Connection conn2 = {id1, connectionType, weight};
        network[id1].push_back(conn1);
        network[id2].push_back(conn2);
        records[id1 - 1].connections.push_back(conn1);
        records[id2 - 1].connections.push_back(conn2);

        cout << "\n[+] Connection established between " << records[id1 - 1].name
             << " and " << records[id2 - 1].name
             << " | Type: " << connectionType << " | Weight: " << weight << endl;
    }

    void displayCriminals() {
        cout << "\n=== All Records ===\n";
        for (const auto &c : records) {
            cout << "ID: " << c.id << " | Name: " << c.name
                 << " | Tag: " << c.tag
                 << " | Age: " << c.age
                 << " | Location: " << c.location << endl;
        }
    }

    void displayNetwork() {
        cout << "\n=== Connection Network ===\n";
        for (auto &n : network) {
            cout << "[" << records[n.first - 1].name << "] -> ";
            for (const auto& conn : n.second) {
                cout << records[conn.targetId - 1].name
                     << " (" << conn.type << ", W:" << conn.weight << "), ";
            }
            cout << "\n";
        }
    }

    void saveToFile() {
        ofstream file("RAVEN_EYE_DATABASE.csv");
        file << "ID,Name,Tag,Age,Location,Connections\n";

        for (auto &c : records) {
            file << c.id << ","
                 << quote(escapeQuotes(c.name)) << ","
                 << quote(escapeQuotes(c.tag)) << ","
                 << c.age << ","
                 << quote(escapeQuotes(c.location)) << ",\"[";

            for (size_t i = 0; i < c.connections.size(); ++i) {
                auto &conn = c.connections[i];
                file << conn.targetId << ":" << conn.type << ":" << conn.weight;
                if (i != c.connections.size() - 1) file << ";";
            }
            file << "]\"\n";
        }
        cout << "\n[+] Data saved to RAVEN_EYE_DATABASE.csv\n";
    }

    void loadFromFile() {
        ifstream file("RAVEN_EYE_DATABASE.csv");
        if (!file.is_open()) { cout << "[!] No file found.\n"; return; }

        records.clear(); network.clear(); nextId = 1;
        string line; getline(file, line); // skip header

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            Criminal c; string temp, connections;

            try {
                getline(ss, temp, ','); c.id = stoi(temp); nextId = max(nextId, c.id + 1);
                getline(ss, c.name, ','); getline(ss, c.tag, ',');
                getline(ss, temp, ','); c.age = stoi(temp);
                getline(ss, c.location, ','); getline(ss, connections);

                if (!connections.empty() && connections.front() == '"') {
                    connections = connections.substr(2, connections.size() - 4); // remove quotes + brackets
                    stringstream cs(connections); string connStr;
                    while (getline(cs, connStr, ';')) {
                        stringstream connStream(connStr);
                        string idStr, type, weightStr;
                        getline(connStream, idStr, ':'); getline(connStream, type, ':'); getline(connStream, weightStr, ':');
                        if (!idStr.empty() && !type.empty() && !weightStr.empty()) {
                            Connection conn{stoi(idStr), type, stoi(weightStr)};
                            c.connections.push_back(conn);
                            network[c.id].push_back(conn);
                        }
                    }
                }

                records.push_back(c);
            }
            catch (...) { cout << "[!] Skipped line: " << line << "\n"; }
        }

        cout << "\n[+] Data loaded. " << records.size() << " records.\n";
    }

    void run() {
        int choice;
        while (true) {
            cout << "\n=== RAVEN EYE SYSTEM ===\n";
            cout << "1. Add Criminal\n2. Add Non-Criminal\n3. Add Connection\n4. Display Records\n5. Display Network\n6. Save\n7. Load\n0. Exit\n";
            cout << "Choice: "; cin >> choice;
            switch(choice) {
                case 1: addCriminal(); break;
                case 2: addNonCriminal(); break;
                case 3: addConnection(); break;
                case 4: displayCriminals(); break;
                case 5: displayNetwork(); break;
                case 6: saveToFile(); break;
                case 7: loadFromFile(); break;
                case 0: cout << "Exiting...\n"; return;
                default: cout << "[!] Invalid choice.\n";
            }
        }
    }
};

int main() {
    RavenEye system;
    system.run();
    return 0;
}
