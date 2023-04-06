#include <stdio.h>
#include <Vbcd7seg.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#define Vtop Vbcd7seg

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
		for(int i = 0;i<4;i++){
			int a = rand() % 4;
			top->x[i] = a;
			printf("x[%d] = %d, ",i,top->x[i]);
		}
		int b = rand() % 4;
		top->y = b;
		top->eval();
		printf("y = %d, f = %d, time = %d\n", b, top->f,sim_time);
		tfp->dump(sim_time);
		sim_time++;
	}
	
	top->final();
	tfp->close();
	delete top;
	delete contextp;

	return 0;
}
