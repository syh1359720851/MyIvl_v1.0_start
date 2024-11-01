#pragma once
#include "BlifElaborate.h"
#include <queue>

class Res {
public:
	Res() : state(0), cycle(0), working(nullptr) {};
	virtual ~Res();

	bool Start(string gate);
	bool isFree();
	void Run1cycle();
	int getCycle() const;
	void ResetCycle();
	string getGate();
	
private:
	string working;
	int state; // 0: free; 1: busy
	int cycle;

};

class ResManage {
public:
	ResManage(int And, int Or, int Not) {
		vector<Res> andGate(And);
		vector<Res> orGate(Or);
		vector<Res> notGate(Not);
	}
	int andBusyAmount();
	int orBusyAmount();
	int notBusyAmount();
	int andFreePos();
	int orFreePos();
	int notFreePos();
	bool allFree();
	vector<string> run1cycle();
	queue<string> andReady;
	queue<string> orReady;
	queue<string> notReady;

private:
	vector<Res> andGate;
	vector<Res> orGate;
	vector<Res> notGate;
};

class Scheduling {
private:
	const unordered_map<string, BlifGate*> gateMap;
	const vector<string> inputGates;
	const vector<string> outputGates;
public:
	Scheduling();
	Scheduling(const BlifElaborate& blifElaborate);

	void ASAP();
	void ALAP();
	void ML_RCS(int And, int Or, int Not);
	void MR_LCS(int cycle);

};