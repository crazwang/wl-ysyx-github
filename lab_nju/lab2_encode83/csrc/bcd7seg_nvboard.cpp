#include<stdio.h>
#include<Vbcd7seg.h>
#include<verilated.h>
#include<nvboard.h>

#define Vtop Vbcd7seg

static TOP_NAME dut;

void nvboard_bind_all_pins(Vtop* top);

int main(int argc,char **argv, char ** env){
	nvboard_bind_all_pins(&dut);
	nvboard_init();
	
	int num=0;
	
	while(1){
		dut.eval();
		nvboard_update();
		//num++;
		//printf("the num is %d\n",num);
	}
	
	nvboard_quit();
	return 0;
}
