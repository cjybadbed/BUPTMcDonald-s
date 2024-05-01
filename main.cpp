#include"Âóµ±ÀÍ.h"


int main() {
	Setup cfg = init();
	int tick = 25199;
	vector<int>finishes;
	vector<Working>jobs;
	vector<Food>foodList = cfg.FoodLUT;
	vector<Order>& orders = cfg.OrderList;
	int comboNum = cfg.typeOfCombo;
	int foodNum = cfg.typeOfFood;
	int maxj = cfg.maxJunctionOrder;
	int minj = cfg.minJunctionOrder;
	bool enable = false;
	
	while (tick < 86400) {
		if(tick>=25199&&tick<=79199)Cookupdate(cfg.FoodLUT, tick);
		if (enable && jobs.size() > maxj)enable = false;
		if ((!enable) && jobs.size() < minj)enable = true;
		acceptor(enable, tick, orders, jobs, finishes,foodNum,cfg.ComboLUT);
		catering(tick,jobs,finishes,foodList,orders,foodNum);
		tick++;
	}
	ofstream outFile("output.txt");
	for (int& fin : finishes) {
		outFile << convertTime(fin);
		if (&fin != &finishes.back())outFile << '\n';
	}
	outFile.close();
	return 0;
}