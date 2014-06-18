#include "Praser.h"
using namespace std;
FILE *f;
char buf[4096];
Node *h,*t;
int main(int argc, char *argv[]) {
	f = fopen("./out.txt", "r");
	bool end = 0;int line = 1;
	get(buf,f);
	char *iter = buf;
	while(1){
		switch (*iter) {
			case eof:
				get(buf,f);
				iter = buf;
				break;
			case 0:
				end = 1;
				break;
			default:	
				if(recogenizer(h, t, iter, line) == ERROR){
					cout<<endl;
				}
				break;
		}
		if(end){
			cout<<endl;
			break;
		}
	}
	print_list(h,200);
	Node *tmp = h;
	if(!ll_praser(tmp)){
		cout<<"error in "<< tmp -> l <<" ";
		print_list(tmp,1);
	}
	cout<<"over"<<endl;
	return 0;
}
