#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<array>
#include<algorithm>
#include<fstream>
#include<cstdlib>
#include<sstream>
using namespace std;
class Food {
public:
	string name;
	int worktime, capacity, storage;
	Food(const string& n) :name(n), storage(0){}

};
class Combo {
public:
	string name;
	vector<int>component;
	Combo(const string& n):name(n){}
};
class Order {
public:
	int seconds;//times elapsed since00:00:00
	int comboNum;
};
class Setup {
public:
	vector<Food>FoodLUT;
	vector<Combo>ComboLUT;
	int totalOrder, maxJunctionOrder, minJunctionOrder;
	int typeOfFood, typeOfCombo;
	vector<Order>OrderList;
};

Setup init() {
	Setup cfg;
	ifstream menu("dict.dic");
	string line;
	int lineNum = 0;
	while (getline(menu, line)) {
		lineNum++;
		if (lineNum == 1) {//read first line in dict.dic
			istringstream iss(line);
			iss >> cfg.typeOfFood >> cfg.typeOfCombo;
		}
		else if (lineNum == 2) {//read second line input foods
			istringstream iss(line);
			string foodName;
			while (iss >> foodName) {
				cfg.FoodLUT.push_back(Food(foodName));
			}
		}
		else {//following lines input combo information
			istringstream iss(line);
			string comboName,comp;
			iss >> comboName;
			cfg.ComboLUT.push_back(Combo(comboName));////
			while (iss >> comp) {
				vector<int>& tmp = cfg.ComboLUT.back().component;
				tmp.push_back(mapFood(comp,cfg.FoodLUT));
				sort(tmp.begin(), tmp.end());
			}
		}
	}
	ifstream operation("input.txt");
	lineNum = 0;
	while (getline(operation, line)) {
		lineNum++;
		if (lineNum == 1) {
			istringstream iss(line);
			iss >> cfg.totalOrder;
		}
		else if (lineNum == 2) {
			istringstream iss(line);
			iss >> cfg.maxJunctionOrder >> cfg.minJunctionOrder;
		}
		else if (lineNum == 3) {
			istringstream iss(line);
			int t, cur = 0;
			while (iss >> t) {
				cfg.FoodLUT.at(cur).worktime = t;
				cur++;
			}
		}
		else if (lineNum == 4) {
			istringstream iss(line);
			int cp, cur = 0;
			while (iss >> cp) {
				cfg.FoodLUT.at(cur).capacity = cp;
				cur++;
			}
		}
		
	}



	return cfg;
}
Order orderInterpreter(const string& line) {

}
int mapFood(const string& input, const vector<Food>& FoodLUT) {
	int cur = 0;
	for (const Food& f : FoodLUT) {
		if (input == f.name)return cur;
		cur++;
	}
	return 0;
}