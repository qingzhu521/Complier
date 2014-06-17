#ifndef PRASER_C
#define PRASER_C
#include "Praser.h"
#define ok(i)  if(!i) return 0
using namespace std;

/*
switch(curstatus){
	case DECLEAR:{
		if(cur.in == ){
			curstatus = ;
		} else{
			
		}
	}
}
*/
int match(int t,Node *&cur){
	if(cur -> t == t){
		cur = cur -> n;
		return 1;
	}
	return 0;
}
void move(Node *&cur){
	cur = cur -> n;
}
int match_not_move(int t, Node *&cur){
	if(cur -> t == t){
		return 1;
	}
	return 0;
}
int decls(Node *&cur){
	ok(match(DEF, cur));
	if(match_not_move(MLB, cur)){
		ok(match(MLB, cur));
		ok(match(LL, cur));
		ok(match(MRB, cur));
	}
	if(match_not_move(IDENTIFIER, cur)){
		ok(match(IDENTIFIER, cur));
	} else{
		return 0;
	}

}

int arith(Node *&cur){
	if(match_not_move(LLB, cur)){
		move();
		arith(cur);
		ok(match(LRB, cur));
	} else if(match_not_move(IDENTIFIER, cur)){
		move();
		if(cur -> t > 7 && cur -> t < 12){
			move();
		}
		arith();
	} else if(match_not_move(LRB,cur)){
		move();
		return 1;
	} else if(match_not_move(SEMICOLON, cur)){
		move();
		return 1;
	}
}

int assign(Node *&cur){
	ok(match(IDENTIFIER, cur));
	if(match_not_move(MLB, cur)){
		match(MLB,cur);
		match(LL,cur);
		match(MRB,cur);
	}
	ok(match(EQU, cur));
	cout<<"------------------assign -> arith-----------"<<endl;
	ok(arith(cur));
}
int  ifstmt(Node *&cur){
	match(IF, cur);
	match(LLB, cur);
	arith(cur);
	match(BOOLOP,cur);
	arith(cur);
	match(LRB, cur); 
	stmt(cur);
	if(match_not_move(else)){
		move();
		stmt();
	}
}
int whilestmt(Node *&cur){
	match(WHILE, cur);
	match(LLB, cur);
	arith(cur);
	match(BOOLOP,cur);
	arith(cur);
	match(LRB, cur); 
	stmt(cur);
}
int stmt(Node *&cur){
	if(match_not_move(DEF, cur)){
		cout<<"------------------stmt -> decls -----------"<<endl;
		decls(cur);
	} else if(match_not_move(IDENTIFIER, cur)){
		cout<<"------------------stmt -> arith -----------"<<endl;
		assign(cur);
	} else if(match_not_move(IF, cur)){
		cout<<"------------------stmt -> ifstmt ----------"<<endl;
		ifstmt(cur);
	} else if(match_not_move(WHILE, cur)){
		cout<<"------------------stmt -> whilestmt -------"<<endl;
		whilestmt(cur);
	} else if(match_not_move(SEMICOLON, cur)){
		match(SEMICOLON,cur);
	} else if(match_not_move(BLB, cur)){
		ok(block(cur));
	}
}
int stmts(Node *&cur){
	if(match(BRB,cur))return 1;
	ok(stmt(Node *&cur));
	ok(stmts(Node *&cur));
	return 1;
}
int block(Node *&cur){
	cout<<"--------------block------------------";
	ok(match(BLB, cur));
	cout<<"--------------block -> stmts --------";
	ok(stmts(cur));
	cout<<"--------------stmts -> block --------";
	ok(match(BRB, cur));
}
int ll_praser(Node *h){
	block(h);
	return 0;
}

#endif
