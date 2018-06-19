struct Student{
 	int name;
 	int grade;
};

struct Class{
 	struct Student students[50];
 	int average;
};

int calculate(struct Class class) {
 	int sum = 0, i = 0, N = 50;
 	while (i < N) {
 		sum = sum + class.students[i].grade;
 		i = i + 1;
 	}
 	class.average = sum / N;
 	return sum / N;
}

int main() {
 	struct Class school[10];
	int i1 = 0, j1 = 0, N1 = 50, N2 = 10;
 	while (i1 < N2) {
 		j1 = 0;
 		while (j1 < N1) {
 			school[i1].students[j1].grade = i1 + j1 * 5;
			j1 = j1 + 1;
		}
		i1 = i1 + 1;
 	}
 	j1 = 0;
	i1 = 0;
	 while (i1 < N2) {
 		j1 = j1 + calculate(school[i1]);
 		i1 = i1 + 1;
 	}
 	write(j1);
 	return 0;
}
