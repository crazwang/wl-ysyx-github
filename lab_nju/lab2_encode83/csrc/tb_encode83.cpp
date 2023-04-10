#include <stdio.h>
#include <stdlib.h>
#include <Vencode83.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#define Vtop Vencode83

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
	int b = 0b00000000;

	while(sim_time <= 20 && !contextp->gotFinish()){ //or ' Verilated::gotFinish() '

		top->x = b;
		top->eval();
		printf("x = %o, o_led = %o, time = %d\n", b, top->o_led,sim_time);
		tfp->dump(sim_time);
		sim_time++;
		b++;
	}
	
	top->final();
	tfp->close();
	delete top;
	delete contextp;

	return 0;
}
