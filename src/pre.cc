#include <iostream>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>
using namespace std;
int main() {
	freopen("in.txt", "r", stdin);
	freopen("out.txt", "w", stdout);
	int state = 0;char c;
	while((c = getchar()) != -1){
		if(state == 0){
			if(c == '/'){
				state = 1;
			} else{
				putchar(c);
			}
		} else if(state == 1){
			if(c == '/'){
				state = 2;
			} else{
				putchar('/');
				putchar(c);
				state = 0;
			}
		} else if(state == 2){
			if(c == '\r' || c == '\n'){
				putchar('\n');
				state = 0;
			}
		}
	}
	return 0;
}
