/*************************************************************************
	> File Name: csrc/main_nvboard.cpp
	> Author: wangliang
	> Mail: 314821558@qq.com 
	> Created Time: 2023年03月31日 星期五 19时31分00秒
 ************************************************************************/

#include<nvboard.h>
#include<Vlight.h>

//static TOP_NAME dut;

void nvboard_bind_all_pins(Vlight *light);

static void single_cycle(Vlight *light) {
  light->clk = 0; light->eval();
  light->clk = 1; light->eval();
}

static void reset(int n,Vlight *light) {
  light->rst = 1;
  while (n -- > 0) single_cycle(light);
  light->rst = 0;
}

int main(){
	Vlight *light = new Vlight;
	nvboard_bind_all_pins(light);
	nvboard_init();
	
	reset(10,light);

	while(1){
		nvboard_update();
		single_cycle(light);
	}
}
