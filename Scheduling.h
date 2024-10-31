#pragma once
#include "BlifElaborate.h"
#include <queue>

class Res {
public:
	Res() : state(0), cycle(0) {};
	virtual ~Res();

	virtual bool Start(BlifGate* gate);
	virtual bool isFree();
	virtual void Run1cycle();
	virtual BlifGate* isDone() = 0;

protected:
	int getCycle() const;
	int ResetCycle();
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
	unordered_map<string, BlifGate*> gateMap;
	vector<string> inputGates;
	vector<string> outputGates;
public:
	Scheduling();
	Scheduling(BlifElaborate& blifElaborate) : gateMap(blifElaborate.GetMap(), inputGates(blifElaborate.getInput()), outputGates(blifElaborate.getOutput()) ){};

	 friend BlifGate void ASAP();
	void ALAP();
	void ML_RCS(int And, int Or, int Not);
	void MR_LCS(int cycle);

};