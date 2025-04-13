#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

struct TestCase {
    std::string id;
    std::string baseStationId;
    std::string location;
    std::string operatorName;
    std::string plmn;
    std::string tac;
    std::string band;
    std::string bandwidth;
    std::string transmissionPower;
    std::string frequency;
    std::string cellId;
    std::string cellName;
    std::string cellType;
    std::string ethernet;
    std::string ip;
};

std::string trim(const std::string& str) {
    const std::string whitespace = " \t\r\n";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

std::vector<TestCase> readTestCasesFromCSV(const std::string& filePath) {
    std::vector<TestCase> testCases;
    std::ifstream file(filePath);
    std::string line;

    std::getline(file, line); // Otsikon ohitus

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(ss, field, ',')) {  // CSV tiedoston rivin arvojen jäsennys
            fields.push_back(trim(field));
        }

        if (fields.size() < 15) {
            std::cerr << "Invalid row (less than 15 fields): " << line << "\n";
            continue;
        }

        TestCase tc;
        tc.id = fields[0];
        tc.baseStationId = fields[1];
        tc.location = fields[2];
        tc.operatorName = fields[3];
        tc.plmn = fields[4];
        tc.tac = fields[5];
        tc.band = fields[6];
        tc.bandwidth = fields[7];
        tc.transmissionPower = fields[8];
        tc.frequency = fields[9];
        tc.cellId = fields[10];
        tc.cellName = fields[11];
        tc.cellType = fields[12];
        tc.ethernet = fields[13];
        tc.ip = fields[14];

        std::cout << "DEBUG: Read BaseStationID from CSV: [" << tc.baseStationId << "]\n";
        testCases.push_back(tc);
    }

    return testCases;
}

std::string findValue(const std::string& xml, const std::string& tag) {
    std::string open = "<" + tag + ">";
    std::string close = "</" + tag + ">";
    size_t start = xml.find(open);
    size_t end = xml.find(close);
    if (start == std::string::npos || end == std::string::npos) return "";
    start += open.size();
    return xml.substr(start, end - start);
}

void replaceValue(std::string& xml, const std::string& tag, const std::string& newValue) {
    std::string open = "<" + tag + ">";
    std::string close = "</" + tag + ">";
    size_t start = xml.find(open);
    size_t end = xml.find(close);
    if (start == std::string::npos || end == std::string::npos) return;
    start += open.size();
    xml.replace(start, end - start, newValue);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: ./generateXMLfromCSV <testcase.csv> <commissioning-file.xml>\n";
        return 1;
    }

    std::string csvPath = argv[1];
    std::string xmlPath = argv[2];

    std::vector<TestCase> testCases = readTestCasesFromCSV(csvPath);

    std::ifstream xmlFile(xmlPath);
    if (!xmlFile) {
        std::cerr << "Error: Could not open XML file " << xmlPath << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << xmlFile.rdbuf();
    std::string xmlContent = buffer.str();
    xmlFile.close();

    std::string xmlBaseStationId = trim(findValue(xmlContent, "BaseStationID"));
    std::cout << "DEBUG: BaseStationID from XML: [" << xmlBaseStationId << "]\n";

    for (const auto& tc : testCases) {
        if (tc.baseStationId == xmlBaseStationId) {
            replaceValue(xmlContent, "PLMN", tc.plmn);
            replaceValue(xmlContent, "TAC", tc.tac);
            replaceValue(xmlContent, "Bandwidth", tc.bandwidth);
            replaceValue(xmlContent, "TransmissionPower", tc.transmissionPower);
            replaceValue(xmlContent, "Frequency", tc.frequency);
            replaceValue(xmlContent, "CellID", tc.cellId);
            replaceValue(xmlContent, "CellName", tc.cellName);
            replaceValue(xmlContent, "CellType", tc.cellType);
            replaceValue(xmlContent, "Ethernet", tc.ethernet);
            replaceValue(xmlContent, "IP", tc.ip);

            std::ofstream outFile(xmlPath);
            if (!outFile) {
                std::cerr << "Error: Could not write to file " << xmlPath << "\n";
                return 1;
            }
            outFile << xmlContent;
            outFile.close();

            std::cout << "Updated " << xmlPath << " successfully.\n";
            return 0;
        }
    }

    std::cerr << "No matching BaseStationID found in CSV.\n";
    return 1;
}