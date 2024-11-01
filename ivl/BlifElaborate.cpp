#include "BlifElaborate.h"

BlifElaborate::BlifElaborate()
{
}

void BlifElaborate::BlifElaborateRead()
{
    ifstream file("C:\\Users\\86198\\Desktop\\��������\\sample1.blif"); // �滻Ϊ����ļ�·��
    if (!file.is_open()) {
        cerr << "�޷����ļ�" << endl;
    }

    string line;
    while (getline(file, line)) {

        istringstream iss(line);
        string token;

        // ��ȡ��һ�����ʣ�ͨ����ָ��
        iss >> token;
        if (token == ".model") {
            // ��ȡģ�����ƣ������ո�
            std::string model_name;
            if (iss >> model_name) { // ȷ�����Զ�ȡģ������
                setFileName(model_name); // ʹ�����ú��������ļ���
            }
        }

        else if (token == ".inputs") {
            // ��ȡ����˿�
            while (iss >> token) {
//                 BlifGate* input = new BlifGate();
//                 input->setGateType(BlifGate::Input);
//                 addGateMap(token, input);
                setInput(token); // ʹ�����ú����������˿�
                setWire(token);// ʹ�����ú���������ж˿�
            }
        }
        else if (token == ".outputs") {
            // ��ȡ����˿�
            while (iss >> token) {
                setOutput(token); // ʹ�����ú����������˿�
                setWire(token);// ʹ�����ú���������ж˿�
            }
        }
        else if (token == ".names") {
            BlifWire myBlifWire;//Ϊÿ��names������һ����װ�����ݽṹ

            // �������� .names ���е�����
            while (iss >> token) {
                setWire(token);// ʹ�����ú�����Ӷ˿���û�еĶ˿�
                myBlifWire.addHead(token);//������������ж˿�
            }
            BlifGate* nodeGate = new BlifGate();
            string outputName = myBlifWire.getHeadoutput();
            myBlifWire.setOutput(outputName);//��������˿ڵķ���
            string nexline;
            while (getline(file, nexline) && (isdigit(nexline[0]) || nexline[0] == '-')) {
                const vector<string>& it = myBlifWire.getHead();//��ñ�ͷ
                string expr;
                bool first = true;
                for (size_t i = 0; i < it.size(); ++i) {
                    if (nexline[i] == '1') {
                        if (!first) expr += " & ";
                        expr += it[i];
                        first = false;
                    }
                    else if (nexline[i] == '0') {
                        if (!first) expr += " & ";
                        expr += "!" + it[i];
                        first = false;
                    }
                }
                myBlifWire.addLogic(expr);
            }
            myBlifWires.push_back(myBlifWire);

            //Ϊ����˿ڽ���nodeGate
            auto itLogic = myBlifWire.getLogic();
            if (itLogic.size() == 1) {
                for (auto it : itLogic) {
                    nodeGate->setGateType(BlifGate::AND);
                    for (char ch : it) {
                        if (islower(ch)) {
                            string str;
                            str.push_back(ch); // ���ַ���ӵ��ַ��� str ��ĩβ
                            nodeGate->setGateinputs(str);
                        }
                        if (ch == '!') { nodeGate->setGateType(BlifGate::NOT); }
                    }
                }
            }
            else {
                nodeGate->setGateType(BlifGate::OR);
                for (auto it : itLogic) {
                    nodeGate->setGateinputs(it);
                }
            }
            addGateMap(outputName, nodeGate);

            if (!nexline.empty() && !isdigit(nexline[0])) {
                file.putback('\n');
                for (auto it = nexline.rbegin(); it != nexline.rend(); ++it) {
                    file.putback(*it);
                }
            }
        }
    }
    file.close();

}

void BlifElaborate::setFileName(const string& filename)
{
    fileName = filename;
}

string BlifElaborate::getFileName() const
{
    return fileName;
}

void BlifElaborate::setInput(const string& inputName)
{
    input.push_back(inputName);
}

void BlifElaborate::setOutput(const string& outputName)
{
    output.push_back(outputName);
}

void BlifElaborate::setWire(const string& wireName)
{
    auto it = find(wire.begin(), wire.end(), wireName);
    if (it == wire.end()) {
        // ���outputName����outputs�У������
        wire.push_back(wireName);
    }
}

const std::vector<std::string>& BlifElaborate::getInput() const
{
    return input;
}

const std::vector<std::string>& BlifElaborate::getOutput() const
{
    return output;
}

void BlifElaborate::Blif2Verilog()
{
    std::ofstream outFile(this->fileName + ".v");

    // д���ļ���
    string fileName = getFileName();
    outFile << "module " << fileName << "(clk, rst";
    for (const auto& output : output) {
        outFile << "," << output;
    }
    for (const auto& input : input) {
        outFile << "," << input;
    }

    outFile << ");" << endl;
    outFile << "input clk,rst;" << endl;
    for (const auto& output : output) {
        outFile << "output " << output << ";" << endl;
    }
    for (const auto& input : input) {
        outFile << "input  " << input << ";" << endl;
    }
    outFile << endl;

    for (const auto& wire : wire) {
        outFile << "wire  " << wire << ";" << endl;
    }
    outFile << endl;

    for (auto blifWire : myBlifWires) {
        outFile << "assign " << blifWire.getOutput() << " = ";
        const auto& it = blifWire.getLogic();
        for (size_t i = 0; i < it.size(); ++i) {
            //�жϵ�ǰ���Ƿ���ڸ��Ͽ�
            bool hasAnd = false;
            for (char ch : it[i]) {
                if (ch == '&') {
                    hasAnd = true;
                }
            }
            //����
            if (hasAnd && it.size() > 1) {
                outFile << "(" << it[i] << ")";
            }
            else {
                outFile << " " << it[i] << " ";
            }
            if (i < it.size() - 1) {
                outFile << " | ";
            }
        }
        outFile << ";\n";
    }

    outFile << endl;

    outFile << "endmodule" << endl;

    outFile.close();

    std::cout << "������д�뵽�ļ��С�" << std::endl;

}

void BlifElaborate::addGateMap(const string& gateName,BlifGate* myBlifGate)
{
    vector<string> inputs = myBlifGate->getGateInputs();
    for (string input : inputs) {
        // ����input: "a & !c"
        istringstream iss(input);
        string token;
        vector<string> tokens;
        BlifGate* andGate = nullptr;

        while (getline(iss, token, ' '))
        {
            if (token == "&") {
                // ����м仹��&�����ӹ�ϵ�����½����ӹ�ϵ
                andGate = new BlifGate;
                andGate->setGateType(BlifGate::AND);
            }
            else if (token[0] == '!') {
                // ������ַ�����!��ͷ�����½����ӹ�ϵ
                BlifGate* notGate = new BlifGate;
                notGate->setGateinputs(string(1, token[1]));
                notGate->setGateType(BlifGate::NOT);
                gateMap.insert({token, notGate});
                tokens.push_back(token);
            }
            else tokens.push_back(token);
            // ��һ��ѭ��tokens{a}
            // �ڶ���ѭ���½�andGate
            // ������ѭ���½���!c->c,!�����ӹ�ϵ
                // ͬʱ��!c ����token
        }
		if (andGate != nullptr) {
			andGate->setGateinputs(tokens);
			gateMap.insert({ input, andGate });
		}
    }
    gateMap.insert({ gateName, myBlifGate });
}

bool BlifElaborate::findGateInMap(const string& gateName)
{
    auto it = gateMap.find(gateName); // ʹ�� find ���������Ҽ�
    if (it != gateMap.end()) {
        return true;
    }
    // ������������� end��˵��û���ҵ���Ӧ�ļ�
    return false;
}

std::unordered_map<std::string, BlifGate*> BlifElaborate::GetMap() const
{
    return gateMap;
}

BlifWire::BlifWire()
{
}

void BlifWire::setOutput(const string& outPutName)
{
    Output = outPutName;
}

void BlifWire::addLogic(const string& logicRow)
{
    logic.push_back(logicRow);
}

void BlifWire::addHead(const string& headName)
{
    head.push_back(headName);
}

const vector<string>& BlifWire::getHead()
{
    return head;
}

string BlifWire::getOutput()
{
    return Output;
}

string BlifWire::getHeadoutput()
{
    return head.back();
}


const vector<string>& BlifWire::getLogic()
{
    return logic;
}

BlifGate::BlifGate()
{

}

BlifGate::~BlifGate()
{

}

bool BlifGate::deleteGate(const string& s)
{
    auto it = find(GateInputs.begin(), GateInputs.end(), s);
    if (it == GateInputs.end()) return false;
    GateInputs.erase(it);
    return true;
}

void BlifGate::setGateinputs(const vector<string>& vec)
{
    copy(vec.begin(), vec.end(), back_inserter(GateInputs));
}

std::vector<std::string> BlifGate::getGateInputs()
{
    return GateInputs;
}

BlifGate::GateType BlifGate::getGateType() const {
    return myGateType;
}