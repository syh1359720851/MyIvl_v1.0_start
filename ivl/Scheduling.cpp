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

	// 打印
	//for ()
	//for (const auto& vec : gate_of_cycle) {
	//	cout << 
	//}
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
				if (!visitedMap.find(*it)->second && find(inputGates.begin(), inputGates.end(), *it) == inputGates.end()) {
					// 未加入过，且未找到
					toReady.push(*it);
					currCycle.push_back(*it);
					visitedMap[*it] = 0;
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
	// 新建map
	unordered_map<string, BlifGate*> MLgateMap;
	for (auto it = gateMap.begin(); it != gateMap.end(); it++) {
		MLgateMap[it->first] = new BlifGate;
		MLgateMap[it->first]->setGateinputs(it->second->getGateInputs());
		MLgateMap[it->first]->setGateType(it->second->getGateType());
	}

	// 初始化ResManage
	ResManage resManage(And, Or, Not);

	// 二维数组储存结果
	vector<vector<string>> gate_of_cycle;

	// 用一个队列储存已经完成的门 
	queue<string> finished;
	
	while (!resManage.allFree() && !finished.empty()) {
		// 每轮
		vector<string> currCycle;
		// 搜索就绪的门，加入就绪队列中
		while (!finished.empty()) {
			string currgate = finished.front();
			finished.pop();
			auto it = MLgateMap.begin();
			for (it; it != MLgateMap.end(); ) {
				// 遍历所有value，如果有当前的wire，则删掉
				it->second->deleteGate(currgate);
				// 如果该节点的前序节点已经全部就绪，则加入就绪队列
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
					// 在表中删除该节点，防止第二遍循环时再次发现value是0，再次加入vector中
					it = MLgateMap.erase(it);
				}
				else {
					++it;
				}
			}

		}
		
		// 运行一个cycle
		currCycle = resManage.run1cycle();
		
		// 将当前轮的vector加入到总的vector中
		gate_of_cycle.push_back(currCycle);
	}

		// 打印
	
}

void Scheduling::MR_LCS(int cycle)
{
	// 根据ASAP确定最多的gate数量
	// 减小一个gate看看能不能运行
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

bool ResManage::allFree() // return 1 表示全都free
{
	return !(andBusyAmount() && orBusyAmount() && notBusyAmount());
}

std::vector<std::string> ResManage::run1cycle() // 返回该轮已经完成的gate
{	
	vector<string> result;
	// 如果有空位，将就绪队列中的放入执行
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

	// 执行
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
