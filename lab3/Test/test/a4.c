int main() {
 	int x[5], tem, i, j;
	i = 0;
 	while(i < 5) {
 		x[i] = read();
 		i = i + 1;
 	}
 	i = 1;
 	while (i < 5) {
 		j = i;
 		while (j > 0 && x[j-1] > x[j]) {
 			tem = x[j];
 			x[j] = x[j-1];
 			x[j-1] = tem;
 			j = j - 1;
 		}
 		i = i + 1;
 	}
 	i = 0;
 	while (i < 5) {
 		write(x[i]);
 		i = i + 1;
 	}
 	return 0;
}
