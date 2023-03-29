#include <stdio.h>
#include <Vtop.h>
#include <verilated.h>
#include <verilated_vcd_c.h>


int main(int argc, char ** argv,char ** env) {
	//写法1:
	//Verilated::commandArgs(argc,argv);
	//Verilated::traceEverOn(true);
	//Vtop * top = new Vtop("top");
	//
	//写法2:
	VerilatedContext * contextp = new VerilatedContext;
	contextp->commandArgs(argc,argv);
	contextp->traceEverOn(true);
	Vtop * top = new Vtop{contextp};

	VerilatedVcdC * tfp = new VerilatedVcdC;
	top->trace(tfp,0);
	tfp->open("waveform.vcd");

	int sim_time = 0;
	while(sim_time <= 20 && !contextp->gotFinish()){ //or ' Verilated::gotFinish() '
		int a = rand() & 1;
		int b = rand() & 2;
		top->a = a;
		top->b = b;
		top->eval();
		printf("a = %d, b = %d, f = %d,time = %d\n", a, b, top->f,sim_time);
		tfp->dump(sim_time);
		sim_time++;
	}
	
	top->final();
	tfp->close();
	delete top;
	delete contextp;

	return 0;
}
