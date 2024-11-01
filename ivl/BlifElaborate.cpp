#include "BlifElaborate.h"

BlifElaborate::BlifElaborate()
{
}

void BlifElaborate::BlifElaborateRead()
{
    ifstream file("C:\\Users\\86198\\Desktop\\测试样例\\sample1.blif"); // 替换为你的文件路径
    if (!file.is_open()) {
        cerr << "无法打开文件" << endl;
    }

    string line;
    while (getline(file, line)) {

        istringstream iss(line);
        string token;

        // 读取第一个单词，通常是指令
        iss >> token;
        if (token == ".model") {
            // 读取模型名称，跳过空格
            std::string model_name;
            if (iss >> model_name) { // 确保可以读取模型名称
                setFileName(model_name); // 使用设置函数设置文件名
            }
        }

        else if (token == ".inputs") {
            // 读取输入端口
            while (iss >> token) {
//                 BlifGate* input = new BlifGate();
//                 input->setGateType(BlifGate::Input);
//                 addGateMap(token, input);
                setInput(token); // 使用设置函数添加输入端口
                setWire(token);// 使用设置函数添加所有端口
            }
        }
        else if (token == ".outputs") {
            // 读取输出端口
            while (iss >> token) {
                setOutput(token); // 使用设置函数添加输出端口
                setWire(token);// 使用设置函数添加所有端口
            }
        }
        else if (token == ".names") {
            BlifWire myBlifWire;//为每个names块设置一个封装的数据结构

            // 继续解析 .names 行中的令牌
            while (iss >> token) {
                setWire(token);// 使用设置函数添加端口中没有的端口
                myBlifWire.addHead(token);//设置里面的所有端口
            }
            BlifGate* nodeGate = new BlifGate();
            string outputName = myBlifWire.getHeadoutput();
            myBlifWire.setOutput(outputName);//设置输出端口的符号
            string nexline;
            while (getline(file, nexline) && (isdigit(nexline[0]) || nexline[0] == '-')) {
                const vector<string>& it = myBlifWire.getHead();//获得表头
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

            //为输出端口建立nodeGate
            auto itLogic = myBlifWire.getLogic();
            if (itLogic.size() == 1) {
                for (auto it : itLogic) {
                    nodeGate->setGateType(BlifGate::AND);
                    for (char ch : it) {
                        if (islower(ch)) {
                            string str;
                            str.push_back(ch); // 将字符添加到字符串 str 的末尾
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
        // 如果outputName不在outputs中，添加它
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

    // 写入文件名
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
            //判断当前行是否存在复合块
            bool hasAnd = false;
            for (char ch : it[i]) {
                if (ch == '&') {
                    hasAnd = true;
                }
            }
            //括号
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

    std::cout << "数据已写入到文件中。" << std::endl;

}

void BlifElaborate::addGateMap(const string& gateName,BlifGate* myBlifGate)
{
    vector<string> inputs = myBlifGate->getGateInputs();
    for (string input : inputs) {
        // 假设input: "a & !c"
        istringstream iss(input);
        string token;
        vector<string> tokens;
        BlifGate* andGate = nullptr;

        while (getline(iss, token, ' '))
        {
            if (token == "&") {
                // 如果中间还有&的连接关系，则新建连接关系
                andGate = new BlifGate;
                andGate->setGateType(BlifGate::AND);
            }
            else if (token[0] == '!') {
                // 如果该字符串以!开头，则新建连接关系
                BlifGate* notGate = new BlifGate;
                notGate->setGateinputs(string(1, token[1]));
                notGate->setGateType(BlifGate::NOT);
                gateMap.insert({token, notGate});
                tokens.push_back(token);
            }
            else tokens.push_back(token);
            // 第一次循环tokens{a}
            // 第二次循环新建andGate
            // 第三次循环新建了!c->c,!的连接关系
                // 同时将!c 放入token
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
    auto it = gateMap.find(gateName); // 使用 find 方法来查找键
    if (it != gateMap.end()) {
        return true;
    }
    // 如果迭代器等于 end，说明没有找到对应的键
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