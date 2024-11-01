#ifndef MYSTRUCT_H
#define MYSTRUCT_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cctype> // ���� std::islower �� std::tolower

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
    }; //�ö˿����������
    bool deleteGate(const string& s);
    void setGateinputs(const string& s) { GateInputs.push_back(s); } //�����˿�����
    void setGateinputs(const vector<string>& vec);
    void setGateType(GateType type) { myGateType = type; } // ���ö˿�����
    vector<string> getGateInputs(); 
    GateType getGateType() const;
private:
    vector<string> GateInputs; //�ö˿�ӵ�е���������˿�
    GateType myGateType; //�ö˿����������
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
    string getFileName() const;
    void setInput(const string& inputName);
    void setOutput(const string& outputName);
    void setWire(const string& wireName);
    const vector<string>& getInput() const;
    const vector<string>& getOutput() const;
    void BlifElaborateRead();  // ����
    void Blif2Verilog();   //���

    void addGateMap(const string& gateName, BlifGate* myBlifGate); //�����кõ�gate��������ӵ�е������type������ӵ�MAP
    bool findGateInMap(const string& gateName); //�ӹ�ϣ�����ҵ���gate
    unordered_map<string, BlifGate*> GetMap() const;
private:
    string fileName; //��blif�ļ�����
    vector<BlifWire> myBlifWires;//�洢���е����ӹ�ϵ�����ݽṹ
    vector<string> output; //blif�е�����˿�
    vector<string> input; //blif�е�����˿�
    vector<string> wire; //blif�еĶ˿�
    ifstream file;
    unordered_map<string, BlifGate*> gateMap; //��Blif�Ĺ�ϣ��
};


#endif // !MYSTRUCT_H