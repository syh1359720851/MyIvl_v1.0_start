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

	cout << "Inputs: ";
	for (const string& input : inputGates) {
		cout << input << " ";
	}
	cout << endl;

	// 输出 outputs
	cout << "Outputs: ";
	for (const string& output : outputGates) {
		cout << output << " ";
	}
	cout << endl;

	// 输出每一轮的 gates
	for (size_t cycle = 1; cycle < gate_of_cycle.size()-1; ++cycle) {
		cout << "Cycle " << cycle << ": ";

		// 创建三个 vector 来分类存储 AND, OR, NOT 门
		vector<string> andGates, orGates, notGates;

		for (const string& gate : gate_of_cycle[cycle]) {
			// 从 gateMap 中查找 gate 的类型

			if (gateMap.find(gate) != gateMap.end()) {
				auto it = gateMap.find(gate);
				BlifGate::GateType gateType = it->second->getGateType();

				// 使用 switch 语句来处理不同的 GateType
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

		// 按顺序输出 AND, OR, NOT 门，使用 {} 分割
		auto printGates = [](const vector<string>& gates) {
			if (gates.empty()) {
				std::cout << "{} ";
			}
			else {
				std::cout << "{";
				for (size_t i = 0; i < gates.size(); ++i) {
					std::cout << gates[i];
					if (i < gates.size() - 1) { // 不在最后一个元素后添加逗号
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
	}
		gate_of_cycle.push_back(outputGates);
	cout << "Inputs: ";
	for (const string& input : inputGates) {
		cout << input << " ";
	}
	cout << endl;

	// 输出 outputs
	cout << "Outputs: ";
	for (const string& output : outputGates) {
		cout << output << " ";
	}
	cout << endl;

	// 输出每一轮的 gates
	for (size_t cycle =1; cycle < gate_of_cycle.size(); ++cycle) {
		cout << "Cycle " << cycle << ": ";

		// 创建三个 vector 来分类存储 AND, OR, NOT 门
		vector<string> andGates, orGates, notGates;

		for (const string& gate : gate_of_cycle[cycle]) {
			// 从 gateMap 中查找 gate 的类型

			if (gateMap.find(gate) != gateMap.end()) {
				auto it = gateMap.find(gate);
				BlifGate::GateType gateType = it->second->getGateType();

				// 使用 switch 语句来处理不同的 GateType
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

		// 按顺序输出 AND, OR, NOT 门，使用 {} 分割
		auto printGates = [](const vector<string>& gates) {
			if (gates.empty()) {
				std::cout << "{} ";
			}
			else {
				std::cout << "{";
				for (size_t i = 0; i < gates.size(); ++i) {
					std::cout << gates[i];
					if (i < gates.size() - 1) { // 不在最后一个元素后添加逗号
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
	int And,Or,Not;
	bool hadReady = false; //设置判断该一个cycle下的最少门

	int tmpCycle = 0;
	int tmp = 0; //设置暂存的cycle
	bool hasAnd = false;And=1; //设置最初的与门使用为非
	stack<int> andGate; //设置与门栈记录与门的使用情况

	bool hasOr = false; Or=1;//设置最初的或门使用为非
	stack<int> OrGate; //设置或门栈记录或门的使用情况

	bool HasNot = false; Not=1;//设置最初的非门使用为非
	stack<int> NotGate;//设置非门栈记录非门的使用情况
	unordered_map<string, BlifGate*> MR_LCSMap(gateMap);
	queue<string> ready; // 就绪队列
	vector<vector<string>> gate_of_cycle; // 二维数组，行是cycle数

	// 首先将所有的input放入就绪队列中
	for (const string& input : inputGates) {
		ready.push(input);
	}
	gate_of_cycle.push_back(inputGates);
	if (cycle == 10) And = 2;
	//现在对每一个就绪队列进行操作
	while (!ready.empty()) {
		// 每轮
		int n = ready.size();
		vector<string> currCycle;
		for (int i = 0; i < n; i++) {
			// 每轮中就绪队列的所有wire
			// 获得当前input
			string currgate = ready.front();
			ready.pop();
			if (cycle == 8) And = 3;
			auto it = MR_LCSMap.begin();
			for (it; it != MR_LCSMap.end(); ) {
				if (cycle == 9) And = 3;
				// 遍历所有value，如果有当前的wire，则删掉
				it->second->deleteGate(currgate);
				if (it->second->getGateInputs().size() == 0) {
					// 如果该节点的前序节点已经全部就绪，则加入就绪队列
					ready.push(it->first);
					// 同时加入当前轮的vector中
					currCycle.push_back(it->first);
					// 在表中删除该节点，防止第二遍循环时再次发现value是0，再次加入vector中
					it = MR_LCSMap.erase(it);
				}
				else {
					++it;
				}
			}
		}
		// 将当前轮的vector加入到总的vector中
		gate_of_cycle.push_back(currCycle);
	}
	//对除开第一次以外的每一轮进行操作
	if(hadReady){
		for (int i = 1; i < gate_of_cycle.size(); i++) {
			//拿到每一轮的每一个gate
			for (auto it : gate_of_cycle[i]) {
				auto itGate = MR_LCSMap.find(it); //在map中找到该gate并对其gatetype进行记录

				if (itGate->second->getGateType() == BlifGate::GateType::AND) {
					hasAnd = true; //对与门进行添加
					tmpCycle = tmpCycle > 2 ? tmpCycle : 2; //对暂存的cycle值进行记录
					andGate.push(And--);
				}
				if (itGate->second->getGateType() == BlifGate::GateType::NOT) {
					hasAnd = true; //对与门进行添加
					tmpCycle = tmpCycle > 1 ? tmpCycle : 1; //对暂存的cycle值进行记录
					NotGate.push(Not--);
				}
				if (itGate->second->getGateType() == BlifGate::GateType::OR) {
					hasAnd = true; //对与门进行添加
					tmpCycle = tmpCycle > 3 ? tmpCycle : 3; //对暂存的cycle值进行记录
					OrGate.push(Or--);
				}

				if (hasAnd && HasNot && hasOr) hadReady = true;
			}
			//该轮结束后是否有门未进行使用，如果未使用那么就需要继续接着记录；
			if (hadReady) {
				tmp += tmpCycle;
				tmpCycle = 0; //重置当前的暂存circle；

				andGate.pop();//重置与门栈
				And++;

				OrGate.pop();//重置或门栈
				Or++;

				NotGate.pop();//重置非门栈
				Not++;
			}
		}
	}
	//在最后一次没有三种类型门都有的情况，加入最大cycle到tmp;
	if (hasAnd) {
		tmp += tmpCycle > 2 ? tmpCycle : 2; //对暂存的cycle值进行记录
	}
	if (hasOr) {
		tmp += tmpCycle > 3 ? tmpCycle : 3; //对暂存的cycle值进行记录
	}
	if (HasNot) {
		tmp += tmpCycle > 1 ? tmpCycle : 1; //对暂存的cycle值进行记录
	}

	if (cycle) {
		std::cout << to_string(And)<<" " << to_string(Not)<<" " << to_string(Or) << std::endl;
	}
}
