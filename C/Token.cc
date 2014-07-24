#ifndef TOKEN_CC
#define TOKEN_CC
#include "Token.h"
#include <stdlib.h>
using namespace std;
using namespace __gnu_cxx;
void get(char *buf,FILE *f){
	fread(buf, 1, 4095, f);//0 - 4094
	buf[4095] = eof;//4095
}

void add_to_list(Node *&h, Node *&t, void *x,int p,int line){
	if(h == 0){
		h = t = new Node(p, line, x);
	} else{
		t -> n = new Node(p, line, x);
		t = t -> n;
	}
}

int recogenizer(Node *&h, Node *&t, char *&iter, int &line){
	//cout<<*iter<<endl;
	if(isdigit(*iter)){
		char s[60];int w = 0;
		while(isdigit(*iter)){
			s[w] = *iter;
			w++,iter++;
		}
		if(isalpha(*iter) && (*iter != 'E') && (*iter != 'e')){
			cout<<1<<endl;
			return ERROR;
		} else if(*iter == 'E' || *iter == 'e'){
			iter++;
			if(*iter == '+' || *iter == '-'){
				s[w] = *iter;
				w++, iter++;
			} 
			if(!isdigit(*iter)){
				return ERROR;
			} else{
				while(isdigit(*iter)){
					s[w] = *iter;
					w++;iter++;
				}
			}
			s[w] = 0;
			double *x = new double(atof(s));
			add_to_list(h, t, x, DOUBLE, line);
		} else if(*iter =='.'){
			iter++;
			while(isdigit(*iter)){
				s[w] = *iter;
				w++;iter++;
			}
			s[w] = 0;
			double *x = new double(atof(s));
			add_to_list(h, t, x, DOUBLE, line);
		} else{
			s[w] = 0;
			long long *x = new long long(atoll(s));
			add_to_list(h, t, x, LL, line);
		}
	} else if(isalpha(*iter) || *iter == '_'){
		char *s = new char[30];
		int w = 0;s[w] = *iter;
		w++;iter++;
		while(isalpha(*iter) || isdigit(*iter) || *iter == '_'){
			s[w++] = *iter;
			iter++;
		}
		s[w] = 0;
		if(!strcmp(s, "if")){
			add_to_list(h, t, s, IF, line); 
		} else if(!strcmp(s, "while")){
			add_to_list(h, t, s, WHILE, line);
		} else if(!strcmp(s, "else")){
			add_to_list(h, t, s, ELSE, line);
		} else if(!strcmp(s, "int")){
			add_to_list(h, t, s, DEF, line);
		} else if(!strcmp(s, "double")){
			add_to_list(h, t, s, DEF, line);
		} else{
			add_to_list(h, t, s, IDENTIFIER, line);
		}
	} else if(*iter == '+'){
		add_to_list(h, t, NULL, ADD, line);
		iter++;
	} else if(*iter == '-'){
		add_to_list(h, t, NULL, SUB, line);
		iter++;
	} else if(*iter == '*'){
		add_to_list(h, t, NULL, MUL, line);
		iter++;
	} else if(*iter == '/'){
		add_to_list(h, t, NULL, DIV, line);
		iter++;
	} else if(*iter == '='){
		iter++;
		if(*iter != '='){
			add_to_list(h, t, NULL, EQU, line);
		} else{
			char *x = new char[3];
			x[0] = '='; x[1] = '='; x[2] = 0;
			add_to_list(h, t, x, BOOLOP, line);
			iter++;
		}
	} else if(*iter == '<'){
		iter++;
		if(*iter != '='){
			char *x = new char[3];
			x[0] = '='; x[1] = 0; x[2] = 0;	
			add_to_list(h, t, x, BOOLOP, line);
		} else{
			char *x = new char[3];
			x[0] = '<'; x[1] = '='; x[2] = 0;
			add_to_list(h, t, x, BOOLOP, line);			iter++;
		}
	} else if(*iter == '>'){
		iter++;
		if(*iter != '='){
			char *x = new char[3];
			x[0] = '>'; x[1] = 0; x[2] = 0;
			add_to_list(h, t, x, BOOLOP, line);		} else{
			char *x = new char[3];
			x[0] = '>'; x[1] = '='; x[2] = 0;
			add_to_list(h, t, x, BOOLOP, line);			iter++;
		}
	} else if(*iter == '!'){
		iter++;
		if(*iter != '='){
			return ERROR;
		} else{
			char *x = new char[3];
			x[0] = '!'; x[1] = '='; x[2] = 0;
			add_to_list(h, t, x, BOOLOP, line);
		}
	} else if(*iter =='(' || *iter == ')' || *iter == '{' || *iter == '}' || *iter == '[' || *iter == ']'){
		if(*iter =='('){
			add_to_list(h, t, NULL, LLB, line);
		} else if(*iter == ')'){
			add_to_list(h, t, NULL, LRB, line);
		} else if(*iter == '{'){
			add_to_list(h, t, NULL, BLB, line);
		} else if(*iter == '}'){
			add_to_list(h, t, NULL, BRB, line);
		} else if(*iter == '['){
			add_to_list(h, t, NULL, MLB, line);
		} else{
			add_to_list(h, t, NULL, MRB, line);
		}
		iter++;
	} else if(*iter == '\r' || *iter == '\n'){
		iter++,line++;
	} else if(*iter == ';'){
		add_to_list(h,t,NULL,SEMICOLON, line);
		iter++;
	}else{
		iter++;
	}
	return 0;
}

void print_list(Node *h, int num){
	Node *iter = h;int i;
	for (iter = h, i = 1; iter != 0 && (num > 100 ? 1:i <= num); iter = iter ->n ,i++){
		switch(iter -> t){
			case DOUBLE:cout<<*(double*)iter -> d;break;
			case LL:cout<<*(long long *)iter -> d;break;
			case ADD:cout<<'+';break;
			case SUB:cout<<'-';break;
			case MUL:cout<<'*';break;
			case DIV:cout<<'/';break;
			case LLB:cout<<'(';break;
			case LRB:cout<<')';break;
			case BLB:cout<<'{'<<endl;break;
			case BRB:cout<<'}'<<endl;break;
			case MLB:cout<<'[';break;
			case MRB:cout<<']';break;
			case EQU:cout<<'=';break;
			case SEMICOLON:cout<<';'<<endl;break;
			default:{
				cout<<(char *)iter -> d;
				break;
			}
		}
		cout<<" ";
	}
}

#endif
