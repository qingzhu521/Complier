#include "Token.h"
//#include "Praser.h:
using namespace std;
FILE *f;
char buf[4096];
Node *h,*t;
int main(int argc, char *argv[]) {
	f = fopen("C:\\Users\\Skullpirate\\Documents\\GitHub\\Complier\\src\\out.txt", "r");
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
				recogenizer(h, t, iter, line);
		}
		if(end){
			cout<<endl;
			break;
		}
	}
	print_list(h);
	return 0;
}