#pragma once
#pragma warning(disable : 4996)
/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenTypeInProjece token, const char *tokenString) {
  switch (token) {
    case IF:
    case THEN:
    case ELSE:
    case END:
    case REPEAT:
    case UNTIL:
    case READ:
    case WRITE:

      /* 新增保留字 */
    case WHILE:
    case DO:
    case FOR:
    case ENDWHILE:
    case ENDDO:
    case TO:
    case DOWNTO:
      fprintf(listing, "reserved word: %s\n", tokenString);
      break;

    case EQ:
      fprintf(listing, "==\n");
      break;
    case ASSIGN:
      fprintf(listing, "=\n");
      break;
    case LT:
      fprintf(listing, "<\n");
      break;
    case LPAREN:
      fprintf(listing, "(\n");
      break;
    case RPAREN:
      fprintf(listing, ")\n");
      break;
    case SEMI:
      fprintf(listing, ";\n");
      break;
    case PLUS:
      fprintf(listing, "+\n");
      break;
    case MINUS:
      fprintf(listing, "-\n");
      break;
    case TIMES:
      fprintf(listing, "*\n");
      break;
    case OVER:
      fprintf(listing, "/\n");
      break;
    case ENDFILE:
      fprintf(listing, "EOF\n");
      break;

      /* 新增特殊符号 */
    case REMAINDER:
      fprintf(listing, "%%\n");
      break;
    case POWER:
      fprintf(listing, "^\n");
      break;
    case GT:
      fprintf(listing, ">\n");
      break;
    case LEQ:
      fprintf(listing, "<=\n");
      break;
    case GEQ:
      fprintf(listing, ">=\n");
      break;
    case NEQ:
      fprintf(listing, "<>\n");
      break;
    case MINUSEQUAL:
      fprintf(listing, "-=\n");
      break;

    case AND:
      fprintf(listing, "and\n");
      break;
    case OR:
      fprintf(listing, "or\n");
      break;
    case NOT:
      fprintf(listing, "not\n");
      break;

    case REGEXEQ:
      fprintf(listing, ":=\n");
      break;
    case REAND:
      fprintf(listing, "&\n");
      break;
    case REOR:
      fprintf(listing, "|\n");
      break;
    case RECLOSURE:
      fprintf(listing, "#\n");
      break;
    case NUM:
      fprintf(listing, "NUM, val= %s\n", tokenString);
      break;
    case ID:
      fprintf(listing, "ID, name= %s\n", tokenString);
      break;
    case ERROR_INP:
      fprintf(listing, "ERROR: %s\n", tokenString);
      break;
    default: /* should never happen */
      fprintf(listing, "Unknown token: %d\n", token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode *newStmtNode(StmtKind kind) {
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else {
    for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode *newExpNode(ExpKind kind) {
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else {
    for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char *copyString(char *s) {
  int n;
  char *t;
  if (s == NULL) return NULL;
  n = strlen(s) + 1;
  t = (char *)malloc(n);
  if (t == NULL)
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  else
    strcpy(t, s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno += 2
#define UNINDENT indentno -= 2

/* printSpaces indents by printing spaces */
static void printSpaces(void) {
  int i;
  for (i = 0; i < indentno; i++) fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode *tree) {
  int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind == StmtK) {
      switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing, "If\n");
          break;
        case RepeatK:
          fprintf(listing, "Repeat\n");
          break;
        case AssignK:
          fprintf(listing, "Assign to: %s\n", tree->attr.name);
          break;
        case ReadK:
          fprintf(listing, "Read: %s\n", tree->attr.name);
          break;
        case WriteK:
          fprintf(listing, "Write\n");
          break;

        /* 新增保留字 */
        case WhileK:
          fprintf(listing, "While\n");
          break;
        case DoWhileK:
          fprintf(listing, "DoWhile\n");
          break;
        case ForK:
          fprintf(listing, "For\n");
          break;
        case MinusEqualK:
          fprintf(listing, "MinusAssign to: %s\n", tree->attr.name);
          break;
        case RegexAssignK:
          fprintf(listing, "RegexAssign to: %s\n", tree->attr.name);
          break;
        default:
          fprintf(listing, "Unknown ExpNode kind\n");
          break;
      }
    } else if (tree->nodekind == ExpK) {
      switch (tree->kind.exp) {
        case OpK:
          fprintf(listing, "Op: ");
          printToken(tree->attr.op, "\0");
          break;
        case ConstK:
          fprintf(listing, "Const: %d\n", tree->attr.val);
          break;
        case IdK:
          fprintf(listing, "Id: %s\n", tree->attr.name);
          break;
        case LetterK:
          fprintf(listing, "Letter: %s\n", tree->attr.name);
          break;
        default:
          fprintf(listing, "Unknown ExpNode kind\n");
          break;
      }
    } else
      fprintf(listing, "Unknown node kind\n");
    for (i = 0; i < MAXCHILDREN; i++) printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}