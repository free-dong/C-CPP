// SequentialSearch.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

using namespace std;

int SequentailSearch(int* a, const int n, const int x);

int main()
{
	// 从数组m中查找7
	int m[] = {2,4,5,6,1,3,8,9,7};
	int result;

	int num = 5;

	// 顺序查找
	result = SequentailSearch(m, 9, num);

	if (result < 0)
		cout << "cant find: " << num << endl;
	else
	{
		cout << "find:" << num << " in m[" << result << "]" << endl;
	}


	return 0;
}

/*
顺序查找，依次按照数组的下标查找数字。
*/
int SequentailSearch(int* a, const int n, const int x)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (a[i] == x)
			return i;
	}
	if (i == n)
		return -1;
}

