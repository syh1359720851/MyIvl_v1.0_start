#ifndef MYSTRUCT_H
#define MYSTRUCT_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

class BilfWire;
class BilfElaborate;


using namespace std;


class BilfWire
{
public:
    BilfWire();
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

class BilfElaborate
{
public:
    BilfElaborate();
    void setFileName(const string& filename);
    string getFileName();
    void setInput(const string& inputName);
    void setOutput(const string& outputName);
    void setWire(const string& wireName);
    void BilfElaborateRead();  // 解析
    void Bilf2Verilog();   //输出

private:
    string fileName; //该bilf文件名称
    vector<BilfWire> myBilfWires;//存储所有的连接关系的数据结构
    vector<string> output; //bilf中的输出端口
    vector<string> input; //bilf中的输入端口
    vector<string> wire; //bilf中的端口
    ifstream file;
};

class mystruct
{
};

#endif // !MYSTRUCT_H
