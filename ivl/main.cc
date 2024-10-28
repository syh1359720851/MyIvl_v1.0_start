# include  <iostream>
# include  <queue>
# include  <cstring>
# include  <list>
# include  <map>
# include  <cstdlib>

# include "BlifElaborate.h"

int main(int argc, char* argv[])
{

	// 将 Blif 文件转化为 verilog 文件
	BlifElaborate myBlif;
	myBlif.BlifElaborateRead();
	myBlif.Blif2Verilog();

	return 0;
}
