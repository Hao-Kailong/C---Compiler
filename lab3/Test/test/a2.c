int main() {
	int dollar, rmb;
	dollar = read();
	rmb = read();
	if (dollar > 100) {
		if (rmb < 50) {
			write(rmb + dollar);
		} else {
			write(rmb - dollar);
		}
	} else if (dollar == 100) {
		if (rmb < 100) {
			write(rmb);
		} else {
			write(rmb - 100);
		}
	} else if (dollar < 100) {
		if (rmb + dollar > 100){
			write(dollar + 100);
		} else {
			write(100);
		}
	}
	write(dollar + rmb);
	return 0;
}
