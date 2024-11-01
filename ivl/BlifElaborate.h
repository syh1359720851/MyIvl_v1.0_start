#ifndef MYSTRUCT_H
#define MYSTRUCT_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cctype> // 用于 std::islower 和 std::tolower

class BlifGate;
class BlifWire;
class BlifElaborate;


using namespace std;

class BlifGate
{
public:
    BlifGate();
    ~BlifGate();
    enum GateType {
        AND,
        OR,
        NOT,
        Input
    }; //该端口输入的类型
    bool deleteGate(const string& s);
    void setGateinputs(const string& s) { GateInputs.push_back(s); } //创建端口名称
    void setGateinputs(const vector<string>& vec);
    void setGateType(GateType type) { myGateType = type; } // 设置端口类型
    vector<string> getGateInputs(); 
    GateType getGateType() const;
private:
    vector<string> GateInputs; //该端口拥有的所有输入端口
    GateType myGateType; //该端口输入的类型
};

class BlifWire
{
public:
    BlifWire();
    void setOutput(const string& outPutName);
    string getOutput(); //获取输出端口名

    void addHead(const string& headName);
    const vector<string>& getHead();//获取表头

    void addLogic(const string& logicRow);
    const vector<string>& getLogic();//获取逻辑式

    string getHeadoutput(); //获取表头的输出端口名
private:
    string Output;
    vector<string> head;  // 表头，例如 {"d", "e", "f", "j"}
    vector<string> logic;  // 逻辑表达式
};

class BlifElaborate
{
public:
    BlifElaborate();
    void setFileName(const string& filename);
    string getFileName() const;
    void setInput(const string& inputName);
    void setOutput(const string& outputName);
    void setWire(const string& wireName);
    const vector<string>& getInput() const;
    const vector<string>& getOutput() const;
    void BlifElaborateRead();  // 解析
    void Blif2Verilog();   //输出

    void addGateMap(const string& gateName, BlifGate* myBlifGate); //将排列好的gate名和其所拥有的输入和type进行添加到MAP
    bool findGateInMap(const string& gateName); //从哈希表中找到该gate
    unordered_map<string, BlifGate*> GetMap() const;
private:
    string fileName; //该blif文件名称
    vector<BlifWire> myBlifWires;//存储所有的连接关系的数据结构
    vector<string> output; //blif中的输出端口
    vector<string> input; //blif中的输入端口
    vector<string> wire; //blif中的端口
    ifstream file;
    unordered_map<string, BlifGate*> gateMap; //该Blif的哈希表
};


#endif // !MYSTRUCT_H