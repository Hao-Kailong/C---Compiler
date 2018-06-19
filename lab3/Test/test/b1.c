int myPow(int x, int n) {
 	int value;
 	if (n == 0){ return 1;}
 	if (n == 1){ return x;}
 	if (n == 2){ return x * x;}
 	if (n == (n/2*2)){ return myPow(myPow(x,n/2),2);}
 	else {
 		value = myPow(myPow(x,n/2),2);
 		return x*value;}
}

int main() {
 	int x1, n1;
 	x1 = read();
 	n1 = read();
 	write(myPow(x1,n1));
 	return 0;
}
