#include"mystruct.h"

BlifElaborate::BlifElaborate()
{
}

void BlifElaborate::BlifElaborateRead()
{
    ifstream file("test1.blif"); // 替换为你的文件路径
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

string BlifElaborate::getFileName()
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

void BlifElaborate::Blif2Verilog()
{
    std::ofstream outFile(this->fileName + ".v");

    // 写入文件名
    string fileName = getFileName();
    outFile << "module " << fileName <<"(clk, rst";
    for (const auto& output : output) {
        outFile << "," << output;
    }
    for (const auto& input : input) {
        outFile << "," << input;
    }

    outFile << ");"<< endl;
    outFile << "input clk,rst;" << endl;
    for (const auto& output : output) {
        outFile << "output " << output << ";" << endl;
    }
    for (const auto& input : input) {
        outFile << "input  "<<input << ";" << endl;
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

    outFile << "endmodule"<< endl;

    outFile.close();

    std::cout << "数据已写入到文件中。" << std::endl;

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




