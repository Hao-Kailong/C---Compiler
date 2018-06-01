int func(int a[3][4])
{
	a[1][1]=5;
	return 0;
}

int main()
{
	int arr[3][4];
	arr[1][1]=7;
	write(arr[1][1]);
	func(arr);
	write(arr[1][1]);
	return 0;
}
