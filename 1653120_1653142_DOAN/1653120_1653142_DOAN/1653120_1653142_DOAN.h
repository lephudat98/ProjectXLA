#pragma once

#include "resource.h"


//
//Các thư viện cần dùng
//
#include <objidl.h>

#include <gdiplus.h>//Hàm vẽ
#include<vector>
#include<string>
#include<fstream>
#include<iostream>

using namespace std;
using namespace cv;
using namespace Gdiplus;

//Đăng kí class
ATOM MyRegisterClass(HINSTANCE hInstance);

//Khời tạo instance mới
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

//----------------------------------------------------------------Xử lí phụ---------------------------------------------------------------------

//Đọc ảnh từ đường dẫn vào cấu trúc dữ liệu Mat
Mat readImage(wstring pathName);

//----------------------------------------------
//Vẽ một danh sách ảnh với cols cột và rows dòng (rows,cols)
//Cần truyền vào handle device context của window cần vẽ (hdc)
//Với vị trí bắt đầu cần vẽ là (x,y)
//Với danh sách đường dẫn file ảnh (pathFiles)
//Scale ảnh lại để vừa khung (weight,height)
//----------------------------------------------
void drawImages(HDC hdc, vector<wstring> pathFiles, int x, int y,
	int rows, int cols, int weight, int height);

//Common dialog cho phép người dùng chọn đường dân ảnh
OPENFILENAME OpenFileDialog(HWND hwnd);

//Đọc toàn bộ file ảnh trong folder
vector<wstring> getFiles(wstring);

//Hàm sort cho yêu cầu 2
void shellSort(vector<float> &arr, vector<wstring> &files, vector<Mat> &dbDescriptor);

//Hàm sort cho yêu cầu 1
void shellSort(vector<double> &arr, vector<wstring> &files, vector<MatND> &dbHists);


void writeDescriptor(Mat descriptorSrc, ofstream & fout);

void readDescriptor(Mat &descriptorSrc, ifstream & fin);


//----------------------------------------------------------------Xử lí đặc trưng màu---------------------------------------------------------------------

//Lấy histogram
//type 1: Quantize
//type 2: Nomarl
MatND GetHist(Mat src_base, int type);

//----------------------------------------------------------------Xử lí đặc trưng cạnh---------------------------------------------------------------------

//Trả ra số lượng điểm ảnh giống nhau
float getMatchValue(Mat descriptorsSrc, Mat descriptorsDst, int &goodSize, int &normalSize);

//Trả ra Mat descriptors
Mat getDesciptor(Mat src);
