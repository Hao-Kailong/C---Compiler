struct Product{
 	int type;
 	int name;
};

int main(){
 	struct Product cola[10];
 	int i, j, add, N = 10;
 	i = 0;
 	while(i < N) {
 		cola[i].type = 10;
 		cola[i].name = i;
		i = i + 1;
 	}
 	i = 0;
 	add = 0;
 	while (i < N) {
 		j = 0;
		add = add + cola[i].name;
		while (j < N) {
 			cola[i].type = cola[i].type + add * cola[j].
			name;
 			j = j + 1;
 		}
 		i = i + 1;
	}
 	write(cola[N-1].type);
 	return 0;
}
