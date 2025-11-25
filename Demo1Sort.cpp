#include <iostream>
#include <ctime>
#include<cstdlib>
using namespace std;

//Yeu cau:Xay dung mot menu 4 sort, hoi ngta muon danh sach co bao nhieu phan tu, random gia tri. In ra man hinh qua trinh sort-0.5 de xuat.

void myDelay(int millisecond) {
	clock_t start_time = clock();
	while (clock() < start_time + millisecond);
}


int menu() {
	int chon;
	cout << "=====MENU===== \n";
	cout << "CHON CHUC NANG: \n";
	cout << "1.SHELL SORT. \n";
	cout << "2.COUNTING SORT. \n";
	cout << "3.RADIX SORT. \n";
	cout << "4.BUCKET SORT. \n";
	cin >> chon;
	return chon;
}

void printArr(int* a, int n) {
	for (int i = 0; i < n; i++)
	{
		cout << a[i] << " ";
	}
	cout << endl;
}

void printDelayArr(int* a, int n) {
	printArr(a, n);
	myDelay(300);
}

//Shell sort
void shellSort(int* a, int n) {
	for (int gap = n/2; gap > 0; gap /=2)
	{
		for (int i = 0; i < n; i++)
		{
			int temp = a[i];
			int j;
			for ( j = i; j >= gap && a[j-gap] > temp; j-=gap)
			{
				a[j] = a[j - gap];
			}
			if (j != i)
			{
				a[j] = temp;
				cout << "Chen " << temp << "vao vi tri " << j << ": ";
				printDelayArr(a, n);
			}
		}
	}
}

//tim max
int findMax(int* a, int n) {
	int maxVal = a[0];
	for (int i = 0; i < n; i++)
	{
		if (a[i] > maxVal)
		{
			maxVal = a[i];
		}
	}
	return maxVal;
}

//Counting sort
void countingSort(int* a, int n) {
	if (n == 0)return;
	int maxVal = findMax(a, n);
	
	//Cap phat mang output,count
	int* count = new int[maxVal + 1];
	int* output = new int[n];

	//Reset mang dem
	for (int i = 0; i <= maxVal; i++)
	{
		count[i] = 0;
	}

	//Dem
	for (int i = 0; i < n; i++)
	{
		count[a[i]] ++;
	}


}


int main() {
	cout << menu();
	return 0;
}
