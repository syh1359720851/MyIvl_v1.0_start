# include  <iostream>
# include  <queue>
# include  <cstring>
# include  <list>
# include  <map>
# include  <cstdlib>

# include "BlifElaborate.h"

int main(int argc, char* argv[])
{

	// �� Blif �ļ�ת��Ϊ verilog �ļ�
	BlifElaborate myBlif;
	myBlif.BlifElaborateRead();
	myBlif.Blif2Verilog();

	return 0;
}
