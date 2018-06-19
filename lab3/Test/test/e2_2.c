int qsort(int array[10], int l, int r) {
 	int x = array[l], a = l, b = r;
	if (a < b) {
 		while (a < b) {
 			while (a < b && array[b] > x) {b = b - 1;}
 			if (a < b) {array[a] = array[b]; a = a + 1;}
 			while (a < b && array[a] < x) {a = a + 1;}
 			if (a < b) {array[b] = array[a]; b = b - 1;}
 		}
 		array[a] = x;
 		qsort(array,l,a-1);
		qsort(array,a+1,r);
 	}
 	return 0;
}

int main() {
 	int number[10], N = 10, i = 0;
 	while (i < N) {
 		number[i] = read();
		i = i + 1;
 	}
 	qsort(number,0,N-1);
 	i = 0;
 	while (i < N) {
		write(number[i]);
		i = i + 1;
 	}
 	return 0;
}
