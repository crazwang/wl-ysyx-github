/*************************************************************************
	> File Name: main_nvboard.cpp
	> Author: wangliang
	> Mail: 314821558@qq.com 
	> Created Time: 2023年03月31日 星期五 10时59分46秒
 ************************************************************************/

#include<stdio.h>
#include<Vtop.h>
#include<verilated.h>
#include<nvboard.h>

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
	return 0;
}
