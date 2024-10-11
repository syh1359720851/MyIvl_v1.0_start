
# include  "pform.h"
# include  "PClass.h"
# include  "PEvent.h"
# include  "PGenerate.h"
# include  "PPackage.h"
# include  "PScope.h"
# include  "PSpec.h"
# include  "parse_api.h"
# include  "Mynetlist.h"
#include "utils.h"


#include <algorithm> 

using namespace std;
using IdString = std::string;
std::map<std::string, MYWIRE*> sub_exp;

std::map<std::string, int> id2num_store;

int id2num(std::string id)
{
	if (id2num_store.count(id) > 0)
		return id2num_store[id];
	return id2num_store[id] = id2num_store.size() + 1;
}

int get_fileline()
{
	return 0;
}
MyScope::MyScope(void)
{

}

MyScope::~MyScope(void)
{

}

MyModule::MyModule(void) {

}

MyModule::~MyModule(void) {

}

MyDesign::MyDesign(void)
{

}

MyDesign::~MyDesign(void)
{

}


void MyModule::My_SubExpOpt() {
	for (auto it = m_cells.begin(); it != m_cells.end(); ++it) {
		for (auto conn = (*it).cell_conns.begin(); conn != (*it).cell_conns.end(); ) {
			bool modified = false;
			for (auto out = output.begin(); out != output.end(); ++out) {
				if (out->second == conn->first) {
					// 将conn->first 修改为 out->first
					auto node = *conn;
					conn = (*it).cell_conns.erase(conn);  // 删除原来的键值对并更新迭代器
					(*it).cell_conns.insert({ out->first, node.second });  // 插入新的键值对
					modified = true;  // 标记已经修改
					break;  // 键值对已经修改，退出当前循环
				}
			}
			if (!modified) {
				++conn;  // 只有在没有修改的情况下才增加迭代器
			}
		}
	}


	
	for (auto it = m_cells.begin(); it != m_cells.end(); ++it) {
		for (auto it2 = it + 1; it2 != m_cells.end(); ++it2) {
			if (it->equal(*it2)) {
				auto it_temp = it2;
				/*if (it2 + 1 != m_cells.end())
					it2 = it2 + 1;*/
				it2--;
				m_cells.erase(it_temp);
			}
			//else if (it->in_equal(*it2)) {
			//	int out1 = 0, out2 = 0;
			//	string name;
			//	// 从conn连接信息中，读取到该cell的输出name
			//	for (auto conn = (*it).cell_conns.begin(); conn != (*it).cell_conns.end();++conn) {
			//		if ((*conn).second.inout_type = 1)
			//			name = (*conn).first;
			//	}
			//	// 将输出name与wire作比较，如果二者相同，证明是输出，out=1
			//	for (int i = 0; i < m_wires.size(); i++) {
			//		if (m_wires[i].name == name) {
			//			out1 = 1;
			//		}
			//	}
			//	// 比较it2
			//	for (auto conn = (*it2).cell_conns.begin(); conn != (*it2).cell_conns.end();++conn) {
			//		if ((*conn).second.inout_type = 1)
			//			name = (*conn).first;
			//	}
			//	for (int i = 0; i < m_wires.size(); i++) {
			//		if (m_wires[i].name == name) {
			//			out2 = 1;
			//		}
			//	}

			//	// 对两种情况的处理
			//	if (out1 == 1 && out2 == 1);
			//	else if (out1 == 0 && out2 == 1) {
			//		
			//	}
				
			//}
		}
	}
}

void MyModule::MakeCell() {
	CalPri();

	for (int i = 0; i < m_exp.size(); i++) {
		while (1) {
			if (m_exp[i].nameTable.size() == 1) {
				
				break;
			}
			else {
				NameTable firstname = m_exp[i].nameTable[0];
				for (int j = 1; j < m_exp[i].nameTable.size(); j++) {

					if (firstname.to_op == m_exp[i].nameTable[j].to_op) {
						
						// 将二者插入cell和nameTable
						string str_left = firstname.name;
						string str_right =  m_exp[i].nameTable[j].name;
						if (str_left > str_right)
						{
							auto temp = str_left;
							str_left = str_right;
							str_right = temp;
						}
						str_left = IsOut(str_left);
						str_right = IsOut(str_right);

						string sch(1, m_exp[i].opTable[m_exp[i].nameTable[j].to_op].op);
						string strname = str_left + sch + str_right;  // strname为二者结合后的字符串

						if (m_exp[i].nameTable.size() == 2) {
							output.insert({ m_exp[i].left, strname});
							strname = m_exp[i].left;
						}

						// cell
						MYMID tmp;
						tmp.name = strname;
						m_mids.push_back(tmp);


						MYCELL cell;
						cell.name = "$" + std::to_string(m_cells.size() + 1);
						cell.type = "$" + sch;
						MYPORT port1 = { "A", 0 };
						MYPORT port2 = { "B", 0 };
						MYPORT port3 = { "Y", 1 };
						cell.cell_conns.insert({ str_left, port1 });
						//int k = id2num(str_left);
						cell.cell_conns.insert({ str_right, port2 });
						//k = id2num(str_right);
						cell.cell_conns.insert({ strname, port3 });
						//k = id2num(strname);
						m_cells.push_back(cell);

						// nametable
						
						NameTable newname;
						newname.name = strname;
						int to_op = m_exp[i].opTable[m_exp[i].nameTable[j].to_op].to_op;
						if (to_op == -1) {
							newname.to_op = m_exp[i].nameTable[j].to_op;
						}
						else {
							newname.to_op = to_op;
						}
						m_exp[i].nameTable.erase(m_exp[i].nameTable.begin() + j);
						m_exp[i].nameTable.erase(m_exp[i].nameTable.begin());
						

						m_exp[i].nameTable.push_back(newname);

						for (int i1 = 0; i1 < m_exp[i].max_op; i1++) {
							for (int j1 = i1 + 1; j1 < m_exp[i].max_op + 1; j1++) {
								//if (i == j)  continue;
								if (m_exp[i].opTable[i1].op == m_exp[i].opTable[i1].op && m_exp[i].opTable[i1].to_op == j1)
								{
									for (int k = 0; k < m_exp[i].nameTable.size() - 1; k++)
									{
										if (m_exp[i].nameTable[k].to_op == i1)
											m_exp[i].nameTable[k].to_op = j1;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
	}
}

string MyScope::IsOut(string exp) {
	for (auto it = output.begin(); it != output.end(); ++it) {
		if ((*it).second == exp)
			return (*it).first;
	}
	return exp;
}

bool MYCELL::equal(const MYCELL& other) const {
	if (cell_conns.size() != other.cell_conns.size()) {
		return false;
	}
	if (type != other.type) {
		return false;
	}

	std::vector<IdString> vec1_keys;
	std::vector<IdString> vec2_keys;

	for (const auto& pair : cell_conns) {
		//if ( pair.second.inout_type == 0)
		vec1_keys.push_back(pair.first);
	}
	for (const auto& pair : other.cell_conns) {
		//if (pair.second.inout_type == 0)
		vec2_keys.push_back(pair.first);
	}

	std::sort(vec1_keys.begin(), vec1_keys.end());
	std::sort(vec2_keys.begin(), vec2_keys.end());

	return vec1_keys == vec2_keys;
}

bool MYCELL::in_equal(const MYCELL& other) const{
	if (cell_conns.size() != other.cell_conns.size()) {
		return false;
	}
	if (type != other.type) {
		return false;
	}

	std::vector<IdString> vec1_keys;
	std::vector<IdString> vec2_keys;

	for (const auto& pair : cell_conns) {
		if ( pair.second.inout_type == 0)
		vec1_keys.push_back(pair.first);
	}
	for (const auto& pair : other.cell_conns) {
		if (pair.second.inout_type == 0)
		vec2_keys.push_back(pair.first);
	}

	std::sort(vec1_keys.begin(), vec1_keys.end());
	std::sort(vec2_keys.begin(), vec2_keys.end());

	return vec1_keys == vec2_keys;
}

string elaborate_ident(PEIdent* ident)
{
	pform_name_t ls = ident->path();
	list<name_component_t>::iterator s = ls.begin();
	return (*s).name.str();       
}

string  MyModule::elaborate_expression(PExpr* expr)
{
	string exp;
	if (PEIdent* ident = dynamic_cast<PEIdent*>(expr)) 
	{
		exp =  elaborate_ident(ident);
		return exp;
	}
	else if (PEBinary* binary = dynamic_cast<PEBinary*>(expr)) {

		PExpr* left = (PExpr*)binary->get_left();
		string str_left = elaborate_expression(left);

		char Op = binary->get_op();
		string sch(1, Op);

		PEIdent* right = (PEIdent*)binary->get_right();
		string str_right = elaborate_expression(right);

		if (str_left > str_right)
		{
			auto temp = str_left;
			str_left = str_right;
			str_right = temp;
		}

		string strname = str_left + sch + str_right;
		MYMID tmp;
		tmp.name = strname;
		m_mids.push_back(tmp);

		MYCELL cell;
		cell.name = "$" + to_string(m_cells.size() + 1);
		cell.type = "$" + sch;
		MYPORT port1 = { "A",0 };
		MYPORT port2 = { "B",0 };
		MYPORT port3 = { "Y",1 };
		cell.cell_conns.insert({ str_left, port1 });
		cell.cell_conns.insert({ str_right, port2 });
		cell.cell_conns.insert({ strname, port3});
		m_cells.push_back(cell);

		return strname;
	}
	else if (PEUnary* unary = dynamic_cast<PEUnary*>(expr)) {
	}
}

string  MyModule::elaborate_expression(PExpr* expr, MYEXP* myexp)
{
	string exp;
	if (PEIdent* ident = dynamic_cast<PEIdent*>(expr))
	{
		exp = elaborate_ident(ident);
		if (state == 1) {  //左子树
			NameTable Name;
			Name.name = exp;
			Name.to_op = myexp->max_op + 1;
			myexp->nameTable.push_back(Name);
		}
		else if (state == 4) {  //左子树
			NameTable Name;
			Name.name = exp;
			Name.to_op = myexp->max_op + 1;
			myexp->nameTable.push_back(Name);
		}
		else if (state == 3) {  //右子树
			NameTable Name;
			Name.name = exp;
			Name.to_op = myexp->max_op;
			myexp->nameTable.push_back(Name);
		}
		return exp;
	}

	else if (PEBinary* binary = dynamic_cast<PEBinary*>(expr)) {

		PExpr* left = (PExpr*)binary->get_left();
		if (state == 3) state = 4;
		else state = 1;  //状态为左子式输入
		string str_left = elaborate_expression(left, myexp);

		char op = binary->get_op();
		string sch(1, op);

		if (state == 1) {
			myexp->max_op++;
			OpTable Op;
			Op.op = op;
			Op.to_op = myexp->max_op + 1;
			myexp->opTable.push_back(Op);
		}
		else if (state == 3) {
			if (myexp->opTable[myexp->max_op].op == op)
			{
				// 什么也不做
			}
			else {
				// 检测是不是最顶层，如果不是，将to_op标记为-1
				int i = myexp->max_op;
				while (i > 0) {
					if (myexp->opTable[i].to_op == -1)
						break;
					i--;
				}
				if (i == 0)
				{
					myexp->max_op++;
					OpTable Op;
					Op.op = op;
					Op.to_op = -1;
					myexp->opTable.push_back(Op);
				}
				else {
					myexp->max_op++;
					OpTable Op;
					Op.op = op;
					Op.to_op = i;
					myexp->opTable.push_back(Op);
				}
			}
			
		}
		else if (state == 4) {
			int i = 0;
			if (myexp->opTable[myexp->max_op].op == op)
			{
				myexp->nameTable[myexp->nameTable.size() - 1].to_op = myexp->max_op;
			}
			else {
				myexp->opTable[myexp->opTable.size() - 1].to_op++;
				myexp->max_op++;
				OpTable Op;
				Op.op = op;
				Op.to_op = myexp->max_op-1;
				myexp->opTable.push_back(Op);
				
			}
			
		}

		//PEIdent* right = (PEIdent*)binary->get_right();
		state = 3;
		string str_right = elaborate_expression(binary->get_right(), myexp);

		

		
	}
	else if (PEUnary* unary = dynamic_cast<PEUnary*>(expr)) {
	}
}

void MyModule::CalPri()
{
	for (int i = 0; i < m_exp.size(); i++) {  // 待匹配name循环
		
		for (int j = 0; j < m_exp[i].nameTable.size(); j++) {  
			
			for (int i1 = 0; i1 < m_exp.size(); i1++) {  // 匹配对象内部循环
			
				for (int j1 = 0; j1 < m_exp[i1].nameTable.size(); j1++) {

					if (m_exp[i].nameTable[j].name == m_exp[i1].nameTable[j1].name
						&& m_exp[i].opTable[ m_exp[i].nameTable[j].to_op ].op == m_exp[ i1].opTable[m_exp[i1].nameTable[j1].to_op ].op)

						m_exp[i].nameTable[j].pri++;
				}
			}
		}
	}

	// 按pri的顺序排序
	for (int i = 0; i < m_exp.size(); i++) {
		std::sort(m_exp[i].nameTable.begin(), m_exp[i].nameTable.end(), [](const NameTable& a, const NameTable& b) {
			return a.pri > b.pri;
			});
	}
		
}

bool MyModule::elaborate_gate(Module* rmod)
{
	const list<PGate*>& gl = rmod->get_gates();

	for (list<PGate*>::const_iterator gt = gl.begin()
		; gt != gl.end(); ++gt) {
		PGate* cur = *gt;

		state = 0;
		MYEXP myexp;
		string str_finalout = elaborate_expression(cur->pin(0));     
		myexp.left = str_finalout;

		string str_allright = elaborate_expression(cur->pin(1), &myexp);
		myexp.right = str_allright;
		if (myexp.opTable.size() == 1) {
			myexp.opTable[0].to_op = -1;
		}
		
		for (int i = 0; i < myexp.max_op ; i++) {
			for (int j = i+1; j < myexp.max_op + 1; j++) {
				//if (i == j)  continue;
				if (myexp.opTable[i].op == myexp.opTable[j].op && myexp.opTable[i].to_op == j)
				{
					for (int k = 0; k < myexp.nameTable.size() - 1; k++)
					{
						if (myexp.nameTable[k].to_op == i)
							myexp.nameTable[k].to_op = j;
					}
				}
			}
		}
		for (int i = 0; i < myexp.max_op; i++) {
			if (myexp.opTable[i].to_op == myexp.max_op + 1)
				myexp.opTable[i].to_op = -1;
		}
		m_exp.push_back(myexp);
	}
	MakeCell();
	return true;
}

bool MyModule::elaborate_sig(Module* rmod)
{
		bool flag = true;
        //get wire
		for (map<perm_string, PWire*>::const_iterator wt = rmod->wires.begin()
			; wt != rmod->wires.end(); ++wt) {

			PWire* cur = (*wt).second;
			MYWIRE tmp;
			tmp.name = cur->basename();
			tmp.porttype = (enum MY_PORT_TYPE) cur->get_port_type();
			//tmp.port_input = 1;
			//tmp.port_output = 1;
			m_wires.push_back(tmp);
		}
		///get port
		for (unsigned idx = 0; idx < rmod->ports.size(); idx += 1) {
			Module::port_t* pp = rmod->ports[idx];
			MYPORT tmp;
			const char* namestr = pp->name.str();

			tmp.portname += *namestr;
			tmp.inout_type = 0;
			m_ports.push_back(tmp);
		}

	return true;
}

MyDesign* My_elaborate(list<perm_string>roots)
{

      MyDesign*des = new MyDesign;

	  for (list<perm_string>::const_iterator root = roots.begin()
		  ; root != roots.end(); ++root) {

		  map<perm_string, Module*>::const_iterator mod = pform_modules.find(*root);

		  Module* rmod = (*mod).second;

		  des->scope.path = rmod->get_file().str();
		  //des->scope.set_num_ports( rmod->port_count() );

		  des->scope.elaborate_sig(rmod);
		  des->scope.elaborate_gate(rmod);
		  des->scope.My_SubExpOpt();
	  }
      return des;
}

int My_show_netlist(MyDesign* des)
{

	//std::string module_name(des->scope.path.str());
	std::string module_name = des->scope.path;

	// 创建并打开.dot文件
	FILE* dotfile = fopen("output\\test1.dot", "w");
	if (dotfile == nullptr) {
		printf("failed to open the file.");
		return -1;
	}

	// 写入.dot文件的头部信息
	fprintf(dotfile, "digraph \"%s\" {\n", module_name.c_str());
	fprintf(dotfile, "label=\"%s\";\n", module_name.c_str());
	fprintf(dotfile, "rankdir=\"LR\";\n");
	fprintf(dotfile, "remincross=true;\n");

	////////////////get wire()/////////////////
	for (auto it = des->scope.m_cells.begin(); it != des->scope.m_cells.end(); ++it) {
		for (auto conn = (*it).cell_conns.begin(); conn != (*it).cell_conns.end(); ++conn) { // 注意这里的 ++conn
			int k = id2num((*conn).first);
			
		}
	}



	std::set<std::string> all_sources, all_sinks;
	vector<int> idcount;

	for (auto wire : des->scope.m_wires) {
		const char* shape = "diamond", * color = "Black", * frontcolor = "Black";
		if (wire.porttype == PINPUT2 || wire.porttype == POUTPUT2 || wire.porttype == PINOUT2)
			shape = "octagon";

		fprintf(dotfile, "n%d [ shape=%s, label=\"%s\",color=\"%s\",frontcolor=\"%s\"];\n", id2num(wire.name), shape, wire.name.c_str(), color, frontcolor);
		if (wire.porttype == PINPUT2 || wire.porttype == PINOUT2)
		{
			all_sources.insert(stringf("n%d", id2num(wire.name)));
		}

		if (wire.porttype == POUTPUT2 || wire.porttype == PINOUT2)
		{
			all_sinks.insert(stringf("n%d", id2num(wire.name)));
		}
		idcount.push_back(id2num(wire.name));
	}

	for (auto id : id2num_store) {
		auto it = std::find(idcount.begin(), idcount.end(), id.second);
		if (it == idcount.end())
		{
			const char* color = "Black", * frontcolor = "Black";
			fprintf(dotfile, "n%d [label=\"%s\",color=\"%s\",frontcolor=\"%s\"];\n", id.second, id.first.c_str(), color, frontcolor);
		}


	}

	for (auto cell : des->scope.m_cells) {
		std::vector<std::string> in_ports, out_ports;
		std::vector<std::string> in_label_pieces, out_label_pieces;
		for (auto conn : cell.cell_conns) {
			////getports()///////
			if (conn.second.inout_type == 0)
				in_ports.push_back(conn.second.portname);
			else
				out_ports.push_back(conn.second.portname);
		}
		for (auto& p : in_ports) {
			in_label_pieces.push_back(stringf("<p%d> %s", id2num(p), p.c_str()));
		}

		for (auto& p : out_ports)
			out_label_pieces.push_back(stringf("<p%d> %s", id2num(p), p.c_str()));

		IdString in_label = join_label_pieces(in_label_pieces);
		IdString out_label = join_label_pieces(out_label_pieces);

		std::string label_string = stringf("{{%s}|%s\\n%s|{%s}}", in_label.c_str(),
			cell.name.c_str(), cell.type.c_str(), out_label.c_str());

		fprintf(dotfile, "c%d [ shape=record, label=\"%s\", ];\n",
			id2num(cell.name), label_string.c_str());

		//////////////////get connetction()//////////////////////////
		for (auto conn : cell.cell_conns) {
			const char* color = "Black", * frontcolor = "Black";
			std::string portname = stringf("c%d:p%d", id2num(cell.name), id2num(conn.second.portname));
			std::string net = stringf("n%d", id2num(conn.first));
			if (conn.second.inout_type == 0) //in port
			{
				fprintf(dotfile, "%s:e -> %s:w[color=\"%s\",frontcolor=\"%s\"];\n", net.c_str(), portname.c_str(), color, frontcolor);
			}
			else
				fprintf(dotfile, "%s:e -> %s:w[color=\"%s\",frontcolor=\"%s\"];\n", portname.c_str(), net.c_str(), color, frontcolor);


		}

	}

	// 写入.dot文件的尾部信息
	fprintf(dotfile, "}\n");

	// 关闭.dot文件
	fclose(dotfile);

	printf("Dot file generated successfully.");

	return 0;
}
