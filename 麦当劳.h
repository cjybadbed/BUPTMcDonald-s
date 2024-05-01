#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<array>
#include<algorithm>
#include<map>
#include<fstream>
#include<cstdlib>
using namespace std;
class Food {
public:
	string name;
	int worktime, capacity, storage;
	Food(const string& n) :name(n){}

};
class Combo {
public:
	string name;
	vector<Food>component;
	Combo(const string& n):name(n){}
};
class Order {
public:
	int seconds;//times elapsed since00:00:00
	Combo request;
};
class Setup {
public:
	map<string, Combo>ComboLUT;
	map<string, Food>FoodLUT;
};

Setup init() {
	Setup cfg;
	int typeOfFood, typeOfCombo;
	ifstream menu("dict.dic");
	string line;
	int lineNum = 0;
	while (getline(menu, line)) {
		lineNum++;
		if (lineNum == 1) {//read first line in dict.dic
			istringstream iss(line);
			iss >> typeOfFood >> typeofcombo;
		}
		else if (lineNum == 2) {//read second line input foods
			istringstream iss(line);
			string foodName;
			while (iss >> foodName) {
				cfg.FoodLUT[foodName] = Food(foodName);
			}
		}
		else {//following lines input combo information
			istringstream iss(line);
			string comboName,comp;
			iss >> comboName;
			cfg.ComboLUT[comboName] = Combo(comboName);
			while (iss >> comp) {
				cfg.ComboLUT[comboName].component.push_back(comp);
			}
		}
	}

}
