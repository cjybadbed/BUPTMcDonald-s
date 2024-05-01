#pragma once
#include<string>
#include<iomanip>
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
	int num;
	int type;//0 for single 1 for combo
};

class Setup {
public:
	vector<Food>FoodLUT;
	vector<Combo>ComboLUT;
	int totalOrder, maxJunctionOrder, minJunctionOrder;
	int typeOfFood, typeOfCombo;
	vector<Order>OrderList;
	int mapFood(const string& input)const {
		int cur = 0;
		for (const Food& f : FoodLUT) {
			if (input == f.name)return cur;
			cur++;
		}
		return -1;
	}
};

class Working {
public:
	static vector<Combo> ComboLUT;
	vector<int>requirements;
	int time;
	static int foodType;
	Working(const Order& d){
		time = d.seconds;
		requirements.resize(foodType);
		if (d.type) {
			for (int t : ComboLUT[d.num].component)requirements[t] = 1;
		}
		else {
			requirements[d.num] = 1;
		}
	}
};

Order orderInterpreter(const string& line, const vector<Combo>& ComboLUT, const vector<Food>& FoodLUT) {
	istringstream iss(line);
	int time;
	string content;
	iss >> time >> content;
	Order neworder;
	neworder.seconds = time;
	int num = 0;
	for (const auto& a : ComboLUT) {
		if (content == a.name) {
			neworder.type = 1;
			neworder.num = num;
			return neworder;
		}
		num++;
	}
	num = 0;
	for (const auto& a : FoodLUT) {
		if (content == a.name) {
			neworder.type = 0;
			neworder.num = num;
			return neworder;
		}
		num++;
	}
	return neworder;
}

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
				tmp.push_back(cfg.mapFood(comp));
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
		else {
			cfg.OrderList.push_back(orderInterpreter(line, cfg.ComboLUT, cfg.FoodLUT));
		}
		
	}
	menu.close();
	operation.close();
	return cfg;
}


int mapCombo(const string& input, const vector<Combo>& ComboLUT) {
	int cur = 0;
	for (const Combo& f : ComboLUT) {
		if (input == f.name)return cur;
		cur++;
	}
	return -1;
}
int TimeProcess(const string& T) {
	return ((int)T[0]) * 36000 + ((int)T[1]) * 3600 + ((int)T[3]) * 600 + ((int)T[4]) * 60 + ((int)T[5]) * 10 + (int)T[6];
}
string convertTime(int t) {//
	if (t == -1) {
		return "Fail";
	}
	int hour, minute, second;
	hour = t / 3600;
	t %= 3600;
	minute = t / 60;
	t %= 60;
	second = t;
	ostringstream oss;
	oss << setfill('0') << setw(2) << hour << ':' << setw(2) << minute << ':' << second;
	return oss.str();
}
void Cookupdate(vector<Food>&foodindex, int tick) {
	static vector<int>CookingStatus;
	CookingStatus.resize(foodindex.size());
	for (int i = 0; i < foodindex.size(); i++) {
		if (tick == 25199) {
			CookingStatus[i] = foodindex[i].worktime;
			foodindex[i].storage = 0;
		}
		if(foodindex[i].storage<foodindex[i].capacity&&CookingStatus[i]==0)CookingStatus[i] = foodindex[i].worktime;
		if (CookingStatus[i] == 1)foodindex[i].storage++;
		CookingStatus[i]--;
	}
}
void acceptor(bool en,int time,const vector<Order>&all,vector<Working>&jobs,vector<int>&finish){
	static int orderSeq = 0;
	if (time = all[orderSeq].seconds) {
		if (!en) {
		finish[orderSeq] = -1;
		orderSeq++;
		return;
		}
		else {
			jobs.push_back(Working(all[orderSeq]));
			orderSeq++;
			return;
		}
	}
}
vector<int> cater() {

}