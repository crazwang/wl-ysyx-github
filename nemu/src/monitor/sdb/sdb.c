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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <sys/stat.h>

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_help(char *args);

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
  //int n = atoi(args);
  //printf("Step number: int :%d ,char : %s \n",n,args);
  if(args){
    cpu_exec(atoi(args));
  }
  else{
    cpu_exec(1);
  }
  return 0;
}

void display_wp();
void single_display_wp(char *args);

static int cmd_info(char *args){
  if(args == NULL){
    return 0;
  }

  if(strcmp(args,"r") == 0){
    isa_reg_display();
  }
  else if (strcmp(args,"w") == 0)
  {
    display_wp();
  }
  else if(args[0] == '$')
  {
    char name[32];
    strcpy(name,args+1);
    bool *suc = NULL;
    suc =(bool *)malloc(sizeof(bool));
    word_t reg_val=isa_reg_str2val(name,suc);
    if(*suc){
      printf("The value of register %s is 0x%08lx .\n",args,reg_val);
    }
    else{
      printf("The register %s doesn't exist !\n",args);
    }
  }
  else{
    single_display_wp(args);
  }

  return 0;
}

word_t paddr_read(paddr_t addr, int len);
uint8_t* guest_to_host(paddr_t paddr);

static int cmd_x(char *args){
  if(args == NULL){
    return 0;
  }

  char *num_char = strtok(NULL, " ");
  int num = atoi(num_char);
  char *addr_char = num_char + strlen(num_char) + 1;
  paddr_t addr;
  sscanf(addr_char,"%x",&addr);
  //printf("num : %d , addr : %s , int :%x\n",num,addr_char,addr);
  printf("Addr_guest\tAddr_host\t\tValue\n");
  for(int i=0;i<=num;i++){
    printf("0x%x\t%p\t\t%08lx\n",addr+4*i,guest_to_host(addr+4*i),paddr_read(addr+4*i,4));
  }
  return 0;
}

static int cmd_p(char *args){
  if(args == NULL){
    return 0;
  }

  bool *suc = NULL;
  suc =(bool *)malloc(sizeof(bool));
  word_t result = expr(args,suc);
  printf("expr: %s . result: %lu .\n",args,result);
  return 0;
}

static int cmd_pfile(char *args){
  if(args == NULL){
    return 0;
  }

  FILE *fp=fopen(args,"r");
  bool *suc = NULL;
  suc =(bool *)malloc(sizeof(bool));
  int cnt_all = 0;
  int cnt_right = 0;
  if(!fp){
    printf("Can't open file: %s\n" ,args);
    return 0;
  }
  else{
    //struct stat sb;
    //stat(args, &sb);
    //char *express = malloc(sb.st_size);
    while(!feof(fp)){
      char express[65536];
      if(fgets(express,65536,fp)){
        express[strlen(express) - 1] = '\0';
        char *result_char = strtok(express, " ");
        word_t result_ref = atoi(result_char);
        char *express_char = result_char + strlen(result_char) + 1;
        word_t result = expr(express_char,suc);
        //printf("expr: %s . result: %u . result_ref: %u \n",express_char,result,result_ref);
        if(result == result_ref){
          cnt_all ++;
          cnt_right++;
        }
        else{
          cnt_all++;
          //printf("expr: %s , result: %u , result_ref: %u , cnt_all: %d \n",express_char,result,result_ref,cnt_all);
        }
      }
      else{
        printf("The file evaluation completely done or fgets() wrong.\n");
      }
      //sscanf(addr_char,"%x",&addr);
    }
    printf("The total accuracy is %.2f%% .\n",(double)cnt_right/cnt_all*100);
  }
  fclose(fp);
  return 0;
}

typedef struct watchpoint WP;
WP* new_wp(char *args);

static int cmd_w(char *args){
  if(args == NULL){
    return 0;
  }

  if(new_wp(args) == NULL){
    printf("The watchpoint set fail!\n");
  }
  return 0;
}

bool delete_wp(char *args);

static int cmd_d(char *args){
  if(args == NULL){
    return 0;
  }

  if (delete_wp(args)){
    printf("Delete watchpoint %s successfully !\n" ,args);
  }
  return 0;
}

bool enable_wp(char *args);

static int cmd_enable(char *args){
  if(args == NULL){
    return 0;
  }

  if (enable_wp(args)){
    printf("The watchpoint %s is set to be enabled !\n",args);
  }

  return 0;
}

bool disable_wp(char *args);

static int cmd_disable(char *args){
  if(args == NULL){
    return 0;
  }

  if (disable_wp(args)){
    printf("The watchpoint %s is set to be disabled !\n",args);
  }

  return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step in with the speed of n", cmd_si},
  { "info", "Print the state of register", cmd_info},
  { "x", "Scan memory from base to base+n", cmd_x},
  { "p", "Simple expression evaluation" ,cmd_p},
  { "pfile", "Evaluate the expression from file" ,cmd_pfile},
  { "w", "Set a watchpoint" , cmd_w},
  { "d", "Delete a watchpoint", cmd_d},
  { "enable", "Enable a watchpoint", cmd_enable},
  { "disable", "Disable a watchpoint", cmd_disable},
  

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
