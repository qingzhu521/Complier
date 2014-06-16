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
int decls(Node *&cur){
	ok(match(IDENTIFIER, cur));
	if(match(MLB,cur)){
		
	} else{
		
	}
}
int stmt(Node *&cur){
	
}
int stmts(Node *&cur){
	stmt(Node *&cur);
	stmts(Node *&cur);
	return 1;
}
int block(Node *&cur){
	cout<<"--------------block------------------";
	ok(match(BLB, cur));
	cout<<"--------------block -> decls --------";
	ok(decls(cur));
	cout<<"--------------decls -> block --------";
	ok(match(BRB, cur));
}
int ll_praser(Node *h){
	block(h);
	return 0;
}

#endif
