#ifndef PRASER_C
#define PRASER_C
#include "Praser.h"
#define ok(i)  if(!(i)) return 0
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
	if(cur != 0){
		if(cur -> t == t){
			cur = cur -> n;
			return 1;
		}
	} 
	return 0;

}
int move(Node *&cur){
	cur = cur -> n;
	return 0;
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
		move(cur);
		ok(match(LL, cur));
		ok(match(MRB, cur));
	}
	if(match_not_move(IDENTIFIER, cur)){
		ok(match(IDENTIFIER, cur));
	} else{
		return 0;
	}
	if(match_not_move(EQU, cur)){
		move(cur);
		if(match_not_move(IDENTIFIER,cur) || match_not_move(DOUBLE, cur) || match_not_move(LL, cur)){
			move(cur);
		}
	}
	ok(match(SEMICOLON,cur));
	cout<<"-----------------decls -> stmt --------"<<endl;
	return 1;
}

int arith(Node *&cur){
	if(match_not_move(LLB, cur)){
		move(cur);
		if(!arith(cur)){
			return 0;
		}
		ok(match(LRB,cur));
		return 1;
	} else if(match_not_move(IDENTIFIER, cur)||match_not_move(DOUBLE,cur) || match_not_move(LL,cur)){
		move(cur);
		if(cur -> t >= ADD && cur -> t <= DIV){
			move(cur);
			return arith(cur);
		}
		return 1;
	} else {
		return 1;
	}
}

int assign(Node *&cur){
	ok(match(IDENTIFIER, cur));
	if(match_not_move(MLB, cur)){
		ok(match(MLB,cur));
		ok(match(LL,cur));
		ok(match(MRB,cur));
	}
	ok(match(EQU, cur));
	cout<<"------------------assign -> arith-----------"<<endl;
	ok(arith(cur));
	ok(match(SEMICOLON,cur));
	cout<<"------------------assign -> stmt------------"<<endl;
	return 1;
}
int  ifstmt(Node *&cur){
	ok(match(IF, cur));
	ok(match(LLB, cur));
	ok(arith(cur));
	ok(match(BOOLOP,cur));
	ok(arith(cur));
	ok(match(LRB, cur)); 
	cout<<"------------------ifstmt -> stmt -----------"<<endl;
	ok(stmt(cur));
	if(match_not_move(ELSE, cur)){
		move(cur);
		ok(stmt(cur));
	}
	return 1;
}
int whilestmt(Node *&cur){
	ok(match(WHILE, cur));
	ok(match(LLB, cur));
	ok(arith(cur));
	ok(match(BOOLOP,cur));
	ok(arith(cur));
	ok(match(LRB, cur)); 
	ok(stmt(cur));
	return 1;
}
int stmt(Node *&cur){
	if(match_not_move(DEF, cur)){
		cout<<"------------------stmt -> decls -----------"<<endl;
		ok(decls(cur));
		return 1;
	} else if(match_not_move(IDENTIFIER, cur)){
		cout<<"------------------stmt -> assign -----------"<<endl;
		ok(assign(cur));
		return 1;
	} else if(match_not_move(IF, cur)){
		cout<<"------------------stmt -> ifstmt ----------"<<endl;
		ok(ifstmt(cur));
		return 1;
	} else if(match_not_move(WHILE, cur)){
		cout<<"------------------stmt -> whilestmt -------"<<endl;
		ok(whilestmt(cur));
		return 1;
	} else if(match_not_move(SEMICOLON, cur)){
		ok(match(SEMICOLON,cur));
		return 1;
	} else if(match_not_move(BLB, cur)){
		ok(block(cur));
		return 1;
	} else {
		return 0;
	}
}

int stmts(Node *&cur){
	if(match(BRB,cur))return 1;
	cout<< "---------------stmts -> stmt ------------"<<endl;
	ok(stmt(cur));
	cout<< "---------------stmt -> stmts ------------"<<endl;
	ok(stmts(cur));
	return 1;
}
int block(Node *&cur){
	cout<<"--------------block------------------"<<endl;
	ok(match(BLB, cur));
	cout<<"--------------block -> stmts --------"<<endl;
	ok(stmts(cur));
	cout<<"--------------stmts -> block --------"<<endl;
	//ok(match(BRB, cur));
	cout<<"--------------block_end--------------"<<endl;
	return 1;
}
int ll_praser(Node *&h){
	ok(block(h));
	if(h != 0){
		return 0;
	}
	return 1;
}

#endif
