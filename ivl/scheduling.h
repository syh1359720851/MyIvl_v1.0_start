#pragma once
#include "BlifElaborate.h"
#include <queue>

class Res {
public:
	Res() : state(0), cycle(0), working(nullptr) {};
	virtual ~Res();

	bool Start(BlifGate* gate);
	bool isFree();
	void Run1cycle();
	virtual BlifGate* isDone() = 0;
protected:
	int getCycle() const;
	void ResetCycle();
	BlifGate* getGate();
	
private:
	BlifGate* working;
	int state; // 0: free; 1: busy
	int cycle;

};

class AndGate : public Res {
public:
	virtual BlifGate* isDone();
};

class OrGate : public Res {
public:
	virtual BlifGate* isDone();
};

class NotGate : public Res {
public:
	virtual BlifGate* isDone();
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