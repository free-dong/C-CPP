#include "toolfuncs.h"

using namespace std;
using namespace cv;

set<int>flags;
bool across(int id)
{
	for (auto it = flags.begin(); it != flags.end();)
	{
		if ((*it) == id)
			return true;
		it++;
	}
	return false;
}

/* int loadJsonArgs(arguments* args, crow::json::rvalue x)
{
	string videoname = x["video name"].s();
	if (!videoname.empty())
		args->seq = videoname;

	string videopath = x["videopath"].s();
	if (!videopath.empty())
		args->videopath = videopath;

	string httpd_ip = x["httpd_ip"].s();
	if (!httpd_ip.empty())
		args->httpd_ip = httpd_ip;

	string httpd_port = x["httpd_port"].s();
	if (!httpd_port.empty())
		args->httpd_port = httpd_port;

	string threshold = x["threshold"].s();
	if (!threshold.empty())
		args->threshold = atoi(threshold.c_str());

	string thre_flag = x["thre_flag"].s();
	if (!thre_flag.empty())
		args->thre_flag = atoi(thre_flag.c_str());

	string display = x["display"].s();
	if (!display.empty())
		args->display = atoi(display.c_str());

	string point_a = x["point_a"].s();
	if (!point_a.empty())
		args->point_a = point_a;

	string point_b = x["point_b"].s();
	if (!point_b.empty())
		args->point_b = point_b;

	string frame_skip = x["frame_skip"].s();
	if (!frame_skip.empty())
		args->frame_skip = atoi(frame_skip.c_str());

	string pt_interval = x["pt_interval"].s();
	if (!pt_interval.empty())
		args->pt_interval = pt_interval;

	string videofps = x["videofps"].s();
	if (!videofps.empty())
		args->videofps = atoi(videofps.c_str());
} */

int encrypt(string path, string outputpath)
{
	ifstream classNamesFile(path);//标签文件coco有80类
	ofstream ofs;
	ofs.open(outputpath, ios::out);
	string key = "1";
	vector<string>classNamesVec;
	vector<string>encnames;
	if (classNamesFile.is_open()) {
		string className = "";
		//int i = 1;
		while (getline(classNamesFile, className))
		{
			std::string encryptedMsg = encrypt_vigenere(className, key);
			encnames.push_back(encryptedMsg);
			ofs << encryptedMsg << endl;
			//if (i % 10000 == 1)
			//{
			//	cout << "No." << i << "'th line" << endl;
			//	//cout << "encrypted line:" << encryptedMsg << endl;
			//}
			//i++;
		}
	}
	classNamesFile.close();
	ofs.close();
}
int decrypt(string path, string outputpath)
{

	ifstream cnf(path);
	ofstream ofs;
	ofs.open(outputpath, ios::out);
	string key = "1";
	if (cnf.is_open()) {
		string line = "";
		//int i = 1;
		while (getline(cnf, line))
		{
			string newkey = extend_key(line, key);
			string decryptedMsg = decrypt_vigenere(line, newkey);
			ofs << decryptedMsg << endl;
			//if (i % 10000 == 1)
			//{
			//	cout << "No." << i << "'th line" << endl;
			//	//cout << "decrypted line:" << decryptedMsg << endl;
			//}
			//i++;
		}
	}
	cnf.close();
	ofs.close();

}
// Computes IOU between two bounding boxes
double GetIOU(Rect_<float> bb_test, Rect_<float> bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}

float colors[6][3] = { {1,0,1}, {0,0,1},{0,1,1},{0,1,0},{1,1,0},{1,0,0} };
float get_color(int c, int x, int max) {
	float ratio = ((float)x / max) * 5;
	int i = floor(ratio);
	int j = ceil(ratio);
	ratio -= i;
	float r = (1 - ratio) * colors[i][c] + ratio * colors[j][c];
	return r;
}

Points inside(Point C, Point A, Point B, int d)
{
	bool flag1 = 0;
	bool flag2 = 0;
	bool flag3 = 0;
	bool flag4 = 0;
	Point Aout;
	Point Bout;
	Point Cout;
	Point Dout;
	if ((A.x != B.x) && (A.y != B.y))
	{
		double k1 = ((double)(A.y - B.y) / (double)(A.x - B.x));
		double k = -1 / k1;
		double b_1 = A.y - k * A.x;
		double b_2 = B.y - k * B.x;
		double a = k * k + 1;
		double b1 = (-2) * (A.x + k * (A.y - b_1));
		double b2 = (-2) * (B.x + k * (B.y - b_2));
		double c1 = A.x * A.x + (A.y - b_1) * (A.y - b_1) - d * d;
		double c2 = B.x * B.x + (B.y - b_2) * (B.y - b_2) - d * d;
		double delta1 = b1 * b1 - 4 * a * c1;
		double delta2 = b2 * b2 - 4 * a * c2;
		if (delta1 > 0)
		{
			double x1 = (-1 * b1 + sqrt(delta1)) / (2 * a);
			double y1 = k * x1 + b_1;
			double x2 = (-1 * b1 - sqrt(delta1)) / (2 * a);
			double y2 = k * x2 + b_1;
			Aout.x = x1; Aout.y = y1;
			Bout.x = x2; Bout.y = y2;
		}
		else if (delta1 = 0)
		{
			double x1, x2;
			x1 = -1 * b1 / (2 * a);
			x2 = x1;
			double y1 = k * x1 + b_1;
			double y2 = k * x2 + b_1;
			Aout.x = x1; Aout.y = y1;
			Bout.x = x2; Bout.y = y2;
		}
		else
		{
			cout << "NULL" << endl;
			Aout.x = 0; Aout.y = 0;
			Bout.x = 0; Bout.y = 0;
		}
		if (delta2 > 0)
		{
			double x3 = (-1 * b2 + sqrt(delta2)) / (2 * a);
			double y3 = k * x3 + b_2;
			double x4 = (-1 * b2 - sqrt(delta2)) / (2 * a);
			double y4 = k * x4 + b_2;
			Cout.x = x3; Cout.y = y3;
			Dout.x = x4; Dout.y = y4;
		}
		else if (delta2 = 0)
		{
			double x3, x4;
			x3 = -1 * b2 / (2 * a);
			x4 = x3;
			double y3 = k * x3 + b_1;
			double y4 = k * x4 + b_1;
			Cout.x = x3; Cout.y = y3;
			Dout.x = x4; Dout.y = y4;
		}
		else
		{
			cout << "NULL" << endl;
			Cout.x = 0; Cout.y = 0;
			Dout.x = 0; Dout.y = 0;
		}
		double b_3 = Aout.y - k1 * Aout.x;
		double b_4 = Bout.y - k1 * Bout.x;
		//line 1:AoutBout
		if ((k * B.x - B.y + b_1) > 0)
		{
			if ((k * C.x - C.y + b_1) > 0)
				flag1 = 1;
			else
				flag1 = 0;
		}
		else
		{
			if ((k * C.x - C.y + b_1) < 0)
				flag1 = 1;
			else
				flag1 = 0;
		}
		//line 2:CoutDout
		if ((k * A.x - A.y + b_2) > 0)
		{
			if ((k * C.x - C.y + b_2) > 0)
				flag2 = 1;
			else
				flag2 = 0;
		}
		else
		{
			if ((k * C.x - C.y + b_2) < 0)
				flag2 = 1;
			else
				flag2 = 0;
		}
		//line 3:AoutCout
		if ((k1 * Bout.x - Bout.y + b_3) > 0)
		{
			if ((k1 * C.x - C.y + b_3) > 0)
				flag3 = 1;
			else
				flag3 = 0;
		}
		else
		{
			if ((k1 * C.x - C.y + b_3) < 0)
				flag3 = 1;
			else
				flag3 = 0;
		}
		//line 4:BoutDout
		if ((k1 * Aout.x - Aout.y + b_4) > 0)
		{
			if ((k1 * C.x - C.y + b_4) > 0)
				flag4 = 1;
			else
				flag4 = 0;
		}
		else
		{
			if ((k1 * C.x - C.y + b_4) < 0)
				flag4 = 1;
			else
				flag4 = 0;
		}
	}
	else if (A.x == B.x)
	{
		Aout.x = A.x - d; Aout.y = A.y;
		Bout.x = A.x + d; Bout.y = A.y;
		Cout.x = B.x - d; Cout.y = B.y;
		Dout.x = B.x + d; Dout.y = B.y;
		//line 1:
		if (A.y > B.y)
		{
			if ((C.y >= B.y) && (C.y <= A.y))
				flag1 = 1;
			else
				flag1 = 0;
		}
		else
		{
			if ((C.y >= A.y) && (C.y <= B.y))
				flag2 = 1;
			else
				flag2 = 0;
		}
		if (Aout.x > Bout.x)
		{
			if ((C.x >= Bout.x) && (C.x <= Aout.x))
				flag3 = 1;
			else
				flag3 = 0;
		}
		else
		{
			if ((C.x >= Aout.x) && (C.x <= Bout.x))
				flag4 = 1;
			else
				flag4 = 0;
		}
	}
	else if (A.y == B.y)
	{
		Aout.x = A.x; Aout.y = A.y - d;
		Bout.x = A.x; Bout.y = A.y + d;
		Cout.x = B.x; Cout.y = B.y - d;
		Dout.x = B.x; Dout.y = B.y + d;
		if (A.x > B.x)
		{
			if ((C.x >= B.x) && (C.x <= A.x))
				flag1 = 1;
			else
				flag1 = 0;
		}
		else
		{
			if ((C.x >= A.x) && (C.x <= B.x))
				flag2 = 1;
			else
				flag2 = 0;
		}
		if (Aout.y > Bout.y)
		{
			if ((C.y >= Bout.y) && (C.y <= Aout.y))
				flag3 = 1;
			else
				flag3 = 0;
		}
		else
		{
			if ((C.y >= Aout.y) && (C.y <= Bout.y))
				flag4 = 1;
			else
				flag4 = 0;
		}
	}
	Points points;
	points.A = Aout;
	points.B = Bout;
	points.C = Cout;
	points.D = Dout;
	points.flag = flag1 && flag2 && flag3 && flag4;
	return points;
}

double distance(Point center, Point line_a, Point line_b, int thre)
{
	double d = (fabs((line_b.y - line_a.y) * center.x + (line_a.x - line_b.x) * center.y + ((line_b.x * line_a.y) - (line_a.x * line_b.y)))) / (sqrt(pow(line_b.y - line_a.y, 2) + pow(line_a.x - line_b.x, 2)));
	/*if ((sqrt((line_a.x - center.x) * (line_a.x - center.x) + (line_a.y - center.y) * (line_a.y - center.y)) > 200) || (sqrt((line_b.x - center.x) * (line_b.x - center.x) + (line_b.y - center.y) * (line_b.y - center.y)) > 100))
		d += 100;*/
	Points points = inside(center, line_a, line_b, thre);
	if (!points.flag)
		d += 100;
	return d;
}

void makedir(string dirpath)
{
	string path_s = dirpath;
	char p_s[path_s.length()];
	int i_s;
	for (i_s = 0; i_s < path_s.length(); i_s++)
		p_s[i_s] = path_s[i_s];
	p_s[i_s] = '\0';
	if (access(p_s, F_OK) == -1)
		mkdir(p_s, S_IRWXU);
}

// 绘制四边形
void pltRect(Points quad_gt, Mat frame)
{
	line(frame, quad_gt.A, quad_gt.B, Scalar(0, 0, 255), 1.5);
	line(frame, quad_gt.A, quad_gt.C, Scalar(0, 0, 255), 1.5);
	line(frame, quad_gt.B, quad_gt.D, Scalar(0, 0, 255), 1.5);
	line(frame, quad_gt.C, quad_gt.D, Scalar(0, 0, 255), 1.5);
	putText(frame, "A", quad_gt.A, FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255));
	putText(frame, "B", quad_gt.B, FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255));
	putText(frame, "C", quad_gt.C, FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255));
	putText(frame, "D", quad_gt.D, FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255));

	//cout<<"plt"<<endl;
	//imwrite("save.jpg", frame);
	
}

//判断两条线段相交
bool intersection(const vector<Point> & line1, const vector<Point> &line2)//vector<Point> line(2),代表一线段
{
	CV_Assert(line1.size() == line2.size());
	CV_Assert(line1.size()==2);
	Point point1_11,point1_12,point1_21,point1_22;
	//首先判断line1的两个端点,在line2的两侧
	point1_11 = line2[0] - line1[0];
	point1_12 = line2[1]-line1[0];
 
	point1_21 = line2[0]-line1[1];
	point1_22 = line2[1]-line1[1];
 
	//point1_11.cross(point1_12)*point1_21.cross(point1_22)<0;//----------表明在两侧
	//再次判断line2的两个端点，在line1的两侧
	Point point2_11,point2_12,point2_21,point2_22;
 
	point2_11 = line1[0]-line2[0];
	point2_12 = line1[1] - line2[0];
	
	point2_21 = line1[0] -line2[1];
	point2_22 = line1[1]-line2[1];
	
	//point2_11.cross(point2_12)*point2_21.cross(point2_22)<0;
	return ( point1_11.cross(point1_12)*point1_21.cross(point1_22)<0 && point2_11.cross(point2_12)*point2_21.cross(point2_22)<0 );
}

//判断矩形和一条线段相交(线段只要与矩形的一条边相交，就可以判定相交 或者 线段在矩形内部)
bool rect_line_intersection(const vector<Point> & line,const Rect & targetRect)
//rect的四个顶点(roi.x,roi.y),(roi.x,roi.y+roi.height-1),(roi.x+roi.width-1,roi.y),(roi.x+roi.width-1,roi.y+roi.height-1)
{
	CV_Assert(line.size()==2);
	//先判断第一种情况：线段在矩形内部
	if(line[0].inside(targetRect) && line[1].inside(targetRect))
		return true;
	//再判断第二种情况,线段和矩形的至少一条边相交
	//---第一步：提取矩形的四条边
	vector<Point> line1;
	line1.clear();
	line1.push_back(Point(targetRect.x,targetRect.y));
	line1.push_back(Point(targetRect.x,targetRect.y+targetRect.height-1));
	if(intersection(line1,line))
		return true;
	vector<Point> line2;
	line2.clear();
	line2.push_back(Point(targetRect.x+targetRect.width-1,targetRect.y));
	line2.push_back(Point(targetRect.x+targetRect.width-1,targetRect.y+targetRect.height -1));
	if( intersection(line2,line))
		return true;
	vector<Point> line3;
	line3.clear();
	line3.push_back(Point(targetRect.x,targetRect.y));
	line3.push_back(Point(targetRect.x+targetRect.width-1,targetRect.y));
	if(  intersection(line3,line))
		return true;
	vector<Point> line4;
	line4.clear();
	line4.push_back(Point(targetRect.x,targetRect.y+targetRect.height-1));
	line4.push_back(Point(targetRect.x+targetRect.width-1,targetRect.y+targetRect.height-1));
	if(  intersection(line4,line) )
		return true;
	//return ( intersection(line1,line) || intersection(line2,line)||intersection(line3,line)||intersection(line4,line) );
	return false;
}
 
bool dete_intersection(Points quad_gt, const Rect & targetRect)
{	//依次判断四边形的四个边 是否 与 矩形相交
	//相交则返回：true
	vector<Point> line_AB;
	line_AB.clear();
	line_AB.push_back(quad_gt.A);
	line_AB.push_back(quad_gt.B);
	if(rect_line_intersection(line_AB, targetRect))
		return true;

	vector<Point> line_AC;
	line_AC.clear();
	line_AC.push_back(quad_gt.A);
	line_AC.push_back(quad_gt.C);
	if(rect_line_intersection(line_AC, targetRect))
		return true;
	
	vector<Point> line_BD;
	line_BD.clear();
	line_BD.push_back(quad_gt.B);
	line_BD.push_back(quad_gt.D);
	if(rect_line_intersection(line_BD, targetRect))
		return true;

	vector<Point> line_CD;
	line_CD.clear();
	line_CD.push_back(quad_gt.C);
	line_CD.push_back(quad_gt.D);
	if(rect_line_intersection(line_CD, targetRect))
		return true;

	//若 都不相交
	return false;

}

Point getPoint(string args_Point)
{	//获取cfg里的point
	size_t posa1 = args_Point.find(",");
	string corda1 = args_Point.substr(1, posa1 - 1);
	size_t posa2 = args_Point.find(")");
	string corda2 = args_Point.substr(posa1+1, posa2-posa1-1);
	Point p(atoi(corda1.c_str()), atoi(corda2.c_str()));
	
	return p;
}