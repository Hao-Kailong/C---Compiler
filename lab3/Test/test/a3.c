int main(){
 	int x, n, result;
	x = read();
 	n = read();
 	if (n == (n / 2 * 2)) {
 		result = 1;
 	} else {
 		result = x;
 	}
 	n = n / 2;
 	while (n > 0) {
 		x = x * x;
 		if (n != (n / 2 * 2)) {
 			result = result * x;
 		}
 		n = n / 2;
 	}
 	write(result);
 	return 0;
}
