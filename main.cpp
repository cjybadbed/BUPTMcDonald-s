#include"Âóµ±ÀÍ.h"


int main() {
	Setup cfg = init();
	int tick = 0;
	vector<int>finishes;
	
	while (tick < 86400) {
		Cookupdate(cfg.FoodLUT, tick);
		



		tick++;
	}
	ofstream outFile("output.txt");
	for (int fin : finishes) {
		outFile << convertTime(fin) << "\n";
	}
	outFile.close();
	return 0;
}