#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Module.h"

using namespace std;

struct AssignmentTreeNode {  // 需要一个完善AssignmentTreeNode的接口，以及维护该文件对应的.c文件
public:

private:
	enum GateType
	{
		AND = 0,
		OR,
		NOT
	};
	int gateType;
	bool endNode;  // 是否为端点
	vector<string> inputs;  // 输入列表
	string output;  // 输出
	vector<shared_ptr<AssignmentTreeNode>> forwardNodes;  // 前驱节点
	vector<shared_ptr<AssignmentTreeNode>> backNodes;  // 后续节点
};


class Scheduling {
	shared_ptr<AssignmentTreeNode> beginNode;  // beginNode 连接所有首个计算的节点
	shared_ptr<AssignmentTreeNode> endNode;  // endNode 连接所有最后计算的节点
public:
	void BuildTree(Module* mod) {  // one for this func

	}
	void ASAP();  // one for this func
	void ALAP();  // and one for this func
};

// syh_remote change 1