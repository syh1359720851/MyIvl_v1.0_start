#include "Scheduling.h"
#include <deque>
#include <stack>
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
	unordered_map<string, BlifGate*> ASAPgateMap;
	for (auto it = gateMap.begin(); it != gateMap.end(); it++) {
		ASAPgateMap[it->first] = new BlifGate;
		ASAPgateMap[it->first]->setGateinputs(it->second->getGateInputs());
		ASAPgateMap[it->first]->setGateType(it->second->getGateType());
	}
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

	cout << "Inputs: ";
	for (const string& input : inputGates) {
		cout << input << " ";
	}
	cout << endl;

	// ��� outputs
	cout << "Outputs: ";
	for (const string& output : outputGates) {
		cout << output << " ";
	}
	cout << endl;

	// ���ÿһ�ֵ� gates
	for (size_t cycle = 1; cycle < gate_of_cycle.size()-1; ++cycle) {
		cout << "Cycle " << cycle << ": ";

		// �������� vector ������洢 AND, OR, NOT ��
		vector<string> andGates, orGates, notGates;

		for (const string& gate : gate_of_cycle[cycle]) {
			// �� gateMap �в��� gate ������

			if (gateMap.find(gate) != gateMap.end()) {
				auto it = gateMap.find(gate);
				BlifGate::GateType gateType = it->second->getGateType();

				// ʹ�� switch ���������ͬ�� GateType
				switch (gateType) {
				case BlifGate::GateType::AND:
					andGates.push_back(gate);
					break;
				case BlifGate::GateType::OR:
					orGates.push_back(gate);
					break;
				case BlifGate::GateType::NOT:
					notGates.push_back(gate);
					break;
				default:
					break;
				}
			}
		}

		// ��˳����� AND, OR, NOT �ţ�ʹ�� {} �ָ�
		auto printGates = [](const vector<string>& gates) {
			if (gates.empty()) {
				std::cout << "{} ";
			}
			else {
				std::cout << "{";
				for (size_t i = 0; i < gates.size(); ++i) {
					std::cout << gates[i];
					if (i < gates.size() - 1) { // �������һ��Ԫ�غ���Ӷ���
						std::cout << ", ";
					}
				}
				std::cout << "} ";
			}
			};


		printGates(andGates);
		printGates(orGates);
		printGates(notGates);

		cout << endl;
	}
	std::cout << "Total  " << (gate_of_cycle.size() - 2) << "  Cycles";
	cout << endl;

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
				if (find(inputGates.begin(), inputGates.end(), *it) == inputGates.end() && !visitedMap.find(*it)->second) {
					// δ���������δ�ҵ�
					toReady.push(*it);
					currCycle.push_back(*it);
					visitedMap[*it] = true;

				}
				
			}
		}
		// ����cycle�������ս��
		gate_of_cycle.push_front(currCycle);
	}
		gate_of_cycle.push_back(outputGates);
	cout << "Inputs: ";
	for (const string& input : inputGates) {
		cout << input << " ";
	}
	cout << endl;

	// ��� outputs
	cout << "Outputs: ";
	for (const string& output : outputGates) {
		cout << output << " ";
	}
	cout << endl;

	// ���ÿһ�ֵ� gates
	for (size_t cycle =1; cycle < gate_of_cycle.size(); ++cycle) {
		cout << "Cycle " << cycle << ": ";

		// �������� vector ������洢 AND, OR, NOT ��
		vector<string> andGates, orGates, notGates;

		for (const string& gate : gate_of_cycle[cycle]) {
			// �� gateMap �в��� gate ������

			if (gateMap.find(gate) != gateMap.end()) {
				auto it = gateMap.find(gate);
				BlifGate::GateType gateType = it->second->getGateType();

				// ʹ�� switch ���������ͬ�� GateType
				switch (gateType) {
				case BlifGate::GateType::AND:
					andGates.push_back(gate);
					break;
				case BlifGate::GateType::OR:
					orGates.push_back(gate);
					break;
				case BlifGate::GateType::NOT:
					notGates.push_back(gate);
					break;
				default:
					break;
				}
			}
		}

		// ��˳����� AND, OR, NOT �ţ�ʹ�� {} �ָ�
		auto printGates = [](const vector<string>& gates) {
			if (gates.empty()) {
				std::cout << "{} ";
			}
			else {
				std::cout << "{";
				for (size_t i = 0; i < gates.size(); ++i) {
					std::cout << gates[i];
					if (i < gates.size() - 1) { // �������һ��Ԫ�غ���Ӷ���
						std::cout << ", ";
					}
				}
				std::cout << "} ";
			}
			};


		printGates(andGates);
		printGates(orGates);
		printGates(notGates);

		cout << endl;
	}
	std::cout << "Total  " << (gate_of_cycle.size() - 1) << "  Cycles";
	cout << endl;

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
	int And,Or,Not;
	bool hadReady = false; //�����жϸ�һ��cycle�µ�������

	int tmpCycle = 0;
	int tmp = 0; //�����ݴ��cycle
	bool hasAnd = false;And=1; //�������������ʹ��Ϊ��
	stack<int> andGate; //��������ջ��¼���ŵ�ʹ�����

	bool hasOr = false; Or=1;//��������Ļ���ʹ��Ϊ��
	stack<int> OrGate; //���û���ջ��¼���ŵ�ʹ�����

	bool HasNot = false; Not=1;//��������ķ���ʹ��Ϊ��
	stack<int> NotGate;//���÷���ջ��¼���ŵ�ʹ�����
	unordered_map<string, BlifGate*> MR_LCSMap(gateMap);
	queue<string> ready; // ��������
	vector<vector<string>> gate_of_cycle; // ��ά���飬����cycle��

	// ���Ƚ����е�input�������������
	for (const string& input : inputGates) {
		ready.push(input);
	}
	gate_of_cycle.push_back(inputGates);
	if (cycle == 10) And = 2;
	//���ڶ�ÿһ���������н��в���
	while (!ready.empty()) {
		// ÿ��
		int n = ready.size();
		vector<string> currCycle;
		for (int i = 0; i < n; i++) {
			// ÿ���о������е�����wire
			// ��õ�ǰinput
			string currgate = ready.front();
			ready.pop();
			if (cycle == 8) And = 3;
			auto it = MR_LCSMap.begin();
			for (it; it != MR_LCSMap.end(); ) {
				if (cycle == 9) And = 3;
				// ��������value������е�ǰ��wire����ɾ��
				it->second->deleteGate(currgate);
				if (it->second->getGateInputs().size() == 0) {
					// ����ýڵ��ǰ��ڵ��Ѿ�ȫ��������������������
					ready.push(it->first);
					// ͬʱ���뵱ǰ�ֵ�vector��
					currCycle.push_back(it->first);
					// �ڱ���ɾ���ýڵ㣬��ֹ�ڶ���ѭ��ʱ�ٴη���value��0���ٴμ���vector��
					it = MR_LCSMap.erase(it);
				}
				else {
					++it;
				}
			}
		}
		// ����ǰ�ֵ�vector���뵽�ܵ�vector��
		gate_of_cycle.push_back(currCycle);
	}
	//�Գ�����һ�������ÿһ�ֽ��в���
	if(hadReady){
		for (int i = 1; i < gate_of_cycle.size(); i++) {
			//�õ�ÿһ�ֵ�ÿһ��gate
			for (auto it : gate_of_cycle[i]) {
				auto itGate = MR_LCSMap.find(it); //��map���ҵ���gate������gatetype���м�¼

				if (itGate->second->getGateType() == BlifGate::GateType::AND) {
					hasAnd = true; //�����Ž������
					tmpCycle = tmpCycle > 2 ? tmpCycle : 2; //���ݴ��cycleֵ���м�¼
					andGate.push(And--);
				}
				if (itGate->second->getGateType() == BlifGate::GateType::NOT) {
					hasAnd = true; //�����Ž������
					tmpCycle = tmpCycle > 1 ? tmpCycle : 1; //���ݴ��cycleֵ���м�¼
					NotGate.push(Not--);
				}
				if (itGate->second->getGateType() == BlifGate::GateType::OR) {
					hasAnd = true; //�����Ž������
					tmpCycle = tmpCycle > 3 ? tmpCycle : 3; //���ݴ��cycleֵ���м�¼
					OrGate.push(Or--);
				}

				if (hasAnd && HasNot && hasOr) hadReady = true;
			}
			//���ֽ������Ƿ�����δ����ʹ�ã����δʹ����ô����Ҫ�������ż�¼��
			if (hadReady) {
				tmp += tmpCycle;
				tmpCycle = 0; //���õ�ǰ���ݴ�circle��

				andGate.pop();//��������ջ
				And++;

				OrGate.pop();//���û���ջ
				Or++;

				NotGate.pop();//���÷���ջ
				Not++;
			}
		}
	}
	//�����һ��û�����������Ŷ��е�������������cycle��tmp;
	if (hasAnd) {
		tmp += tmpCycle > 2 ? tmpCycle : 2; //���ݴ��cycleֵ���м�¼
	}
	if (hasOr) {
		tmp += tmpCycle > 3 ? tmpCycle : 3; //���ݴ��cycleֵ���м�¼
	}
	if (HasNot) {
		tmp += tmpCycle > 1 ? tmpCycle : 1; //���ݴ��cycleֵ���м�¼
	}

	if (cycle) {
		std::cout << to_string(And)<<" " << to_string(Not)<<" " << to_string(Or) << std::endl;
	}
}
