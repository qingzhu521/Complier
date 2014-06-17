#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>

const int eof = -1;
const int ERROR = -1;
const int DOUBLE = 1;
const int LL  = 2;
const int IF = 3;
const int WHILE = 4;
const int ELSE = 5;
const int BOOLOP = 6;
const int EQU = 7;//assign
const int ADD = 8;
const int SUB = 9;
const int MUL = 10;
const int DIV = 11;
const int LLB = 12;
const int LRB = 13;
const int BLB = 14;
const int BRB = 15;
const int IDENTIFIER = 16; //identifier
const int DEF = 18
const int SEMICOLON = 17;
const int MLB = 20;
const int MRB = 21;

void get(char *buf,FILE *f);
const char key[100][10] ={
	"const",
	"if",
	"while",
	"inline",
	"operator",
	"return",
	"sizeof",
	"void",
	"int",
	"float"
};

struct Node{
	int t; //type ---- tag
	int l; //line
	void *d;//date
	Node *n;//next
	Node(){
		t = l = 0;
		n = 0;
		d = 0;
	}
	Node (int it, int il, void *id){
		t = it;
		l = il;
		d = id;
		n = 0;
	}
};

int recogenizer(Node *&h, Node *&t, char *&iter, int &line);
void print_list(Node *h);
#endif