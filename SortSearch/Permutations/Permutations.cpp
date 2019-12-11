// Permutations.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。


#include <iostream>

using namespace std;

// 输出字符串的所有的排列和组合
void Permutations(char* p, const int k, const int m)
{
	if (k == m)
	{
		for (int i = 0; i <= m; i++)
			cout << p[i];
		cout << endl;
	}
	else
	{
		for (int i = k; i <= m; i++)
		{
			swap(p[k], p[i]);
			Permutations(p, k + 1, m);
			swap(p[k], p[i]);
		}
	}
}

int main()
{
	char s[] = "abc";
	Permutations(s, 0, 2);
	system("pause");
}

