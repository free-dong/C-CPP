//
#ifndef __TRACK_H__
#define __TRACK_H__
#include <set>
#include <queue>
#include <time.h>
#include <math.h>
#include <vector>
#include <ostream>
#include <fstream>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <darknet.h>
#include <improcess.h>
#include "opencv2/core/core.hpp"
#include "loadargs.h"
#include "Hungarian.h"
#include "toolfuncs.h"
#include "KalmanTracker.h"

#define CNUM 20
using namespace std;

extern int total_frames;
extern double total_time;

typedef struct TrackingBox
{
	int frame;
	int id;
	Rect_<float> box;
}TrackingBox;

typedef struct Center
{
	int id;
	float x;
	float y;
	Center(void)
	{
		id = -1;
		x = 0;
		y = 0;
	}
}Center;

typedef struct Postframe
{
	Mat frame;
	string time;
	string frametime;
	string picname;
}Postframe;

typedef struct Speed {
	int id;
	float x;
	float y;
	float sum;
	Speed(void)
	{
		id = -1;
		x = 0;
		y = 0;
		sum = 0;
	}
}Speed;

typedef struct Stop {
	int id;
	int num;
	bool inside;
	Stop(void)
	{
		id = -1;
		num = 0;
		inside = 0;
	}
}Stop;

typedef struct ClimbFrame{
	int id;
	cv::Mat frame;
	string h;
	string m;
	string s;
}ClimbFrame;

typedef struct myQueue {
	queue<int> q;
	set<int> s;
	void push(int x) {
		if (s.count(x)) return; //如果存在就不入队
		q.push(x); s.insert(x); //入队，加入集合
	}
	int front(void) {
		assert(!q.empty()); //如果队空就报错
		return q.front();
	}
	int back(void) {
		assert(!q.empty());
		return q.back();
	}
	int size(void) {
		return q.size();
	}
	bool operator ==(const myQueue v) {
		queue<int> a = this->q;
		queue<int> b = v.q;
		return a == b;
	}
	bool exist(int x) {
		if (s.count(x)) return 1;
	}
	void pop() {
		assert(!q.empty());
		int h = q.front(); //取出队头
		s.erase(h); //从集合中删除
		q.pop(); //从队列中删除
	}
	bool empty() {
		return q.empty();
	}
}myQueue;

typedef struct timer
{
	int h;
	int m;
	int s;
	timer(void)
	{
		h = 0;
		m = 0;
		s = 0;
	}
	void add(int xs)
	{
		this->s = this->s + xs;
		if (this->s >= 60)
		{
			this->m = this->m + this->s / 60;
			this->s = this->s % 60;
			if (this->m >= 60)
			{
				this->h = this->h + this->m / 60;
				this->m = this->m % 60;
				if (this->h >= 24)
					this->h = this->h % 24;
			}
		}
	}
}timer;



int TTrack(arguments args);

#endif
