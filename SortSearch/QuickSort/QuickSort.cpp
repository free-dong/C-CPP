#include<iostream>
using namespace std;

// https://blog.csdn.net/helloworld_ptt/article/details/86170077
void quick_sort(int array[], int start, int last)
{
	int i = start;
	int j = last;
	int temp = array[i];
	if (i < j)
	{
		while (i < j)
		{
			while (i < j && array[j] >= temp)
				j--;
			if (i < j)
			{
				array[i] = array[j];
				i++;
			}

			while (i < j && temp > array[i])
				i++;
			if (i < j)
			{
				array[j] = array[i];
				j--;
			}

		}
		//把基准数放到i位置
		array[i] = temp;
		quick_sort(array, start, i - 1);
		quick_sort(array, i + 1, last);
	}
}

int main()
{
	int array[] = { 12,345,545,123,45,10,8,23,1,21 };
	int len = sizeof(array) / sizeof(int);
	cout << "The orginal array are:" << endl;
	for (int i = 0; i < len; ++i)
	{
		cout << array[i] << " ";
	}
	cout << endl;
	quick_sort(array, 0, len - 1);
	cout << "The sorted array are:" << endl;
	for (int i = 0; i < len; ++i)
	{
		cout << array[i] << " ";
	}
	cout << endl;
	return 0;
}
