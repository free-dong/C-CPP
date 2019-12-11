// BinarySearch.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

using namespace std;

int BinarySearch(int* a, const int x, const int n);
int BinarySearchR(int* a, const int x, const int low, const int high);

int main()
{
	int m[] = { 1,2,3,4,5,6,7,8,9 };
	int result;
	int num;

	num = 7;
	result = BinarySearch(m, num, 9);

	if (result > 0)
		cout << "binary search: " << num << " in m[" << result << "]" << endl;
	else
		cout << "cont find" << num << endl;

	cout << "递归 折半查找：" << endl;
	result = BinarySearchR(m, num, 0, 9);

	if (result > 0)
		cout << "binary search: " << num << " in m[" << result << "]" << endl;
	else
		cout << "cont find" << num << endl;

	return 0;
}

/*
二分查找（折半查找）：将排序好的数组，依次依次对比中间数据
*/
int BinarySearch(int* a, const int x, const int n)
{
	int low, high, mid;
	low = 0, high = n - 1;

	while (low <= high)
	{
		mid = (low + high) / 2;
		if (a[mid] == x)
			return mid;
		else if (a[mid] < x)
			low = mid + 1;
		else if (a[mid] > x)
			high = mid - 1;
	}

	return -1;
}

/*
二分查找（折半查找）：递归
*/
int BinarySearchR(int* a, const int x, const int low, const int high)
{
	while (low <= high)
	{
		int mid = (low + high) / 2;
		if (a[mid] == x)
			return mid;
		else if (a[mid] < x)
			return BinarySearchR(a, x, mid+1, high);
		else if (a[mid] > x)
			return BinarySearchR(a, x, low, mid-1);
	}

	return -1;
}