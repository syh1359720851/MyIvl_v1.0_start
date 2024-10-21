#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Module.h"
#include "mystruct.h"
using namespace std;

class AssignmentTreeNode {  // 需要一个完善AssignmentTreeNode的接口，以及维护该文件对应的.c文件
public:
	bool operator==(const AssignmentTreeNode& other) const;
	static string genName();
	enum GateType
	{
		AND = 0,
		OR,
		NOT
	};
private:
	static int count;
	int gateType;
	bool endNode;  // 是否为端点
	vector<string> inputs;  // 输入列表
	string output;  // 输出(也是节点名称）
};


class Scheduling {
private:
	unordered_map<string, shared_ptr<AssignmentTreeNode>> nodeMap;
	shared_ptr<AssignmentTreeNode> beginNode;  // beginNode 连接所有首个计算的节点
	shared_ptr<AssignmentTreeNode> endNode;  // endNode 连接所有最后计算的节点
	void m_buildTree(perm_string gate, Module* mod);
public:
	Scheduling();
	void BuildTree(Module* mod, BlifElaborate& myBilf );
	void ASAP();  // one for this func
	void ALAP();  // and one for this func
};

// syh_remote change 1