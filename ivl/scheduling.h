#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Module.h"

using namespace std;

struct AssignmentTreeNode {  // ��Ҫһ������AssignmentTreeNode�Ľӿڣ��Լ�ά�����ļ���Ӧ��.c�ļ�
public:

private:
	enum GateType
	{
		AND = 0,
		OR,
		NOT
	};
	int gateType;
	bool endNode;  // �Ƿ�Ϊ�˵�
	vector<string> inputs;  // �����б�
	string output;  // node
	vector<shared_ptr<AssignmentTreeNode>> forwardNodes;  // ǰ���ڵ�
	vector<shared_ptr<AssignmentTreeNode>> backNodes;  // �����ڵ�
};


class Scheduling {
	shared_ptr<AssignmentTreeNode> beginNode;  // beginNode ���������׸�����Ľڵ�
	shared_ptr<AssignmentTreeNode> endNode;  // endNode ��������������Ľڵ�
public:
	void BuildTree(Module* mod, BlifElaborate& myBlif) {  // one for this func

	}
	void ASAP();  // one for this func
	void ALAP();  // and one for this func
};

// syh_remote change 1