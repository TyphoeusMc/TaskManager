#ifndef READINFO_H
#define READINFO_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
using namespace std;
vector<string> ReadInfo(ifstream &info, vector<int> line, bool notail);
map<string,string> ReadInfobyName(ifstream &info, vector<string> line);
unsigned long str2uns(std::string str);
#endif
