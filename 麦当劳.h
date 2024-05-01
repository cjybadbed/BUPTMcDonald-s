#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<array>
#include<algorithm>
using namespace std;
class Food {
public:
	string name;
	int worktime, capacity, storage;
	Food(int wt, cp) :worktime(wt), capacity(cp), storage(0){}

};
class Combo {
public:
	string name;
	vector<string>component;
};
class Order {

};