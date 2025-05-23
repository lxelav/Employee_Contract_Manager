#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Contract {
public:
    string startDate, endDate, id;
    int cost;


    Contract(string& startDate, string& endDate, int cost, string& id) {
        this->startDate = startDate;
        this->endDate = endDate;
        this->cost = cost;
        this->id = id;
    }

    friend ostream& operator<<(ostream& os, const Contract& contract);

};

ostream& operator<<(ostream& os, const Contract& contract) {
    os << contract.id;

    return os;
}

class Record {
public:
    string currentFIO;
    vector<Contract> vectorContracts;

    Record(string currentFIO, vector<Contract>& vectorContracts) {
        this->currentFIO = currentFIO;
        this->vectorContracts = vectorContracts;
    }

    Record() : currentFIO(""), vectorContracts({}) {}

    friend ostream& operator<<(ostream& os, const Record& record);
};

ostream& operator<<(ostream& os, const Record& record) {
    os << "[ ";

    for (auto& value : record.vectorContracts) {
        os << value << " ";
    }

    os << "]";

    return os;
}


class Records {
public:
    map<string, Record> records;

    void readRecords(const string& filename) {
        ifstream file(filename);
        string line;

        string currentFIO;
        vector<Contract> vectorContracts;

        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            if (line[0] == '{') { vectorContracts.clear(); }
            else if (line[0] == '}') {
                records[currentFIO] = Record(currentFIO, vectorContracts);
            }
            else {
                size_t pos = line.find(' ');

                if (pos != string::npos) {
                    string key = line.substr(0, pos);

                    if (key != "Contract" && key != "{") {
                        currentFIO = line;
                    }
                    else if (key == "Contract") {
                        int cost;
                        string startDate, endDate, id;

                        size_t temp_pos = line.find(' ', pos + 1);
                        while (temp_pos != string::npos) {
                            string value = line.substr(pos + 1, temp_pos - pos - 1);

                            if (value[0] == '#') {
                                id = value;
                            }
                            else if (value == "start") {
                                pos = temp_pos;
                                temp_pos = line.find(' ', pos + 2);

                                startDate = line.substr(pos + 1, temp_pos - pos - 2);
                            }
                            else if (value == "end") {
                                pos = temp_pos;
                                temp_pos = line.find(' ', pos + 2);

                                endDate = line.substr(pos + 1, temp_pos - pos - 2);
                            }
                            else if (value == "Cost") {
                                cost = stoi(line.substr(temp_pos, line.find(';') - 1));
                            }
                            else {
                                pos = temp_pos;
                                temp_pos = line.find(' ', pos + 2);
                            }

                            pos = temp_pos;
                            temp_pos = line.find(' ', temp_pos + 1);
                            
                        }
                        Contract c(startDate, endDate, cost, id);
                        vectorContracts.push_back(c);
                    }
                }
            }
        }
    }

    int calculateTotalCost(const string& name) {
        if (records.count(name) == 0) {
            return 0; // Сотрудник не найден
        }

        int totalCost = 0;
        for (const auto& contract : records[name].vectorContracts) {
            totalCost += contract.cost;
        }
        return totalCost;
    }

    void listEmployeeContracts(const string& name) {
        if (records.count(name) == 0) {
            cout << "Сотрудник не найден." << endl;
            return;
        }

        const Record& employeeRecord = records[name];
        cout << "Договора сотрудника " << name << ":\n";
        for (const auto& contract : employeeRecord.vectorContracts) {
            cout << "Договор " << contract.id << " (с " << contract.startDate << " по " << contract.endDate << "): " << contract.cost << " рублей\n";
        }
    }

    void findLongestContract() {
        string longestContractID, FIO;
        int longestDuration = 0;


        for (const auto& employee : records) {
            for (const auto& contract : employee.second.vectorContracts) {
                int duration = calculateContractDuration(contract.startDate, contract.endDate);
                if (duration > longestDuration) {
                    longestDuration = duration;
                    longestContractID = contract.id;
                    FIO = employee.first;
                }
            }
        }

        if (!longestContractID.empty()) {
            cout << FIO << ": Самый продолжительный договор: " << longestContractID << " (продолжительность " << longestDuration << " дней)" << endl;
        }
        else {
            cout << "Договоры не найдены." << endl;
        }
    }

    void findMostExpensiveContract() {
        string mostExpensiveContractID, FIO;
        int maxCost = 0;

        for (const auto& employee : records) {
            for (const auto& contract : employee.second.vectorContracts) {
                if (contract.cost > maxCost) {
                    maxCost = contract.cost;
                    mostExpensiveContractID = contract.id;
                    FIO = employee.first;
                }
            }
        }

        if (!mostExpensiveContractID.empty()) {
            cout << FIO << ": Самый дорогой договор: " << mostExpensiveContractID << " (стоимость " << maxCost << " рублей)" << endl;
        }
        else {
            cout << "Договоры не найдены." << endl;
        }
    }

    void removeEmployee(const string& employeeName) {
        if (records.count(employeeName) > 0) {
            records.erase(employeeName);
            cout << "Информация о сотруднике " << employeeName << " удалена." << endl;
        }
        else {
            cout << "Сотрудник не найден." << endl;
        }
    }

    int calculateContractDuration(const string& startDate, const string& endDate) {
        //продолжительность считается как разница между датами в днях (предполагается, что даты имеют формат "dd.mm.yyyy").
     

        int startDay, startMonth, startYear;
        int endDay, endMonth, endYear;

        sscanf_s(startDate.c_str(), "%d.%d.%d", &startDay, &startMonth, &startYear);
        sscanf_s(endDate.c_str(), "%d.%d.%d", &endDay, &endMonth, &endYear);

        // Расчет продолжительности в днях.
        int duration = 0;

        // Рассчитываем разницу в годах.
        duration += (endYear - startYear) * 365;

        // Рассчитываем разницу в месяцах.
        duration += (endMonth - startMonth) * 30; // Предполагаем, что в месяце 30 дней.

        // Добавляем разницу в днях.
        duration += endDay - startDay;

        return duration;
    }
};



int main() {
    setlocale(0, "");


    Records r;
    r.readRecords("input_data.txt");

    for (auto& item : r.records) {
        cout << item.first << " : " << item.second << endl;
    }

    cout << endl;

    string employeeName = "Akakievich Clumsy Chlustin Neudachlivy";
    int totalCost = r.calculateTotalCost(employeeName);
    cout << "Общая стоимость договоров для " << employeeName << ": " << totalCost << " рублей" << endl;

    r.listEmployeeContracts(employeeName);

    r.findLongestContract();

    r.findMostExpensiveContract();

    string employeeToRemove = "Akakievich Clumsy Chlustin Neudachlivy";
    r.removeEmployee(employeeToRemove);

    return 0;
}
