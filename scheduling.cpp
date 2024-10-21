#include "scheduling.h"
#include <algorithm>
#include "PGate.h"

using namespace std;

int AssignmentTreeNode::count = 0;
class PGate;

bool AssignmentTreeNode::operator==(const AssignmentTreeNode& other) const {
	if (this->gateType == other.gateType && this->inputs == other.inputs) {
		return true;
	}
	else return false;
}

string AssignmentTreeNode::genName()
{
	return "_unknown" + to_string(count);
	count++;
}


Scheduling::Scheduling()
{
	beginNode = make_shared<AssignmentTreeNode>();
	endNode = make_shared<AssignmentTreeNode>();
}

void Scheduling::BuildTree(Module* mod, BlifElaborate& myBlif)
{
	vector<perm_string> input;
	vector<perm_string> output;
	list<PGate*> gates = mod->get_gates();
	for (const auto wireName : myBlif.getInput()) {
		input.push_back(wireName);
	}
	for (const auto wireName : myBlif.getOutput()) {
		output.push_back(wireName);
	}
	
	for (const auto currgate : output) {
		gates
	}
}

void Scheduling::m_buildTree(perm_string gate, Module* mod) {
	list<PGate*> gates = mod->get_gates();
	
} 