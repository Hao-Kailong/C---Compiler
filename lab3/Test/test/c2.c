int trap() {
 	int lh = 0, lIndex = 0, i = 0, count = 0, thisPool = 0;
 	int n = 12;
 	int rh = 0;
 	int height[12];
	while (i < n) {
	 	height[i] = read();
 		i = i + 1;
 	}
 	i = 0;
 	while (i < n && height[i] == 0) {
 		i = i + 1;
 	}
 	if (i >= n) {
 		return 0;
 	}
 	lh = height[i];
 	lIndex = i;
 	thisPool = 0;
 	i = i + 1;
 	while (i < n) {
 		if (height[i] < lh){
 			thisPool = thisPool + (lh - height[i]);
 		}else {
 			count = count + thisPool;
 			lh = height[i];
 			lIndex = i;
 			thisPool = 0;
 		}
 		i = i + 1;
 	}
 	thisPool = 0;
 	rh = 0;
 	i = n - 1;
 	while (i > lIndex && height[i] == 0) {
 		i = i - 1;
 	}
 	rh = height[i];
 	i = i - 1;
 	while (i > lIndex) {
 		if (height[i] < rh) {
 			thisPool = thisPool + (rh - height[i]);
 		} else {
 			count = count+ thisPool;
			rh = height[i];
 			thisPool = 0;
 		}
 		i = i - 1;
 	}
 	return count + thisPool;
}

int main (){
 	int result = trap();
 	write(result);
 	return 0;
}
