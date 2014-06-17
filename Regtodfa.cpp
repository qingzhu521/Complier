#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include <map>
using namespace std;
string s;
/**
中缀表达式建树
叶子节点 字母节点和括号节点
进入时要么是空树的 要么在符号右子树上 合法中缀表达式不可能二元运算符在空树时出现
*后置一元运算符 ()都是一类运算符 非二元运算符
*/
struct Node{
	bool leaf;
	char x;//计算节点 叶子节点
	Node *s[2];
	Node(bool i,char b){
		leaf = i;x = b;
		memset(s,0,sizeof(s));
	}
	Node(){
		leaf = 0;
		memset(s,0,sizeof(s));
	}
};

struct GN{
	int n;
	struct Edge *top;
	GN(){
		top = 0;
		n = 0;
	}
	void copy(const GN *in){
		n = in -> n;
		top = in -> top;
	}
};
struct Edge{
	char type;
	GN *d;
	Edge *nxt;
	Edge(char a, GN *b,Edge *n){
		type = a;
		d = b;
		nxt = n;
	}
};
Node *root;
stack<Node*> rootstack;
void join(Node *tmp){
	if(root == 0){
		root = tmp;
	} else{
		if(tmp -> leaf || tmp -> x == 'B'){
			if((root -> x == '|' || root -> x == '.') && root -> s[1] == 0){//2元运算符
				root -> s[1] = tmp;
			} else{
				Node *tmproot = new Node(0,'.');//前面是后置一元运算符或者是叶子
				tmproot -> s[0] = root;
				tmproot -> s[1] = tmp;
				root = tmproot;
			}
		} else if(tmp -> x == '|' || tmp -> x == '*'){//*的加入
			tmp -> s[0] = root;
			root = tmp;
		} 	
	}
}
void check(Node *f){
	if(f -> s[0] != 0){
		check(f -> s[0]);
	}
	cout<<f->x; 
	if(f -> s[1] != 0){
		check(f -> s[1]);
	}
}
void addedge(GN *x1, GN *x2,char type){
	Edge *tmp = new Edge(type, x2, x1 -> top);
 	x1 -> top = tmp;
}
void dfs(Node *f,GN *&head, GN *&tail){
	if(f == 0)return;
	GN *h1,*t1, *h2,*t2;
	if(f -> s[0] != 0){
		dfs(f -> s[0], h1, t1);
	} 
	if(f -> s[1] != 0){
		dfs(f -> s[1], h2, t2);
	}
	if(f -> x == 'B'){
		head = h1,tail = t1;
	} else if(isalpha(f -> x)){
		head = new GN();tail = new GN();
		Edge *tmp = new Edge(f -> x, tail, head -> top);
		head -> top = tmp;
	} else if(f -> x == '|'){
		head = new GN(); tail = new GN();
		addedge(head, h1,'E');
		addedge(head, h2,'E');
		addedge(t1, tail,'E');
		addedge(t2, tail,'E');
	} else if(f ->x == '*'){
		head = new GN();tail = new GN();
		addedge(t1,h1,'E');
		addedge(head,h1,'E');
		addedge(t1,tail,'E');
		addedge(head,tail,'E');
	} else if(f -> x == '.'){
		t1 -> top = h2 -> top;
		delete(h2);
		head = h1,tail = t2;
	} 
}

void bfs(GN *head){
	int cnt = 1;
	queue<GN*> q;
	head ->  n = cnt;cnt++;
	q.push(head);
	while(!q.empty()){
		GN *x = q.front();q.pop();
		if(x == 0)break;
		Edge *iter = x->top;
		for (; iter != 0; iter = iter -> nxt){
			cout<<iter<<" "<<iter -> d<<endl;
			GN *dis = iter -> d;
			if(dis -> n == 0){
				dis -> n = cnt; cnt++;
				q.push(dis);
			}
		}
	}
}
GN *sto[1000];
void bfso(GN *head){
	queue<GN*> q;
	int in[1000] = {0};
	q.push(head);
	in[head -> n] = 1;
	while(!q.empty()){
		GN *x = q.front();q.pop();
		sto[x -> n] = x;
		Edge *iter = x->top;
		for (; iter != 0; iter = iter -> nxt){
			GN *dis = iter -> d;
			cout<<x -> n<<"->"<<dis -> n<<" [label = \""<<iter -> type<<"\"]"<<endl;
			if(in[dis -> n] == 0){
				in[dis -> n] = 1;//控制入口(QwQ)
				q.push(dis);
			}
		}
	}
}

struct State{
	vector<int> s;
	int  at(int i) const{
		return s[i];
	}
	void show(){
		printf("{");
		for (int i = 0; i < s.size() -1; i++){
			printf("%d,", s[i]);
		}
		printf("%d", s[s.size() - 1]);
		printf("}");
	}
	void pushback(int i){
		s.push_back(i);
	}
	unsigned long size() const{
		return s.size();
	}
	void sorts(){
		sort(s.begin(), s.end());
	}
	bool operator < (const State &in) const {
		int minlen = min(s.size(),in.size());
		for (int i = 0; i < minlen; i++){
			if(s[i] < in.at(i)){
				return 1;
			} else if(s[i] > in.at(i)){
				return 0;
			}
		}
		if(s.size() < in.size()){
			return 1;
		} 
		else return 0;
	}
	bool operator == (const State &in) const{
		int minlen = min(s.size(),in.size());
		if(s.size() == in.size()){
			for (int i = 0; i < minlen; i++){
				if(s[i] != in.at(i)){
					return 0;
				}
			}
			return 1;
		}
		return 0;
	}
};
bool exist[1000] = {0};
char syb[1000];
queue<State> q;
map<State,int> judge;
State move(const State &s, char f){
	State st;
	for (int i = 0; i < s.size(); i++){
		GN *x = sto[s.at(i)];
		Edge *it = x -> top;
		for (; it != 0; it = it -> nxt){
			if(it -> type == f){
				GN *ard = it -> d;
				st.pushback(ard -> n);
			}
		}
	}
	return st;
}
State eclosure(const State &s){
	int hasin[1000] = {0};
	State x;
	queue<int> bq;
	for (int i = 0; i < s.size(); i++){
		bq.push(s.at(i));
		x.pushback(s.at(i));
		hasin[s.at(i)] = 1;
	}
	while(!bq.empty()){
		GN *f = sto[bq.front()];
		bq.pop();
		Edge *iter = f -> top;
		for (; iter != 0; iter = iter -> nxt){
			if(iter -> type == 'E'){
				GN *arr = iter -> d;
				if(!hasin[arr -> n]){
					hasin[arr -> n] = 1;
					bq.push(arr -> n);
					x.pushback(arr -> n);
				}
			}
		}
	}
	x.sorts();
	return x;
}

void convertnfa2dfa(GN *head){
	int cnt = 0;
	for(char i = 'a'; i <= 'z'; i++){
		if(exist[i]){
			syb[cnt++] = i;
		}
	}
	int scnt = 0;
	State init;
	init.pushback(1);
	State s0 = eclosure(init);
	q.push(s0);
	judge.insert(pair<State,int>(s0,scnt++));
	while(!q.empty()){
		State x = q.front(); q.pop();
		for (int i = 0; i < cnt; i++){
			State y = eclosure(move(x,syb[i]));
			if(judge.find(y) == judge.end()){
				judge.insert(pair<State,int>(y,scnt++));
				q.push(y);
			}
			x.show();
			cout<<" s"<<judge[x]<<" ";
			cout<<syb[i]<<" ";
			y.show();
			cout<<" s"<<judge[y]<<endl;
		}
		cout<<1<<endl;
	}
}

int main(int argc, char *argv[]) {
	cin>>s;
	int len = s.size();
	for (int i = 0; i < len; i++){
		exist[s[i]] = 1;
	 	if(s[i] == '('){
			Node *tmproot = new Node(1,'B');
			join(tmproot);
			rootstack.push(root);
			root = 0;
		} else if(s[i] == ')'){
			Node *tmproot = rootstack.top();
			rootstack.pop();
			//不是在在这个上就是在右子树上
			if(tmproot -> x == 'B'){
				tmproot -> s[0] = root;
				root = tmproot;
			} else{
				Node *r = tmproot->s[1];
				r -> s[0] = root;
				root = tmproot;
			}
		} else {
			join(new Node(isalpha(s[i]),s[i]));
		}
	}
	check(root);
	cout<<endl;
	GN *head = 0,*tail = 0;
	dfs(root,head,tail);
	cout<<head<<" "<<tail<<endl;
	bfs(head);
	bfso(head);
	convertnfa2dfa(head);
	return 0;
}
