#include "Scheduling.h"
#include <deque>

using namespace std;
Res::~Res()
{

}

void Res::ResetCycle()
{
	cycle = 0;
	working = " ";
}

string Res::getGate()
{
	return working;
}
	
bool Res::Start(string gate)
{
	if (working != " ") {
		return false;
	}
	working = gate;
	return true;
}

bool Res::isFree()
{
	if (working != " ") {
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
	// ��ʼ��visitedMap
	typedef bool isVisited;
	unordered_map<string, isVisited> visitedMap;
	for (auto it = gateMap.begin(); it != gateMap.end(); it++) {
		visitedMap[it->first] = 0; // ����û�м����
	}
	// ��ʼ������
	queue<string> toReady;
	for (const string& output : outputGates) {
		toReady.push(output);
	}
	
	// ˫�˶��д洢�������Ϊ��Ҫ�õ�push_front
	deque<vector<string>> gate_of_cycle;
	
	while (!toReady.empty()) {
		// ��ѭ������Ӧÿ��cycle
		int n = toReady.size();
		vector<string> currCycle;
		for (int i = 0; i < n; i++) {
			// ��ѭ������Ӧÿ��cycle�б���toReady
			string currgate = toReady.front();
			toReady.pop();

			// ���Ҳ��value�е�����gate�����뵽������������
			// ����Ѿ���������򲻼�
			// �����input���򲻼�
			auto inputs = gateMap.find(currgate)->second->getGateInputs();
			for (auto it = inputs.begin(); it != inputs.end(); it++) {
				if (!visitedMap.find(*it)->second && find(inputGates.begin(), inputGates.end(), *it) == inputGates.end()) {
					// δ���������δ�ҵ�
					toReady.push(*it);
					currCycle.push_back(*it);
					visitedMap[*it] = 0;
				}
				
			}
		}
		// ����cycle�������ս��
		gate_of_cycle.push_front(currCycle);

		// ��ӡ
	}
}

void Scheduling::ML_RCS(int And, int Or, int Not)
{
	// �½�map
	unordered_map<string, BlifGate*> MLgateMap;
	for (auto it = gateMap.begin(); it != gateMap.end(); it++) {
		MLgateMap[it->first] = new BlifGate;
		MLgateMap[it->first]->setGateinputs(it->second->getGateInputs());
		MLgateMap[it->first]->setGateType(it->second->getGateType());
	}

	// ��ʼ��ResManage
	ResManage resManage(And, Or, Not);

	// ��ά���鴢����
	vector<vector<string>> gate_of_cycle;

	// ��һ�����д����Ѿ���ɵ��� 
	queue<string> finished;
	
	while (!resManage.allFree() && !finished.empty()) {
		// ÿ��
		vector<string> currCycle;
		// �����������ţ��������������
		while (!finished.empty()) {
			string currgate = finished.front();
			finished.pop();
			auto it = MLgateMap.begin();
			for (it; it != MLgateMap.end(); ) {
				// ��������value������е�ǰ��wire����ɾ��
				it->second->deleteGate(currgate);
				// ����ýڵ��ǰ��ڵ��Ѿ�ȫ��������������������
				if (it->second->getGateInputs().size() == 0) {
					switch (it->second->getGateType())
					{
					case BlifGate::AND :
						resManage.andReady.push(it->first);
						break;
					case BlifGate::OR:
						resManage.orReady.push(it->first);
						break;
					case BlifGate::NOT:
						resManage.notReady.push(it->first);
						break;
					default:
						break;
					}
					// �ڱ���ɾ���ýڵ㣬��ֹ�ڶ���ѭ��ʱ�ٴη���value��0���ٴμ���vector��
					it = MLgateMap.erase(it);
				}
				else {
					++it;
				}
			}

		}
		
		// ����һ��cycle
		currCycle = resManage.run1cycle();
		
		// ����ǰ�ֵ�vector���뵽�ܵ�vector��
		gate_of_cycle.push_back(currCycle);
	}

		// ��ӡ
	
}

void Scheduling::MR_LCS(int cycle)
{
	// ����ASAPȷ������gate����
	// ��Сһ��gate�����ܲ�������
	//
}

int ResManage::andBusyAmount()
{
	int count = andGate.size();
	for (auto& gate : andGate) {
		if (!gate.isFree()) {
			count--;
		}
	}
	return count;
}

int ResManage::orBusyAmount()
{
	int count = orGate.size();
	for (auto& gate : orGate) {
		if (!gate.isFree()) {
			count--;
		}
	}
	return count;
}

int ResManage::notBusyAmount()
{
	int count = notGate.size();
	for (auto& gate : notGate) {
		if (!gate.isFree()) {
			count--;
		}
	}
	return count;
}

int ResManage::andFreePos()
{
	for (int i = 0; i < andGate.size(); i++) {
		if (andGate[i].isFree()) {
			return i;
		}
	}
	return -1;
}

int ResManage::orFreePos()
{
	for (int i = 0; i < orGate.size(); i++) {
		if (orGate[i].isFree()) {
			return i;
		}
	}
	return -1;
}

int ResManage::notFreePos()
{
	for (int i = 0; i < notGate.size(); i++) {
		if (notGate[i].isFree()) {
			return i;
		}
	}
	return -1;
}

bool ResManage::allFree() // return 1 ��ʾȫ��free
{
	return !(andBusyAmount() && orBusyAmount() && notBusyAmount());
}

std::vector<std::string> ResManage::run1cycle() // ���ظ����Ѿ���ɵ�gate
{	
	vector<string> result;
	// ����п�λ�������������еķ���ִ��
	while (int i = andFreePos() != -1 && !andReady.empty()) {
		andGate[i].Start(andReady.front());
		andReady.pop();
	}
	while (int i = orFreePos() != -1 && !orReady.empty()) {
		orGate[i].Start(andReady.front());
		orReady.pop();
	}
	while (int i = notFreePos() != -1 && !notReady.empty()) {
		notGate[i].Start(notReady.front());
		notReady.pop();
	}

	// ִ��
	for (auto& gate : andGate) {
		gate.Run1cycle();
		if (gate.getCycle() == 2) {
			result.push_back(gate.getGate());
			gate.ResetCycle();
		}
	}
	for (auto& gate : orGate) {
		gate.Run1cycle();
		if (gate.getCycle() == 3) {
			result.push_back(gate.getGate());
			gate.ResetCycle();
		}
	}
	for (auto& gate : notGate) {
		gate.Run1cycle();
		if (gate.getCycle() == 1) {
			result.push_back(gate.getGate());
			gate.ResetCycle();
		}
	}
	return result;
}
