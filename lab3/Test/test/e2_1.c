int main(){
 	int dis[10][5][3];
 	int d1 = 10, d2 = 5, d3 = 3;
 	int i = 0, j = 0, k = 0, sum = 0;
 	while (i < d1) {
 		j = 0;
 		while (j < d2) {
 			k = 0;
			while (k < d3) {
 				dis[i][j][k] = i * d1 + j * d2 + d3 /
				(k+1);
 				k = k + 1;
 			}
 			j = j + 1;
		}
 		i = i + 1;
 	}
 	i = 0;
 	j = 0;
 	k = 0;
 	while (i < d1) {
		j = 0;
 		while (j < d2) {
 			k = 0;
 			while (k < d3) {
 				if (dis[i][j][k] > dis[0][0][0]) {
 					sum = sum + dis[i][j][k];
 				}
				k = k + 1;
 			}
 			j = j + 1;
		}
 		i = i + 1;
 	}
 	write(sum);
 	return 0;
}
