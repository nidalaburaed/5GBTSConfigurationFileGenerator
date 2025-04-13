# Description

A script to generate 5G New Radio (vRAN) base station configuration file (XML) based on predefined testcase Excel sheet (CSV)

# How to use

Compile the script using the following command:

```bash
g++ generateXMLfromCSV.cpp -o generateXMLfromCSV .
```

Run the script using the following command:

```bash
./generateXMLfromCSV <testcase.csv> <commissioning-file.xml>
```

# Notes

Example CSV files and a template of an XML file in the repository

