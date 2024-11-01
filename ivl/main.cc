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

	// 将 Blif 文件转化为 verilog 文件
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
