#ifndef PRASER_C
#define PRASER_C
#include "Praser.h"
#define ok(i)  if(i) return 1; else return 0
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
int match_not_move(int t, Node *&cur){
	if(cur -> t == t){
		return 1;
	}
	return 0;
}
int decls(Node *&cur){
	
}
int arith(Node *&cur){

}
int stmt(Node *&cur){
	if(match_not_move(DEF, cur)){
		decls(cur);
	} else if(match_not_move(IDENTIFIER, cur)){
		arith(cur);
	} else if(match_not_move(IF, cur)){
		ifstmt(cur);
	} else if(match_not_move(WHILE, cur)){
		whilestmt(cur);
	}
}
int stmts(Node *&cur){
	stmt(Node *&cur);
	stmts(Node *&cur);
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
