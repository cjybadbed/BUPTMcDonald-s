#include"Âóµ±ÀÍ.h"


int main() {
	Setup cfg = init();
	int tick = 0;
	vector<int>finishes;
	vector<Working>jobs;
	vector<Order>& orders = cfg.OrderList;
	int comboNum = cfg.typeOfCombo;
	int foodNum = cfg.typeOfFood;
	int maxj = cfg.maxJunctionOrder;
	int minj = cfg.minJunctionOrder;
	bool enable = false;
	Working::foodType = foodNum;
	Working::ComboLUT = cfg.ComboLUT;
	while (tick < 86400) {
		if(tick>=25199&&tick<=79199)Cookupdate(cfg.FoodLUT, tick);
		if (enable && jobs.size() > maxj)enable = false;
		if ((!enable) && jobs.size() < minj)enable = true;
		acceptor(enable, tick, orders, jobs, finishes);
		vector<int> result = cater();
		tick++;
	}
	ofstream outFile("output.txt");
	for (int fin : finishes) {
		outFile << convertTime(fin) << "\n";
	}
	outFile.close();
	return 0;
}