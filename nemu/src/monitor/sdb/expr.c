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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, NUM, NUM_HEX, TK_NEQ, AND_LOGIC, OR_LOGIC
  ,TK_REG
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},                // spaces
  {"(0x|0X)[0-9a-fA-F]+",NUM_HEX},  // hex number
  {"[0-9]+", NUM},                  // digit number
  {"\\$(\\$0|pc|ra|s10|s11|[s,g,t]p|t[0-6]|s[0-9]|a[0-7])",TK_REG},
  {"[(]", '('},                     // left brace
  {"[)]", ')'},                     // right brace
  {"\\+", '+'},                     // plus
  {"-", '-'},                       // minus
  {"\\*", '*'},                     // muti
  {"/", '/'},                       // divide
  {"==", TK_EQ},                    // equal  
  {"!=", TK_NEQ},                   // not equal
  {"&&", AND_LOGIC},                // logic and 
  {"\\|\\|", OR_LOGIC},                 // logic or
  
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  char *priority_1 = "1";   // "(",")",
  char *priority_2 = "2";   // "*" pointer,
  char *priority_3 = "3";   // "*","\",
  char *priority_4 = "4";   // "+","-",

  char *priority_7 = "7";   // "==","!=",

  char *priority_11 = "91"; // "&&",
  char *priority_12 = "92"; // "||",
  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE : break;
          case NUM       : tokens[nr_token].type = rules[i].token_type ; strncpy(tokens[nr_token].str,substr_start,substr_len);tokens[nr_token].str[substr_len] = '\0'; nr_token++; break;
          case NUM_HEX   : tokens[nr_token].type = rules[i].token_type ; strncpy(tokens[nr_token].str,substr_start,substr_len);tokens[nr_token].str[substr_len] = '\0'; nr_token++; break;
          case TK_REG    : tokens[nr_token].type = rules[i].token_type ; strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);tokens[nr_token].str[substr_len-1] = '\0'; nr_token++; break; //delete "$"
          case '('       : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_1); nr_token++; break;
          case ')'       : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_1); nr_token++; break;
          case '*'       : tokens[nr_token].type = rules[i].token_type ; 
                           if(nr_token==0 || tokens[nr_token-1].type != NUM){strcpy(tokens[nr_token].str,priority_2);}  // pointer "*"
                           else {strcpy(tokens[nr_token].str,priority_3);} nr_token++; break;                           // multiple "*"
          case '/'       : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_3); nr_token++; break;  
          case '+'       : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_4); nr_token++; break;
          case '-'       : tokens[nr_token].type = rules[i].token_type ; 
                           if(nr_token==0 || tokens[nr_token-1].type != NUM){strcpy(tokens[nr_token].str,priority_2);}  // negative "-"
                           else {strcpy(tokens[nr_token].str,priority_4);} nr_token++; break;                           // minus    "-" 
          case TK_EQ     : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_7); nr_token++; break;
          case TK_NEQ    : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_7); nr_token++; break;
          case AND_LOGIC : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_11);nr_token++; break;
          case OR_LOGIC  : tokens[nr_token].type = rules[i].token_type ; strcpy(tokens[nr_token].str,priority_12);nr_token++; break;
          default        : break;//TODO();
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
    //printf("tokens[%d].type is : %d ,tokens[%d].str is %s\n",nr_token,tokens[nr_token-1].type, nr_token,tokens[nr_token-1].str);
  }

  return true;
}

int check_legal(int p,int q){
  int bracepair = 0;
  int i = p;
  while(bracepair >= 0 && i <= q){
    if(tokens[i].type == '('){
      bracepair++;
    }
    else if(tokens[i].type == ')'){
      bracepair--;
    }
    i++;
  }
  return bracepair;
}

bool check_parentheses(int p,int q){
  if(tokens[p].type == '(' && tokens[q].type == ')'){
    if(check_legal(p+1,q-1)==0){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    return false;
  }
}

word_t paddr_read(paddr_t addr, int len);

word_t eval(int p, int q) {
  if (p > q) {
    /* Bad expression */
    printf("Bad expression! The index p :%d > index q :%d !\n",p,q);
    assert(0);
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    if(tokens[p].type == TK_REG){
      bool *suc =NULL;
      suc =(bool *)malloc(sizeof(bool));
      return isa_reg_str2val(tokens[p].str,suc) ;
    }
    else if(tokens[p].type == NUM_HEX){
      word_t result;
      sscanf(tokens[p].str,"%lx",&result);
      return result;
    }
    else {
    return strtol(tokens[p].str,NULL,10);}
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */

    return eval(p + 1, q - 1);
  }
  else {
    //op = the position of 主运算符 in the token expression;
    int op=0;
    int bracepair = 0;
    for (int i=q;i>=p;i--){
      if(bracepair == 0){
        switch (tokens[i].type)
        {
        case ')': bracepair ++; break;
        case '+': if(!op){op = i;}else if(strcmp(tokens[op].str,tokens[i].str)<0){op = i;} break;
        case '-': if(!op){op = i;}else if(strcmp(tokens[op].str,tokens[i].str)<0){op = i;} break;
        case '/': if(!op){op = i;}else if(strcmp(tokens[op].str,tokens[i].str)<0){op = i;} break;
        case '*': if(!op){op = i;}else if(strcmp(tokens[op].str,tokens[i].str)<0){op = i;} break;
        default : break;  
        }
      }
      else{
        switch (tokens[i].type)
        {
        case ')': bracepair ++; break;
        case '(': bracepair --; break;
        default : break;
        }
      }
    }
    //printf("op : %d \n",op);
    if(strcmp(tokens[op].str,"2") == 0){
      word_t val2 = eval(op + 1, q);
      switch (tokens[op].type) {
        case '-': return  - val2;
        case '*': char buf[32]; sprintf(buf,"%lx",val2); paddr_t addr; sscanf(buf,"%x",&addr); return paddr_read(addr,4);
        default : printf("Operation does not exist!\n"); assert(0);
      }
    }
    else{
      word_t val1 = eval(p, op - 1);
      word_t val2 = eval(op + 1, q);

      switch (tokens[op].type) {
        case '+':       return val1 + val2;
        case '-':       return val1 - val2;
        case '*':       return val1 * val2;
        case '/':       return val1 / val2;
        case TK_EQ:     return val1 == val2;
        case TK_NEQ:    return val1 != val2;
        case AND_LOGIC: return val1 && val2;
        case OR_LOGIC:  return val1 || val2;
        default: printf("Operation does not exist!\n"); assert(0);
      }
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  int p = 0;
  int q = nr_token - 1;

  if(check_legal(p ,q) >0 ){
    printf("The express is ILLEGAL !\n");
    printf("'()'don't match . Missing token:')' !\n");
    printf("The result is set to 0 !\n");
    *success = false;
    return 0;
  }
  else if (check_legal(p ,q) >0 )
  {
    printf("The express is ILLEGAL !\n");
    printf("'()'don't match . Missing token:'(' !\n");
    printf("The result is set to 0 !\n");
    *success = false;
    return 0;
  }
  else{
    //printf ("%ld\n" ,eval(p,q));
    *success = true;
    return eval(p,q);
  }
/*   if(result){
    *success = true;
  } */
  //printf("EXPR: %s . RESULT: %u .\n",e,result);
}

