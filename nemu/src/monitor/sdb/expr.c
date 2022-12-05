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
  TK_NOTYPE = 256, TK_EQ,TK_NUMBER_10,TK_NUMBER_16

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"\\(", '('},
  {"\\)", ')'},
  {"0[xX][0-9a-fA-F]{1,}", TK_NUMBER_16},
  {"\\b[0-9]{1,}\\b", TK_NUMBER_10},
  

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

//__attribute__((used))这个函数属性通知编译器在目标文件中保留一个静态函数，即使它没有被引用。
static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:break;
          default: 
            int real_len = substr_len < 32 ? substr_len : 32;
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, real_len);
            nr_token++;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

bool check_parentheses(int p, int q) {
  int i;
  int count = 0;//括号计数，左括号+1,右括号就-1
  if(tokens[p].type!='(' || tokens[q].type != ')') return false; //没有被括号包围
  for (i=p; i<=q; i++) {
    if(tokens[i].type == '(') count++;
    else if(tokens[i].type == ')') count--;
    if(count==0 && i<q) return false;
    if(count<0) return false;
  }
  return count==0;
}

int op_rank(int op) {
  switch (op)
  {
  case '/':
  case '*':
    return 3;
  case '+':
  case '-':
    return 4;
  case TK_EQ:
   return 7;
  default:
    return 0;
  }
}

int find_primary_operator(int p, int q) {
  int i ;
  int res = p;

  int pnum = 0;
  int rank,low_rank = 0;

  for(i = p; i <= q; i++) {
    if(tokens[i].type == '(') {
      pnum++;
      continue;
    }
    if(tokens[i].type == ')') {
      pnum--;
      continue;
    }

    if(pnum == 0 ) {
      rank = op_rank(tokens[i].type);
      if(rank >= low_rank) {
        res = i;
        low_rank = rank;
      }
    }
  }
  return res;

}

word_t eval(int p, int q) {
  if(p > q) {
    assert(0);
    return 0;
  }else if(p == q) {
    word_t val = 0;
    if(tokens[p].type == TK_NUMBER_10){
      sscanf(tokens[p].str,"%lu",&val);
    }else if(tokens[p].type == TK_NUMBER_16) {
      sscanf(tokens[p].str,"%lx",&val);
    }else {}
    return val;
  }else if(check_parentheses(p,q)==true) {
    return eval(p+1,q-1);
  }else{
    int op = find_primary_operator(p, q);
    word_t val1,val2;
    val1 = eval(p, op-1);
    val2 = eval(op+1, q);

    switch(tokens[op].type) {
      case '+' : return val1 + val2;break;
      case '-' : return val1 - val2;break;
      case '*' : return val1 * val2;break;
      case '/' : return val1 / val2;break;
      default :assert(0);
    }
  }
  
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */
  // TODO();

  return eval(0, nr_token-1);
}
