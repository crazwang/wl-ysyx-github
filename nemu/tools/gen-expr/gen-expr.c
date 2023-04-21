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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  long int a = %s; "
"  unsigned result = (unsigned)a; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


static int count = 0;
static int retry = 0;

void gen(char c){
  if(count < sizeof(buf)-1){
    buf[count] = c;
    count ++;
  }
  else{
    //printf("The express length is too long ,bigger than size of buf. Try again. \n");
    count ++;
  }
}

static int choose(int n){
  return rand()%n;
}

void gen_num(){
  int a = choose(6)+1;
  char tmp;
  if(a == 1){
    tmp = '0' + rand() % 9;
    gen(tmp);
  }
  else{
    tmp = '1' + rand() % 9;
    gen(tmp);
    for(int i = 1;i<a;i++){
      tmp = '0' + rand() % 10;
      gen(tmp);
    }  
  }
}

void gen_rand_op(){
  switch (choose(4)){
    case 0 : gen('+'); break;
    case 1 : gen('-'); break;
    case 2 : gen('/'); break;
    default: gen('*'); break;
  }
}

void gen_rand_expr() {
  //buf[0] = '\0';
  //int tmp = 0;
  switch (choose(3)) {
    case 0:   if(count >= sizeof(buf)){break;} else {gen_num(); break;}  
    case 1:   if(count >= sizeof(buf)){break;} else {gen('('); gen_rand_expr(); gen(')'); break;} 
    default:  if(count >= sizeof(buf)){break;} else {gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;}
  }
  if(count >= sizeof(buf)){
    retry = 1;
  }
  else{
    //printf("count is %d ,size of buf = %ld\n" ,count,sizeof(buf));
    retry = 0;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    retry = 1;
    while(retry){
      count = 0;
      gen_rand_expr();
      buf[count]='\0';
    }
    
    //printf("express count is %d ,  ",count);
    //printf("buf: %s ,\n" ,buf);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr -Werror=overflow -Werror=div-by-zero");
    if (ret != 0) {
      //assert(0);
      //printf("ret: %d, In expr.c line127: overflow wrong.\n\n\n",ret);
      continue;
    }


    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if(fscanf(fp, "%d", &result)!=EOF){
        pclose(fp);   
    }
    else{
      //assert(0);
      //printf("In expr.c line137: fscanf() wrong.\n");
      continue ;
    }
    
    //printf("ret: %d , loop: %d ,RESULT:%u ,EXPR is: %s\n", ret,i,result, buf);
    printf("%u %s\n",result, buf);
  }
  return 0;
}
