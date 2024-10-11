#ifndef MYNETLIST_H
#define MYNETLIST_H

#include <string>
#include <map>
#include <set>
#include <vector>

#include <netlist.h>
#include <Module.h>

//#include "main.cc"

class MyDesign;
class MyScope;
class MyModule;


using IdString = std::string;
//typedef std::string IdString;

enum MY_PORT_TYPE { NOT_A_PORT2, PIMPLICIT2, PINPUT2, POUTPUT2, PINOUT2, PREF2 };

struct MYMID
{
    IdString name;
};

struct MYWIRE
{
    IdString name;
    //bool port_input;
    //bool port_output;
    MY_PORT_TYPE porttype;
};

struct MYPORT
{
    IdString     portname;
    char        inout_type;  //0:in, 1:out
};

struct MYCELL
{
    //char last_op;
    //std::vector<IdString> name_store;

    IdString name;
    IdString type;
    std::map<IdString, MYPORT> cell_conns;  //<wirename,port>

    bool equal(const MYCELL& other) const;
    bool in_equal(const MYCELL& other) const;

};

struct NameTable
{
    string name;
    int to_op;
    int pri = 0;
};

struct OpTable
{
    char op;
    int to_op;
};


struct MYEXP
{
    string left;
    string right;
    std::vector<NameTable> nameTable;
    std::vector<OpTable> opTable;
    int max_op = -1;
};


class MyScope {

public:
    MyScope();
    ~MyScope();
    std::vector<MYWIRE> m_wires;
    std::vector<MYPORT> m_ports;
    std::vector<MYCELL> m_cells;
    std::vector<MYMID> m_mids;

    std::vector<MYEXP> m_exp;

    std::map<string, string> output;  //<output wire name, out put expression>
    int state;

    string path;
    void set_num_ports(unsigned int num_ports);
    string IsOut(string exp);
private:

};

class MyModule: public MyScope {

public:
    MyModule();
    ~MyModule();
    bool elaborate_sig(Module* rmod);
    bool elaborate_gate(Module* rmod);
    void My_SubExpOpt();
    void CalPri();
    void MakeCell();
    string  elaborate_expression(PExpr* expr);
    string  elaborate_expression(PExpr* expr, MYEXP* myexp);
private:
    char last_op;
    std::vector<string> name_store;
};

class MyDesign {

public:
    MyDesign();
    ~MyDesign();

    MyModule scope;

private:

};




#endif
