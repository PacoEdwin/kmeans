#include<opencv2/opencv.hpp>
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include<iostream>
#include<vector>
#include<stdint.h>
#include<conio.h>
#include<math.h>
#include<algorithm>
#include<fstream>
#include<string>
using namespace std;
using namespace cv;
vector<vector<int>> coord;
int size_x = 0,size_y=0;
bool verify = false;
void convert(string s)
{
	int num1, num2;
	string tmp;
	bool check = false;
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == ';')
		{
			check = true;
			num1 = stoi(tmp);
			tmp = "";
			continue;
		}
		if (!check)
			tmp += s[i];
		else
			tmp += s[i];
	}
	num2 = stoi(tmp);
	coord.resize(coord.size() + 1);
	coord[coord.size() - 1].push_back(num1);
	coord[coord.size() - 1].push_back(num2);
	if (num1 > size_x)
		size_x = num1;
	if (num2 > size_y)
		size_y = num2;
}
void generate(vector<vector<double>> &c)
{
	for (int i = 0; i < c.size(); i++)
	{
		c[i][0] = (int)(rand() % size_x);
		c[i][1] = (int)(rand() % size_y);
	}
}

double euclidian(double x1,double y1,double x0, double y0)
{
	return sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1-y0));
}

bool check(vector<double> norm,double epsilon)
{
	bool checked = false;
	for (int i = 0; i < norm.size(); i++)
	{
		if (norm[i] > epsilon )
			checked = true;
	}
	return checked;
}

vector<double> dist_method(vector<vector<double>> c, vector<vector<double>> c_old)
{
	vector<double> norm(c.size());
	for (int i=0; i < c.size(); i++)
		norm[i] = euclidian(c[i][0],c[i][1],c_old[i][0],c_old[i][1]);
	return norm;
}

int argminD(int x, int y, vector<vector<double>> c)
{
	double dist=10000000;
	int j = 0;
	for (int i = 0; i < c.size(); i++)
	{
		double a = euclidian(x, y, c[i][0], c[i][1]);
		if (a < dist)
		{
			dist = a;
			j = i;
		}
	}
	return j;
}

vector<double> mean(vector<vector<int>> asd)
{
	verify = false;
	vector<double> c(2);
	if (asd.size() > 0)
	{
		for (int i = 0; i < asd.size(); i++)
		{
			c[0] += asd[i][0];
			c[1] += asd[i][1];
		}
		c[0] = c[0] / asd.size();
		c[1] = c[1] / asd.size();
	}
	else
		verify = true;
	return c;
}
void opencv(vector<vector<vector<int>>>, vector<vector<double>>, int);
int main();
vector<vector<double>> kmeans(int k, double epsilon)
{
	vector<vector<double>> centroids(k, vector<double>(2));
	generate(centroids);
	vector<vector<double>> c_old(k, vector<double>(2));
	vector<double> norm = dist_method(centroids, c_old);
	int iterations = 0;
	vector<vector<vector<int>>> final_result;
	while (check(norm,epsilon))
	{
		iterations += 1;
		vector<vector<vector<int>>> assigned(k);
		for (int i = 0; i < coord.size(); i++)
		{
			int j = argminD(coord[i][0], coord[i][1], centroids);
			assigned[j].resize(assigned[j].size()+1);
			assigned[j][assigned[j].size() - 1].push_back(coord[i][0]);
			assigned[j][assigned[j].size() - 1].push_back(coord[i][1]);
		}
		c_old = centroids;
		for (int i = 0; i < k; i++)
		{
			centroids[i] = mean(assigned[i]);
			if (verify)
				centroids[i] = c_old[i];
		}
		norm = dist_method(centroids, c_old);
		final_result = assigned;
	}
	opencv(final_result, centroids, k);
	return centroids;
}

void opencv(vector<vector<vector<int>>> assigned, vector<vector<double>> centroids, int k)
{
	Scalar colorTab[] =
	{
		Scalar(0, 0, 255),
		Scalar(0,255,0),
		Scalar(255,100,100),
		Scalar(255,0,255),
		Scalar(0,255,255)
	};
	Mat img(size_x, size_y, CV_8UC3);

	Mat points(coord.size(), 1, CV_32FC2), labels;
	Mat centers;
	img = Scalar::all(0);
	for (int i = 0; i < k;i++)
	{
		Point p;
		p.x = centroids[i][0];
		p.y = centroids[i][1];
		circle(img, p, 2, colorTab[i], FILLED, LINE_AA);
	}
	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < assigned[i].size(); j++)
		{
			Point p;
			p.x = assigned[i][j][0];
			p.y = assigned[i][j][1];
			circle(img, p, 2, colorTab[i], FILLED, LINE_AA);
		}
	}
	imshow("clusters", img);
	waitKey();
}
int main()
{
	ifstream  fin("mask_7.csv");
	string value;
	bool kek = true;
	while (getline(fin, value))
		convert(value);
	size_x += 20, size_y += 20;
	size_x = size_y = max(size_x, size_y);
	vector<vector<double>> centroids = kmeans(3, 0);
	system("pause");
	return 0;
}