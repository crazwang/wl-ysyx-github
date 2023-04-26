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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char* enable;
  char str[32];
  word_t result;
  int hit ;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char *args){
  if(free_ == NULL){
    printf("The watchpoint_pool is already full. \n");
    return NULL;
  }

  bool *suc = NULL;
  suc =(bool *)malloc(sizeof(bool));
  word_t res = expr(args,suc);

  WP *tmp1 = free_;
  if(*suc){
    free_ = free_->next;
    
    if(head == NULL){
      head = tmp1;
      //printf("Head  \n");
    }
    else {
      WP* tmp2 = head;
      while(tmp2->next){
        tmp2 = tmp2->next;
      }
      tmp2->next = tmp1;
    } 
    strcpy(tmp1->str,args);
    tmp1->result = res;
    tmp1->enable = "y";
    tmp1->hit = 0;
    printf("Successfully set a new wathchpoint !\n");
    printf("No\tEnb\tExpr\t\tRes\t\tHit\n");
    printf("%d\t%s\t%s\t\t%ld\t\t%d\n",tmp1->NO ,tmp1->enable ,tmp1->str,tmp1->result,tmp1->hit);
    tmp1->next = NULL;
    return tmp1;
  }
  else{
    return NULL;
  }

}

bool free_wp(WP *wp){
  if(wp == NULL){
    printf("The watchpoint you want to delete is NULL !\n");
    return false;
  }
  if(head == wp){
    head = head->next;
  }
  else{
    WP* tmp1 = head;
    while (tmp1->next){
      if(tmp1->next == wp){
        break;
      }
      tmp1 = tmp1 ->next;
    }
    tmp1->next = wp ->next;
  }
  wp->next = free_;
  free_ = wp;
  return true;
}

bool delete_wp(char *args){
  int num = atoi(args);
  WP *tmp = head;
  while(tmp){
    if(tmp->NO == num){
      break;
    }
    tmp = tmp->next;
  }
  if(tmp == NULL){
    printf("The watchpoint %d doesn't exit in watchpoint_pool !\n",num);
    return false;
  }
  else{
    return free_wp(tmp);
  }
}

bool enable_wp(char *args){
  int num = atoi(args);
  WP *tmp = head;
  while(tmp){
    if(tmp->NO == num){
      break;
    }
    tmp = tmp->next;
  }
  if(tmp == NULL){
    printf("The watchpoint %d doesn't exit in watchpoint_pool !\n",num);
    return false;
  }
  else{
    tmp->enable = "y";
    return true;
  }
}

bool disable_wp(char *args){
  int num = atoi(args);
  WP *tmp = head;
  while(tmp){
    if(tmp->NO == num){
      break;
    }
    tmp = tmp->next;
  }
  if(tmp == NULL){
    printf("The watchpoint %d doesn't exit in watchpoint_pool !\n",num);
    return false;
  }
  else{
    tmp->enable = "n";
    return true;
  }
}


void compare_wp(){
  WP *tmp = head;
  word_t res;
  bool trig = false;
  while(tmp){
    if(strcmp(tmp->enable,"y") == 0){
      bool *suc = NULL;
      suc =(bool *)malloc(sizeof(bool));
      res = expr(tmp->str,suc);
      if(res != tmp->result){
        if(!trig){
          printf("Triggered watchpoint, triggered list is: \n");
          printf("No\tEnb\tExpr\t\tHit\n");
          trig = true;
        }
        tmp->hit ++;
        printf("%d\t%s\t%s\t\t%d\n",tmp->NO ,tmp->enable ,tmp->str,tmp->hit);
        printf ("Old value is %ld .\n",tmp->result);
        printf("New value is %ld .\n",res);
        tmp->result = res;
        nemu_state.state = NEMU_STOP;
      }
    }
    tmp=tmp->next;
  }
}

void display_wp(){
  if(head ==NULL){
    printf("No watchpoint has been set!\n");
  }
  else{
    WP *tmp = head;
    printf("No\tEnb\tExpr\t\tRes\t\tHit\n");
    while(tmp){
      printf("%d\t%s\t%s\t\t%ld\t\t%d\n",tmp->NO ,tmp->enable ,tmp->str,tmp->result,tmp->hit);
      tmp=tmp->next;
    }
  }
}

void single_display_wp(char *args){
  int num = atoi(args);
  WP *tmp = head;
  while(tmp){
    if(tmp->NO == num){
      break;
    }
    tmp = tmp->next;
  }
  if(tmp == NULL){
    printf("The watchpoint %d doesn't exit in watchpoint_pool !\n",num);
  }
  else{
    printf("No\tEnb\tExpr\t\tRes\t\tHit\n");
    printf("%d\t%s\t%s\t\t%ld\t\t%d\n",tmp->NO ,tmp->enable ,tmp->str,tmp->result,tmp->hit);
  }
}