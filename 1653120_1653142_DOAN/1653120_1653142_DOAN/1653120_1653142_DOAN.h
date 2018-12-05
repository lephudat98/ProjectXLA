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

//Lấy histogram
//type 1: Quantize
//type 2: Nomarl
MatND GetHist(Mat src_base, int type);

//Đọc ảnh từ đường dẫn vào cấu trúc dữ liệu Mat
Mat readImage(wstring pathName);

//Common dialog cho phép người dùng chọn đường dân ảnh
OPENFILENAME OpenFileDialog(HWND hwnd);



//----------------------------------------------
//Vẽ một danh sách ảnh với cols cột và rows dòng (rows,cols)
//Cần truyền vào handle device context của window cần vẽ (hdc)
//Với vị trí bắt đầu cần vẽ là (x,y)
//Với danh sách đường dẫn file ảnh (pathFiles)
//Scale ảnh lại để vừa khung (weight,height)
//----------------------------------------------
void drawImages(HDC hdc, vector<wstring> pathFiles, int x, int y,
	int rows, int cols, int weight, int height);
