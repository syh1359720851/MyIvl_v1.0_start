#include "Scheduling.h"

using namespace std;
Res::~Res()
{

}

bool Res::Start(BlifGate* gate)
{
	if (working != nullptr) {
		return false;
	}
	working = gate;
	return true;
}

bool Res::isFree()
{
	if (working != nullptr) {
		return false;
	}
	else return true;
}

void Res::Run1cycle()
{
	cycle++;
}

int Res::getCycle() const
{
	return cycle;
}

int Res::ResetCycle()
{
	cycle = 0;
}

BlifGate* Res::getGate()
{
	return working;
}
	
bool Res::Start(BlifGate* gate)
{
	if (working != nullptr) {
		return false;
	}
	working = gate;
	return true;
}

bool Res::isFree()
{
	if (working != nullptr) {
		return false;
	}
	else return true;
}

void Res::Run1cycle()
{
	cycle++;
}

int Res::getCycle() const
{
	return cycle;
}

BlifGate* AndGate::isDone()
{
	BlifGate* gate = getGate();
	if (getCycle() == 2) {
		return gate;
		ResetCycle();
	}
	else {
		return nullptr;
	}
}

BlifGate* OrGate::isDone()
{
	BlifGate* gate = getGate();
	if (getCycle() == 3) {
		return gate;
		ResetCycle();
	}
	else {
		return nullptr;
	}
}

BlifGate* NotGate::isDone()
{
	BlifGate* gate = getGate();
	if (getCycle() == 1) {
		return gate;
		ResetCycle();
	}
	else {
		return nullptr;
	}
}


Scheduling::Scheduling()
{

}

Scheduling::Scheduling(BlifElaborate& blifElaborate) : gateMap(blifElaborate.GetMap())
{

}

void Scheduling::ASAP() {
	unordered_map<string, BlifGate*> ASAPgateMap(gateMap);
	queue<string> ready;
	vector<vector<string>> gate_of_cycle;
	for (const string& input : inputGates) {
		ready.push(input);
	}
	while (!ready.empty()) {
		int n = ready.size();
		for (int i = 0; i < n; i++) {
			// 遍历所有门，如果value中有，则删掉
			string currgate = ready.front();
			ready.pop();
			auto it = ASAPgateMap.begin();
			for (it; it != ASAPgateMap.end(); it++) {
				it->second->deleteGate(currgate);
				if (it->second->getGateInputs().size() == 0) {
					// 如果该节点的前序节点已经全部就绪，则加入就绪队列

				}
			}
		}
	}
}

void Scheduling::ALAP()
{

}

void Scheduling::ML_RCS(int And, int Or, int Not)
{

}

void Scheduling::MR_LCS(int cycle)
{

}
