# include  <iostream>
# include  <queue>
# include  <cstring>
# include  <list>
# include  <map>
# include  <cstdlib>

# include "BlifElaborate.h"
# include "scheduling.h"

int main(int argc, char* argv[])
{

	// �� Blif �ļ�ת��Ϊ verilog �ļ�
	BlifElaborate myBlif;
	myBlif.BlifElaborateRead();
	myBlif.Blif2Verilog();

	// Scheduling
	Scheduling scheduling(myBlif);
	scheduling.ASAP();
	scheduling.ALAP();
	int number;
	cin >> number;
	scheduling.MR_LCS(number);
	return 0;
}
