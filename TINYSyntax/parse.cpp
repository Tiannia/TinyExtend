/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "stdafx.h"
#include "globals.h"
#include "parse.h"
#include "scan.h"
#include "util.h"
#include <iostream>

static TokenTypeInProjece token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode* stmt_sequence(void);
static TreeNode* statement(void);
static TreeNode* if_stmt(void);
static TreeNode* repeat_stmt(void);
static TreeNode* assign_stmt(void);
static TreeNode* read_stmt(void);
static TreeNode* write_stmt(void);
static TreeNode* exp(void);
static TreeNode* simple_exp(void);
static TreeNode* term(void);
static TreeNode* factor(void);

/* 扩充 */
static TreeNode* power_term(void);
static TreeNode* While_stmt(void);
static TreeNode* Dowhile_stmt(void);
static TreeNode* for_stmt(void);
static TreeNode* logicExp(void);
static TreeNode* logic_term(void);
static TreeNode* logic_factor(void);

static TreeNode* RegexExp(void);
static TreeNode* regex_term(void);
static TreeNode* regex_factor(void);
static TreeNode* regex_letter(void);


static void syntaxError(char* message) {
  fprintf(listing, "\n>>> ");
  fprintf(listing, "Syntax error at line %d: %s", lineno, message);
  Error = TRUE;
}

static void match(TokenTypeInProjece expected) {
  if (token == expected)
    token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    fprintf(listing, "      ");
  }
}

TreeNode* stmt_sequence(void) {
  TreeNode* t = statement();
  TreeNode* p = t;
  while ((token != ENDFILE) && (token != END) && (token != ELSE) &&
         (token != UNTIL) && (token != ENDDO) && (token != ENDWHILE) && (token != WHILE)) {
    TreeNode* q;
    match(SEMI);
    q = statement();
    if (q != NULL) {
      if (t == NULL)
        t = p = q;
      else /* now p cannot be NULL either */
      {
        p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode* statement(void) {
  TreeNode* t = NULL;
  switch (token) {
    case IF:
      t = if_stmt();
      break;
    case REPEAT:
      t = repeat_stmt();
      break;
    case ID:
      t = assign_stmt();
      break;
    case READ:
      t = read_stmt();
      break;
    case WRITE:
      t = write_stmt();
      break;

    /* 扩充的文法 */
    case WHILE:
      t = While_stmt();
      break;
    case DO:
      t = Dowhile_stmt();
      break;
    case FOR:
      t = for_stmt();
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token, tokenString);
      token = getToken();
      break;
  } /* end case */
  return t;
}

TreeNode* if_stmt(void) {
  TreeNode* t = newStmtNode(IfK);
  match(IF);
  if (t != NULL) {
    t->child[0] = logicExp();
  }
  match(THEN);
  if (t != NULL) t->child[1] = stmt_sequence();
  if (token == ELSE) {
    match(ELSE);
    if (t != NULL) t->child[2] = stmt_sequence();
  }
  match(END);
  return t;
}

TreeNode* repeat_stmt(void) {
  TreeNode* t = newStmtNode(RepeatK);
  match(REPEAT);
  if (t != NULL) t->child[0] = stmt_sequence();
  match(UNTIL);
  if (t != NULL) t->child[1] = logicExp();
  return t;
}

TreeNode* assign_stmt(void) {
  TreeNode* t = newStmtNode(AssignK);
  if ((t != NULL) && (token == ID)) t->attr.name = copyString(tokenString);
  match(ID);
  if (token == ASSIGN) {  //赋值
    match(ASSIGN);
    if (t != NULL) {
      t->child[0] = exp();
    }
  }
  else if (token == MINUSEQUAL) {  //-=
    if (t != NULL) {
      t->kind.stmt = MinusEqualK;
      match(MINUSEQUAL);
      t->child[0] = exp();
    }
  } else if (token == REGEXEQ) {
    if (t != NULL) {
      t->kind.stmt = RegexAssignK;
      match(REGEXEQ);
      t->child[0] = RegexExp();
    }
  }
  return t;
}

TreeNode* read_stmt(void) {
  TreeNode* t = newStmtNode(ReadK);
  match(READ);
  if ((t != NULL) && (token == ID)) t->attr.name = copyString(tokenString);
  match(ID);
  return t;
}

TreeNode* write_stmt(void) {
  TreeNode* t = newStmtNode(WriteK);
  match(WRITE);
  if (t != NULL) t->child[0] = exp();
  return t;
}

TreeNode* exp(void) {
  TreeNode* t = simple_exp();
  if ((token == LT) || (token == EQ) || (token == GT) || (token == GEQ) ||
      (token == LEQ) || (token == NEQ)) {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t != NULL) t->child[1] = simple_exp();
  }
  return t;
}

TreeNode* RegexExp(void) {
  TreeNode* t = regex_term();
  while (token == REOR) {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = regex_term();
    }
  }
  return t;
}

TreeNode* regex_term(void) {
  TreeNode* t = regex_factor();
  while (token == REAND) {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = regex_factor();
    }
  }
  return t;
}

TreeNode* regex_factor(void) {
  TreeNode* t = regex_letter();
  if (token == RECLOSURE) {
    TreeNode* p = newExpNode(OpK);
    if ((p != NULL) && (token == RECLOSURE)) {
      p->attr.op = RECLOSURE;
      match(RECLOSURE);
      p->child[0] = t;
      t = p;
    }
  }
  return t;
}

TreeNode* regex_letter(void) {
    TreeNode* t = NULL;
    switch (token) { 
    case LPAREN:
        match(LPAREN);
        t = RegexExp();
        match(RPAREN);
        break;
    case ID:
    case EPSILON:
      t = newExpNode(LetterK);
      if (t != NULL) t->attr.name = copyString(tokenString);
      //std::cout << t->attr.name << std::endl;
      match(token);
      break;
    }
    return t;
}

TreeNode* logicExp(void) {
  TreeNode* t = logic_term();
  while (token == OR) {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = logic_term();
    }
  }
  return t;
}

TreeNode* logic_term(void) {
  TreeNode* t = logic_factor();
  while (token == AND) {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = logic_factor();
    }
  }
  return t;
}

TreeNode* logic_factor(void) {
  TreeNode* t = NULL;
  switch (token) { 
  case LPAREN:
    match(LPAREN);
    t = logicExp();
    match(RPAREN);
    break;
  case NOT:
    t = newExpNode(OpK);
    if ((t != NULL) && (token == NOT)) {
      t->attr.op = NOT;
      match(NOT);
      t->child[0] = logic_factor();
    }
    break;
  default:
    t = exp();
    break;
  }
  return t;
}

TreeNode* simple_exp(void) {
  TreeNode* t = term();
  while ((token == PLUS) || (token == MINUS)) {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode* term(void) {
  TreeNode* t = power_term();
  while ((token == TIMES) || (token == OVER) ||
         (token == REMAINDER))  // REMAINDER是求余
  {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = power_term();
    }
  }
  return t;
}

/* 新增处理乘方 */
TreeNode* power_term(void) {
  TreeNode* t = factor();
  while (token == POWER)  // ^
  {
    TreeNode* p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode* factor(void) {
  TreeNode* t = NULL;
  switch (token) {
    case NUM:
      t = newExpNode(ConstK);
      if ((t != NULL) && (token == NUM)) t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID:
      t = newExpNode(IdK);
      if ((t != NULL) && (token == ID)) t->attr.name = copyString(tokenString);
      match(ID);
      break;
    case LPAREN:
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token, tokenString);
      token = getToken();
      break;
  }
  return t;
}

//扩充While文法
TreeNode* While_stmt(void) {
  TreeNode* t = newStmtNode(WhileK);
  match(WHILE);
  if (t != NULL) t->child[0] = logicExp();
  match(DO);
  if (t != NULL) t->child[1] = stmt_sequence();
  match(ENDWHILE);
  return t;
}

//扩充Dowhile文法
TreeNode* Dowhile_stmt(void) {
  TreeNode* t = newStmtNode(DoWhileK);
  match(DO);
  if (t != NULL) t->child[0] = stmt_sequence();
  match(WHILE);
  //match(LPAREN);
  if (t != NULL) t->child[1] = logicExp();
  //match(RPAREN);
  return t;
}

//扩充for文法
TreeNode* for_stmt(void) {
  TreeNode* t = newStmtNode(ForK);
  match(FOR);
  if (t != NULL) t->child[0] = assign_stmt();
  if (token == TO) {
    match(TO);
    if (t != NULL) t->child[1] = simple_exp();
  } else {
    match(DOWNTO);
    if (t != NULL) t->child[1] = simple_exp();
  }
  match(DO);
  if (t != NULL) t->child[2] = stmt_sequence();
  match(ENDDO);
  return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode* parse(void) {
  TreeNode* t;
  token = getToken();
  t = stmt_sequence();
  if (token != ENDFILE) syntaxError("Code ends before file\n");
  return t;
}
