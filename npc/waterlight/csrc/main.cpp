/*************************************************************************
	> File Name: csrc/main.cpp
	> Author: wangliang
	> Mail: 314821558@qq.com 
	> Created Time: 2023年03月31日 星期五 18时53分01秒
 ************************************************************************/

#include<stdio.h>
#include<Vlight.h>
#include<verilated.h>
#include<verilated_vcd_c.h>

void single_cycle(Vlight *light){
	light->clk = 0;
	light->eval();
	light->clk = 1;
	light->eval();
}

void reset(int n,Vlight *light){
	light->rst = 1;
	while (n-- > 0) single_cycle(light);
	light->rst = 0;
}

int main(int argc , char **argv , char **env){
	Verilated::commandArgs(argc,argv);
	Verilated::traceEverOn(true);

	Vlight *light = new Vlight;

	VerilatedVcdC * tfp = new VerilatedVcdC;
	light->trace(tfp,0);
	tfp->open("waveform.vcd");

	reset(10,light);
	
	int sim_time = 0;
	while(!Verilated::gotFinish() && sim_time <=500){
		tfp->dump(sim_time);
		printf("the led[15:0] is : %x ,\n",light->led);
		single_cycle(light);
		sim_time++;
	}
	
	light->final();
	tfp->close();
	delete light;

	return 0;
}

