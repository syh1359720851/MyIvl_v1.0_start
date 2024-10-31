#include "Scheduling.h"

using namespace std;
Res::~Res()
{

}

void Res::ResetCycle()
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
	this->cycle++;
}

int Res::getCycle() const
{
	return this->cycle;
}

BlifGate* AndGate::isDone()
{
	BlifGate* gate = Res::getGate();
	if (Res::getCycle() == 2) {
		return gate;
		Res::ResetCycle();
	}
	else {
		return nullptr;
	}
}

BlifGate* OrGate::isDone()
{
	BlifGate* gate = Res::getGate();
	if (Res::getCycle() == 3) {
		return gate;
		Res::ResetCycle();
	}
	else {
		return nullptr;
	}
}

BlifGate* NotGate::isDone()
{
	BlifGate* gate = Res::getGate();
	if (Res::getCycle() == 1) {
		return gate;
		Res::ResetCycle();
	}
	else {
		return nullptr;
	}
}


Scheduling::Scheduling()
{

}



Scheduling::Scheduling(const BlifElaborate& blifElaborate) :
	gateMap(blifElaborate.GetMap()), inputGates(blifElaborate.getInput()), outputGates(blifElaborate.getOutput()) 
{

}

void Scheduling::ASAP() {
	unordered_map<string, BlifGate*> ASAPgateMap(gateMap);
	queue<string> ready; // ��������
	vector<vector<string>> gate_of_cycle; // ��ά���飬����cycle��
	// ���Ƚ����е�input�������������
	for (const string& input : inputGates) {
		ready.push(input);
	}
	gate_of_cycle.push_back(inputGates);
	while (!ready.empty()) {
		// ÿ��
		int n = ready.size();
		vector<string> currCycle;
		for (int i = 0; i < n; i++) {
			// ÿ���о������е�����wire
			// ��õ�ǰinput
			string currgate = ready.front();
			ready.pop();
			auto it = ASAPgateMap.begin();
			for (it; it != ASAPgateMap.end(); ) {
				// ��������value������е�ǰ��wire����ɾ��
				it->second->deleteGate(currgate);
				if (it->second->getGateInputs().size() == 0) {
					// ����ýڵ��ǰ��ڵ��Ѿ�ȫ��������������������
					ready.push(it->first);
					// ͬʱ���뵱ǰ�ֵ�vector��
					currCycle.push_back(it->first);
					// �ڱ���ɾ���ýڵ㣬��ֹ�ڶ���ѭ��ʱ�ٴη���value��0���ٴμ���vector��
					it = ASAPgateMap.erase(it);
				}
				else {
					++it;
				}
			}
		}
		// ����ǰ�ֵ�vector���뵽�ܵ�vector��
		gate_of_cycle.push_back(currCycle);
	}

	// ��ӡ
	//for ()
	//for (const auto& vec : gate_of_cycle) {
	//	cout << 
	//}
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
