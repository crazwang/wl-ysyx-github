/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

paddr_t host_to_guest(uint8_t *haddr);

void isa_reg_display() {
  printf("Name\tAddr_guest\tAddr_host\t\tValue\n");
  for(int i = 0;i < 32;i++){
    printf("%s\t0x%x\t%p\t\t0x%08lx\n",regs[i], host_to_guest(( unsigned char *)&cpu.gpr[i]), &cpu.gpr[i],cpu.gpr[i]);
  }
}

word_t isa_reg_str2val(const char *args, bool *success) {
  if(strcmp(args,"pc") == 0){
    *success =true;
    return cpu.pc;
  }
  int i = 0;
  for(i = 0 ;i < 32;i++){
    if(strcmp(args,regs[i]) == 0) {
      break;
    }
  }
  if(i == 32){
    *success = false;
    return 0;
  }
  else{
    *success = true;
    return cpu.gpr[i];
  }
}
