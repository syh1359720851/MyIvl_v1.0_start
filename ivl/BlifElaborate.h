#ifndef MYSTRUCT_H
#define MYSTRUCT_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

class BlifGate;
class BlifWire;
class BlifElaborate;


using namespace std;

class BlifGate
{
public:
    BlifGate();
    ~BlifGate();
    void setGateInpus(const string& s) { GateInputs.push_back(s); } //�����˿�����
private:
    vector<string> GateInputs; //�ö˿�ӵ�е���������˿�
    enum GateType {
        AND,
        OR,
        NOT
    } GateType; //�ö˿����������
};

class BlifWire
{
public:
    BlifWire();
    void setOutput(const string& outPutName);
    string getOutput(); //��ȡ����˿���

    void addHead(const string& headName);
    const vector<string>& getHead();//��ȡ��ͷ

    void addLogic(const string& logicRow);
    const vector<string>& getLogic();//��ȡ�߼�ʽ

    string getHeadoutput(); //��ȡ��ͷ������˿���
private:
    string Output;
    vector<string> head;  // ��ͷ������ {"d", "e", "f", "j"}
    vector<string> logic;  // �߼����ʽ
};

class BlifElaborate
{
public:
    BlifElaborate();
    void setFileName(const string& filename);
    string getFileName();
    void setInput(const string& inputName);
    void setOutput(const string& outputName);
    void setWire(const string& wireName);
    const vector<string>& getInput() const;
    const vector<string>& getOutput() const;
    void BlifElaborateRead();  // ����
    void Blif2Verilog();   //���

    void addGateMap(const string& gateName, const BlifGate& myBlifGate); //�����кõ�gate��������ӵ�е������type������ӵ�MAP
    bool findGateInMap(const string& gateName); //�ӹ�ϣ�����ҵ���gate
private:
    string fileName; //��blif�ļ�����
    vector<BlifWire> myBlifWires;//�洢���е����ӹ�ϵ�����ݽṹ
    vector<string> output; //blif�е�����˿�
    vector<string> input; //blif�е�����˿�
    vector<string> wire; //blif�еĶ˿�
    ifstream file;
    unordered_map<string, BlifGate> gateMap; //��Blif�Ĺ�ϣ��
};

class mystruct
{
};

#endif // !MYSTRUCT_H