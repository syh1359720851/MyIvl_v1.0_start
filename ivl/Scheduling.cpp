#include "Scheduling.h"
#include <deque>

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
	unordered_map<string, BlifGate*> ASAPgateMap;
	for (auto it = gateMap.begin(); it != gateMap.end(); it++) {
		ASAPgateMap[it->first] = new BlifGate;
		ASAPgateMap[it->first]->setGateinputs(it->second->getGateInputs());
		ASAPgateMap[it->first]->setGateType(it->second->getGateType());
	}
	queue<string> ready; // 就绪队列
	vector<vector<string>> gate_of_cycle; // 二维数组，行是cycle数
	// 首先将所有的input放入就绪队列中
	for (const string& input : inputGates) {
		ready.push(input);
	}
	gate_of_cycle.push_back(inputGates);
	while (!ready.empty()) {
		// 每轮
		int n = ready.size();
		vector<string> currCycle;
		for (int i = 0; i < n; i++) {
			// 每轮中就绪队列的所有wire
			// 获得当前input
			string currgate = ready.front();
			ready.pop();
			auto it = ASAPgateMap.begin();
			for (it; it != ASAPgateMap.end(); ) {
				// 遍历所有value，如果有当前的wire，则删掉
				it->second->deleteGate(currgate);
				if (it->second->getGateInputs().size() == 0) {
					// 如果该节点的前序节点已经全部就绪，则加入就绪队列
					ready.push(it->first);
					// 同时加入当前轮的vector中
					currCycle.push_back(it->first);
					// 在表中删除该节点，防止第二遍循环时再次发现value是0，再次加入vector中
					it = ASAPgateMap.erase(it);
				}
				else {
					++it;
				}
			}
		}
		// 将当前轮的vector加入到总的vector中
		gate_of_cycle.push_back(currCycle);
	}

	for (size_t cycle = 0; cycle < gate_of_cycle.size(); ++cycle) {
		cout << "Cycle " << cycle << ": ";
		for (const string& gate : gate_of_cycle[cycle]) {
			cout << gate << " ";
		}
		cout << endl;
	}
}


void Scheduling::ALAP()
{
	// 初始化visitedMap
	typedef bool isVisited;
	unordered_map<string, isVisited> visitedMap;
	for (auto it = gateMap.begin(); it != gateMap.end(); it++) {
		visitedMap[it->first] = 0; // 代表还没有加入过
	}
	// 初始化队列
	queue<string> toReady;
	for (const string& output : outputGates) {
		toReady.push(output);
	}
	
	// 双端队列存储结果，因为需要用到push_front
	deque<vector<string>> gate_of_cycle;
	
	int n = toReady.size();
	while (!toReady.empty()) {
		// 外循环，对应每个cycle
		int n = toReady.size();
		vector<string> currCycle;
		for (int i = 0; i < n; i++) {
			// 内循环，对应每个cycle中遍历toReady
			string currgate = toReady.front();
			toReady.pop();

			// 将右侧的value中的所有gate都加入到待就绪队列中
			// 如果已经加入过，则不加
			// 如果是input，则不加
			auto inputs = gateMap.find(currgate)->second->getGateInputs();
			for (auto it = inputs.begin(); it != inputs.end(); it++) {
				if (find(inputGates.begin(), inputGates.end(), *it) == inputGates.end() && !visitedMap.find(*it)->second) {
					// 未加入过，且未找到
					toReady.push(*it);
					currCycle.push_back(*it);
					visitedMap[*it] = true;

				}
				
			}
		}
		// 将该cycle放入最终结果
		gate_of_cycle.push_front(currCycle);

		// 打印
	}
}

void Scheduling::ML_RCS(int And, int Or, int Not)
{
	
}

void Scheduling::MR_LCS(int cycle)
{

}
