int swap(int a, int b) {
 	int tem = a;
 	a = b;
 	b = tem;
 	write(a);
 	write(b);
 	return a;
}

int main() {
	int x[5];
	int i = 0;
 	while (i < 5) {
 		x[i] = read();
 		if (i > 0) {
 			swap(x[i-1],x[i]);
 		}
		i = i + 1;
	}
 	return 0;
}
