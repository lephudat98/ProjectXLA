// 1653120_1653142_DOAN.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "1653120_1653142_DOAN.h"

//Loại bỏ warning 4996
#pragma warning(disable: 4996)
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 512

////Database
//
//Số lượng ảnh gần đúng người dùng chọn để xuất ra.
static int numImages = 0;
//Mảng chứa danh sách đường dẫn file
static vector<wstring > files;
//
////Database



//Đường dẫn file ảnh cần truy vấn
static wstring srcPath;
//Đường dẫn folder chưa database ảnh
static wstring PathName;


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    MAINBOX(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SHOWBOX(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	MyRegisterClass(hInstance);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY16531201653142DOAN));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;
}

// Message handler for MAINBOX box.
INT_PTR CALLBACK MAINBOX(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//ID phân biệt 2 db
	//		1: yêu cầu 1
	//		2: yêu cầu 2
	static int dbID;
	//Tránh trường hợp truy vấn không hợp lệ
	//Lượng hóa
	//		1: quantize
	//		2: non-quantize
	static int colorID;
	//Mảng lưu histogram
	static vector<MatND> dbHists;
	//Mảng lưu đặc trưng
	static vector<Mat> dbDescriptor;
	//Mảng giá trị so khớp ảnh
	static vector<float> dbMatchs;
	//Mảng giá trị compare , dùng method correl
	static vector<double>dbValuesHist;

	//Thông báo tình trạng xử lí đến người dùng
	static HWND hStatus;
	//Xác định loại yêu cầu đề bài
	static int required;
	//Xử lí chuỗi thống báo
	static WCHAR str[MAX_LOADSTRING];

	//Lượng hóa
	//		1: quantize
	//		2: non-quantize
	static int typeofcolor;
	static HWND comboBox = NULL;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG://Khởi tạo đầu tiên cho dialog
	{		
		dbID = required = 2;
		SetFocus(GetDlgItem(hWnd, IDC_EDIT_PATHFILE));
		//Sẽ chọn dèault là quantize
		SendDlgItemMessage(hWnd, IDC_CHECK_QUANTITATIVE, BM_SETCHECK, TRUE, 0);
		hStatus = GetDlgItem(hWnd, IDC_STATUS);
		CheckRadioButton(hWnd, IDC_RADIO_COLORFREATURE, IDC_RADIO_EDGEFEATURE, IDC_RADIO_EDGEFEATURE);
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_QUANTITATIVE), FALSE);
		//Khởi tạo dữ liệu cho dialog
		comboBox = (HWND )GetDlgItem(hWnd, IDC_COMBO_NUMBEROFIMAGE);
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"03");
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"05");
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"11");
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"21");
		SendMessage(comboBox, CB_SETCURSEL, NULL, NULL);
		SendMessage(comboBox, CB_GETLBTEXT, NULL, (LPARAM)str);
		colorID = 1;
		numImages = 3;
		CheckDlgButton(hWnd, IDC_CHECK_QUANTITATIVE, TRUE);
		typeofcolor = 1;
		return (INT_PTR)TRUE;
	}
	break;

	case WM_COMMAND://Xử lí các message từ lệnh người dùng trong dialog
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_LOADIMAGE://Chọn đường dẫn
		{
			OpenFileDialog(hWnd);
			if (srcPath.empty())
			{
				MessageBox(hWnd, L"Invalid path file", L"Error", MB_OK);
			}			
		}
		break;

		case IDC_COMBO_NUMBEROFIMAGE://Xử lí combo box lấy 
		{
			if (HIWORD(wParam) == CBN_SELENDOK)
			{
				WCHAR number[128];
				int comboIndex = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
				SendMessage(comboBox, CB_GETLBTEXT, comboIndex, (LPARAM)number);
				wchar_t * pEnd;
				//Chuyển sang integer
				numImages = wcstol(number, &pEnd, 10);
			}
		}
		break;

		case IDC_CHECK_QUANTITATIVE:
		{
			//Lấy loại quantize hoặc non-quantize
			if (IsDlgButtonChecked(hWnd, IDC_CHECK_QUANTITATIVE))
			{
				typeofcolor = 1;
				wsprintf(str, L"Đang chọn yêu cầu đồ án 1, chế độ lượng hóa màu(quantize)");
			}
			else
			{
				typeofcolor = 2;
				wsprintf(str, L"Đang chọn yêu cầu đồ án 1, chế độ thường(non-quantize)");
			}
			SetDlgItemText(hWnd, IDC_STATUS, str);
		}
		break;
		
		//Yêu cầu đồ án 1:
		case IDC_RADIO_COLORFREATURE:
		{
			if (IsDlgButtonChecked(hWnd, IDC_CHECK_QUANTITATIVE))
				wsprintf(str, L"Đang chọn yêu cầu đồ án 1, chế độ lượng hóa màu");
			else
				wsprintf(str, L"Đang chọn yêu cầu đồ án 1, chế độ không lượng màu hóa");
			required = 1;
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_QUANTITATIVE), TRUE);
			SetDlgItemText(hWnd, IDC_STATUS, str);
		}
		break;

		//Yêu cầu đồ án 2:
		case IDC_RADIO_EDGEFEATURE:
		{
			wsprintf(str, L"Đang chọn yêu cầu đồ án 2");
			required = 2;
			EnableWindow(GetDlgItem(hWnd, IDC_CHECK_QUANTITATIVE), FALSE);
			SetDlgItemText(hWnd, IDC_STATUS, str);
		}
		break;

		case IDC_BUTTON_GETDATABASE:
		{
			files.clear();
			//Lấy đường dẫn folder từ người dùng nhập
			GetDlgItemText(hWnd, IDC_EDIT_PATHFILE, str, MAX_LOADSTRING);
			PathName = wstring(str);

			SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi trong khi lấy dữ liệu!");
			//Lấy toàn bộ file từ đường dẫn trên, lưu vào mảng files

			files = getFiles(PathName);
			
			//Loại trừ trường hợp folder trống hoặc không tồn tại
			if (files.empty())
			{
				SetDlgItemText(hWnd, IDC_STATUS, L"Đường dẫn không tồn tại\n");
				break;
			}
			

			if (required == 1)
			{
				colorID = typeofcolor;
				dbID = 1;
				Mat dstImage;
				dbHists.clear();
				int i = 0;
				while (i < files.size())
				{
					dstImage = readImage(files[i]);

					//Loop đến khi nào đọc được ảnh của file đó

					if (!dstImage.empty())
					{
						wsprintf(str, L"Đang rút trích đặc trưng màu: %d / %d", i, files.size());
						SetDlgItemText(hWnd, IDC_STATUS, str);
						dbHists.push_back(GetHist(dstImage, typeofcolor));
						i++;
					}
				}

				//Lưu db lên file
				SetDlgItemText(hWnd, IDC_STATUS, L"Thêm cơ sở dữ liệu thành công!");
			}
			
			if (required == 2)
			{
				dbID = 2;
				dbDescriptor.clear();
				Mat img;
				SetDlgItemText(hWnd, IDC_STATUS, L"Đang thêm cơ sở dữ liệu yêu cầu 2!");
				//Lấy dữ mảng descriptor
				for (int i = 0; i < files.size(); i++)
				{
					img = readImage(files[i]);
					wsprintf(str, L"Đang rút trích đặc trưng hình dáng, cạnh: %d / %d",i,files.size());
					SetDlgItemText(hWnd, IDC_STATUS, str);
					dbDescriptor.push_back(getDesciptor(img));
				}

				SetDlgItemText(hWnd, IDC_STATUS, L"Đang lưu cơ sở dữ liệu...");
				//ofstream fout;
				//fout.open("db2", ios::binary | ios::out);
				//short size = files.size();
				//fout.write((char*)&size, sizeof(short));
				//for (int i = 0; i < files.size(); i++)
				//{
				//	Mat image1 = readImage(files[i]);
				//	Mat des = getDesciptor(image1);
				//	writeDescriptor(des, fout);
				//}
				SetDlgItemText(hWnd, IDC_STATUS, L"Thêm cơ sở dữ liệu yêu thành công!");
			}
		}
		break;

		case IDOK:
		{
			//Ràng buộc người dùng phải chọn folder hợp lệ 
			if (files.empty())
			{
				MessageBox(hWnd, L"Cơ sở dữ liệu rỗng", L"Lưu ý", MB_OK);

				return (INT_PTR)TRUE;
			}
			
			if (srcPath.empty())
			{
				MessageBox(hWnd, L"Vui lòng chọn ảnh cần truy vấn", L"Lưu ý", MB_OK);
				return (INT_PTR)TRUE;
			}
			
			if (required == 1)
			{
				if (dbID == 2)
				{
					MessageBox(hWnd, L"Không thể truy vấn với cơ sở dữ liệu đặc trưng hình dáng,cạnh", L"Lưu ý", MB_OK);
					return (INT_PTR)TRUE;
				}
				if(colorID!=typeofcolor)
				{
					MessageBox(hWnd, L"Quantize không khớp với loại quantize cơ sở dữ liệu, vui lòng kiểm tra lại", L"Lưu ý", MB_OK);
					return (INT_PTR)TRUE;
				}
				colorID = typeofcolor;
				//Lấy hist cho từng ảnh, lấy ảnh gốc so sánh với từng ảnh trong database, lưu vào vào mảng valueHist
				Mat srcImage = readImage(srcPath);
				MatND srcHist;
				
				//Xóa dữ liệu cũ
				dbValuesHist.clear();

				srcHist = GetHist(srcImage, typeofcolor);

				SetDlgItemText(hWnd, IDC_STATUS, L"Getting value compare.....");

				for (int i = 0; i < files.size(); i++)
				{
					wsprintf(str, L"Đang so sánh histogram (%d / %d)", i + 1, files.size());
					SetDlgItemText(hWnd, IDC_STATUS, str);
					dbValuesHist.push_back(compareHist(srcHist, dbHists[i], CV_COMP_CORREL));
				}
				wsprintf(str, L"Đang sắp xếp... ");
				SetDlgItemText(hWnd, IDC_STATUS, str);
				//Sắp xếp mảng giảm giần từ giống nhiều về ít giống 
				shellSort(dbValuesHist, files,dbHists);

				wsprintf(str, L"Truy vấn hoàn thành");
				SetDlgItemText(hWnd, IDC_STATUS, str);

			}

			if (required == 2)
			{
				if (dbID == 1)
				{
					MessageBox(hWnd, L"Không thể truy vấn với cơ sở dữ liệu đặc trưng màu", L"Lưu ý", MB_OK);
					return (INT_PTR)TRUE;
				}
				dbMatchs.clear();//Xóa dữ liệu mảng giá trị so khớp cũ
				Mat src = readImage(srcPath),img;
				if (src.empty())
				{
					SetDlgItemText(hWnd, IDC_STATUS, L"Nhập ảnh không hợp lệ!");
					return (INT_PTR)TRUE;
				}
				Mat srcDescriptor = getDesciptor(src);
				int normalSize, goodSize;
				for (int i = 0; i < dbDescriptor.size(); i++)
				{
					dbMatchs.push_back(getMatchValue(srcDescriptor, dbDescriptor[i], goodSize, normalSize));
					wsprintf(str, L"Descriptor process: %d / %d", i+1, files.size());
					SetDlgItemText(hWnd, IDC_STATUS, str);
				}

				wsprintf(str, L"Đang sắp xếp... ");
				SetDlgItemText(hWnd, IDC_STATUS, str);
				/*wofstream fout;
				fout.open("log before sort.txt", ios::out);
				for (int i = 0; i < 100; i++)
					fout << dbMatchs[i] << L"\t" << files[i] << endl;
				fout.close();*/
				shellSort(dbMatchs, files,dbDescriptor);
				/*wofstream fout1;
				fout1.open("log after sort.txt", ios::out);
				for (int i = 0; i < files.size(); i++)
					fout1 << dbMatchs[i] << L"\t" << files[i] << endl;
				fout1.close();
*/
				wsprintf(str, L"Truy vấn hoàn thành");
				SetDlgItemText(hWnd, IDC_STATUS, str);
			}
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SHOW), hWnd, SHOWBOX);			
		}
		return (INT_PTR)TRUE;
		default:
			break;
		}
	}
	break;

	case WM_CLOSE://Xử lí message tắt dialog
	{
		PostQuitMessage(0);
	}
	break;

	}

	return (INT_PTR)FALSE;
}

// Message handler for SHOWBOX box.
INT_PTR CALLBACK SHOWBOX(
	HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
	{
		
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		// TODO: Add any drawing code that uses hdc here...
		Gdiplus::Graphics g(hdc);
		//Vẽ ảnh gốc
		Image image(srcPath.c_str(), true);
		g.DrawImage(&image, 0, 0, 200, 200);
		
		//vẽ các ảnh kết quả
		if(!files.empty())
			drawImages(hdc, files, 300, 0, 6, 4, 150, 150);

		EndPaint(hDlg, &ps);

	}
	break;

	case WM_CLOSE:
	{
		EndDialog(hDlg, 1);
		break;
	}

	}

	return (INT_PTR)FALSE;
}


//Đăng kí class
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MAINBOX;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY16531201653142DOAN));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//Khời tạo instance mới
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = (HWND)DialogBox(hInst, MAKEINTRESOURCE(IDD_MAINBOX), NULL, MAINBOX);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//Lấy histogram
//type 1: Quantize
//type 2: Nomarl
MatND GetHist(Mat src_base, int type)
{
	Mat hsv_base;
	MatND hist_base;

	if (type == 1)
	{
		cvtColor(src_base, hsv_base, COLOR_BGR2HSV);
		OutputDebugString(L"Quantize\n");
		int h_bins = 50; int s_bins = 60;
		int histSize[] = { h_bins, s_bins };

		// hue varies from 0 to 179, saturation from 0 to 255
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 256 };

		const float* ranges[] = { h_ranges, s_ranges };

		// Use the o-th and 1-st channels
		int channels[] = { 0, 1 };


		/// Calculate the histograms for the HSV images
		calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false);
		normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());
		return hist_base;
	}
	else
	{
		OutputDebugString(L"Normal\n");
		cvtColor(src_base, hsv_base, COLOR_BGR2HSV);
		// We don't quantize so we will init 180 bins for hue and 256 bins for saturation.
		int h_bins = 180; int s_bins = 256;
		int histSize[] = { h_bins, s_bins };

		// hue varies from 0 to 179, saturation from 0 to 255
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 256 };

		const float* ranges[] = { h_ranges, s_ranges };

		// Use the o-th and 1-st channels
		int channels[] = { 0, 1 };


		/// Calculate the histograms for the HSV images
		calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false);
		normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());
		return hist_base;
	}

	return hist_base;
}

//Đọc ảnh từ đường dẫn vào cấu trúc dữ liệu Mat
Mat readImage(wstring pathName)
{
	//B1: Đọc toàn bộ dữ liệu binary của ảnh vào 1 file tạm
	FILE* fp = _wfopen(pathName.c_str(), L"rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	char* buf = new char[sz];
	fseek(fp, 0, SEEK_SET);
	long n = fread(buf, 1, sz, fp);
	_InputArray arr(buf, sz);
	//B2: decode từ file trên
	Mat img = imdecode(arr, CV_LOAD_IMAGE_COLOR);
	
	//B3: Xóa file tạm
	delete[] buf;

	fclose(fp);

	return img;
}

//Common dialog cho phép người dùng chọn đường dân ảnh
OPENFILENAME OpenFileDialog(HWND hwnd)
{
	WIN32_FIND_DATA find;
	HANDLE tmp;

	OPENFILENAME ofn;       // common dialog box structure
	WCHAR szFile[MAX_LOADSTRING];       // buffer for file name

							// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"PNG(*.png)\0*.png\0JPG(*.jpg)\0*.jpg\0TIF(*.tif)\0*.tif\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE)
	{
		//lưu đường dẫn ảnh cần truy vấn
		srcPath = ofn.lpstrFile;
	}
	return ofn;
}

//----------------------------------------------
//Vẽ một danh sách ảnh với cols cột và rows dòng (rows,cols)
//Cần truyền vào handle device context của window cần vẽ (hdc)
//Với vị trí bắt đầu cần vẽ là (x,y)
//Với danh sách đường dẫn file ảnh (pathFiles)
//Scale ảnh lại để vừa khung (weight,height)
//----------------------------------------------
void drawImages(HDC hdc, vector<wstring> pathFiles, int x, int y,
	int rows, int cols, int weight, int height)
{
	if (pathFiles.empty())
		return;
	Gdiplus::Graphics g(hdc);
	float test;
	int positionX = 0;
	int positionY = 0;
	int fileIndex = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if ((fileIndex<numImages)&&
				(fileIndex < pathFiles.size()))
			{
				Image img(pathFiles[fileIndex].c_str());
				wstringstream ss;
				positionY = y + j * weight;
				positionX = x + i * height;
				g.DrawImage(&img,positionX, positionY,  weight, height);
				fileIndex++;
			}
		}
	}
	
}


//Trả ra số lượng điểm ảnh giống nhau
float getMatchValue(Mat descriptorsSrc, Mat descriptorsDst,
	int &goodSize, int &normalSize)
{
	vector<DMatch> matches; // Tạo mảng vector kiểu DMatch để lưu trữ các điểm match.
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::BRUTEFORCE_HAMMING); //Khởi tạo 1 con trỏ để thực hiện việc so sánh các điểm bằng thuật toán BruteForce-Hamming

	matcher->match(descriptorsSrc, descriptorsDst, matches); // Thực hiện hàm match để kiểm tra giữa các keypoints của 2 descriptors được truyền vào tức (descriptors_src và descriptors_dst) 
	//và trả về 1 mảng vector các điểm trùng hợp kiểu dữ liệu DMatch
	double min_dist = 10000, max_dist = 0;
	for (int i = 0; i < descriptorsSrc.rows; i++)
	{
		double dist = matches[i].distance; // Tính khoảng cách local cho từng điểm match.
		if (dist < min_dist) min_dist = dist; // Tìm khoảng cách local bé nhất
		if (dist > max_dist) max_dist = dist; // Tìm khoảng cách local lớn nhất
	}

	vector< DMatch > good_matches; // Tạo một vector kiểu DMatch để lưu các điểm match tốt hơn sau khi lọc qua khoảng cách được quy định trước hay còn gọi là good match.
	for (int i = 0; i < descriptorsSrc.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 30.0)) // kiểm tra xem khoảng cách của điểm match có nằm trong vùng được quy định
		{
			good_matches.push_back(matches[i]); // Nếu nằm trong thì sẽ lưu lại các điểm match vào 1 vector đã khởi tạo ở trên.
		}
	}
	goodSize = good_matches.size();
	normalSize = matches.size();
	return (good_matches.size()*1.0/ matches.size()); // Trả về số lượng điểm good match .
}

//Trả ra Mat descriptors
Mat getDesciptor(Mat src)
{
	// Tạo 1 vector kiểu dữ liệu KeyPoint để lưu trữ các điểm nhấn của ảnh.
	vector<KeyPoint> keypoints;
	Mat descriptors; // Tạo 1 descriptor kiểu Mat

	// Tạo 1 con trỏ kiểu dữ liệu ORB và khởi tạo để phục vụ cho việc detect ảnh
	Ptr<ORB> detector = ORB::create(); 

	// Tương tự ta cũng tạo 1 con trỏ kiểu DescriptorExtractor phục vụ cho việc tính toán các keypoints.
	Ptr<DescriptorExtractor> descriptor = ORB::create(); 

	// Detect ảnh để phát hiện các điểm nhấn của ảnh được truyền vào tức (src) và lưu vào vector đã tạo.
	detector->detect(src, keypoints); 

	//Tính toán các keypoints đã được detect ở trên và lưu vào descriptor kiểu Mat đã tạo.
	descriptor->compute(src, keypoints, descriptors); 

	// Trả về ảnh đã được tính toán và detect đặc trưng.
	return descriptors; 
}

vector<wstring> getFiles(wstring PathName)
{
	vector<wstring> files;
	//Lấy toàn bộ file từ đường dẫn trên, lưu vào mảng files
	WIN32_FIND_DATA find;
	HANDLE tmp;
	int i = 0;
	files.clear();
	wstring search = PathName + L"/*.*";
	tmp = FindFirstFile(search.c_str(), &find);
	if (tmp != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				//string s = PathName + L"\\" + find.cFileName;
				files.push_back(PathName + L"\\" + find.cFileName);
				//cout << s << endl;
			}
		} while (FindNextFile(tmp, &find));
		FindClose(tmp);

	}
	return files;
}

//Hàm sort cho yêu cầu 2
void shellSort(vector<float> &arr, vector<wstring> &files, 
	vector<Mat> &dbDescriptor)
{
	// Start with a big gap, then reduce the gap 
	for (int gap = arr.size() / 2; gap > 0; gap /= 2)
	{
		// Do a gapped insertion sort for this gap size. 
		// The first gap elements a[0..gap-1] are already in gapped order 
		// keep adding one more element until the entire array is 
		// gap sorted  
		for (int i = gap; i < arr.size(); i += 1)
		{
			// add a[i] to the elements that have been gap sorted 
			// save a[i] in temp and make a hole at position i 
			float temp = arr[i];
			wstring temp1 = files[i];
			Mat temp2 = dbDescriptor[i];
			// shift earlier gap-sorted elements up until the correct  
			// location for a[i] is found 
			int j;
			for (j = i; j >= gap && arr[j - gap] < temp; j -= gap)
			{
				arr[j] = arr[j - gap];
				files[j] = files[j - gap];
				dbDescriptor[j] = dbDescriptor[j - gap];
			}
			//  put temp (the original a[i]) in its correct location 
			arr[j] = temp;
			files[j] = temp1;
			dbDescriptor[j] = temp2;

		}
	}
}

//Hàm sort cho yêu cầu 1
void shellSort(vector<double> &arr, vector<wstring> &files,
	vector<MatND> &dbHists)
{
	// Start with a big gap, then reduce the gap 
	for (int gap = arr.size() / 2; gap > 0; gap /= 2)
	{
		// Do a gapped insertion sort for this gap size. 
		// The first gap elements a[0..gap-1] are already in gapped order 
		// keep adding one more element until the entire array is 
		// gap sorted  
		for (int i = gap; i < arr.size(); i += 1)
		{
			// add a[i] to the elements that have been gap sorted 
			// save a[i] in temp and make a hole at position i 
			double temp = arr[i];//Hàm sort cho yêu cầu 1
			wstring temp1 = files[i];
			MatND temp2 = dbHists[i];

			// shift earlier gap-sorted elements up until the correct  
			// location for a[i] is found 
			int j;
			for (j = i; j >= gap && arr[j - gap] < temp; j -= gap)
			{
				arr[j] = arr[j - gap];
				files[j] = files[j - gap];
				dbHists[j] = dbHists[j - gap];
			}
			//  put temp (the original a[i]) in its correct location 
			arr[j] = temp;
			files[j] = temp1;
			dbHists[j] = temp2;
		}
	}
}


void writeDescriptor(Mat descriptorSrc, ofstream & fout)
{
	fout.write((char*)&descriptorSrc.rows, sizeof(short));
	fout.write((char*)&descriptorSrc.cols, sizeof(short));
	for (int i = 0; i < descriptorSrc.rows; i++)
	{
		for (int j = 0; j < descriptorSrc.cols; j++)
		{
			fout.write((char*)&descriptorSrc.at<uchar>(i, j), sizeof(short));
		}
	}
}
void readDescriptor(Mat &descriptorSrc, ifstream & fin)
{
	descriptorSrc.clone();
	short rows, cols;
	fin.read((char*)&rows, sizeof(short));
	fin.read((char*)&cols, sizeof(short));
	descriptorSrc = Mat(rows, cols, CV_8UC1);
	for (int i = 0; i < descriptorSrc.rows; i++)
	{
		for (int j = 0; j < descriptorSrc.cols; j++)
		{
			fin.read((char*)&descriptorSrc.at<uchar>(i, j), sizeof(short));
		}
	}
}
