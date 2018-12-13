// 1653120_1653142_DOAN.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "1653120_1653142_DOAN.h"

#include<ShlObj.h>
#include<ShlObj_core.h>
#include <chrono> // C++11
using namespace std::chrono;



//Loại bỏ warning 4996
#pragma warning(disable: 4996)
#pragma comment (lib,"Gdiplus.lib")

#define dbPathReq1 "db1"
#define dbPathReq2 "db2"
#define MAX_LOADSTRING 512

////Database
//
//Số lượng ảnh gần đúng người dùng chọn để xuất ra.
 int numImages = 0;
//Mảng chứa danh sách đường dẫn file
 vector<wstring > files;

//Tránh trường hợp truy vấn không hợp lệ
//Lượng hóa
//		1: quantize
//		2: non-quantize
 int colorID;
//Mảng lưu histogram
 vector<MatND> dbHists;
//Mảng lưu đặc trưng
 vector<Mat> dbDescriptors;
//Mảng giá trị so khớp ảnh
 vector<float> dbMatchs;
//Mảng giá trị compare , dùng method correl
 vector<double>dbValuesHist;

//Xử lí chuỗi thống báo
 WCHAR str[MAX_LOADSTRING];

//Lượng hóa
//		1: quantize
//		2: non-quantize
 int typeofcolor;
 HWND comboBox = NULL;
//Thông báo tình trạng xử lí đến người dùng
 HWND hStatus;
//Xác định loại yêu cầu đề bài
 int required;

//Đường dẫn file ảnh cần truy vấn
 wstring srcPath;
//Đường dẫn folder chưa database ảnh
 wstring PathName;


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    MAINBOX(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SHOWBOX(HWND, UINT, WPARAM, LPARAM);

BOOL OpenChooseDatabaseDialog(HWND hDialog, PTSTR folderName) // mở hộp thoại database
{
	BROWSEINFO browseInfo = { 0 };
	browseInfo.lpszTitle = L"Choose database folder";
	browseInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST idList = SHBrowseForFolder(&browseInfo);
	if (idList != 0)
	{
		SHGetPathFromIDList(idList, folderName);
		CoTaskMemFree(idList);
		return TRUE;
	}
	return FALSE;
}

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


	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG://Khởi tạo đầu tiên cho dialog
	{		
		SetFocus(GetDlgItem(hWnd, IDC_EDIT_PATHFILE));
		//Chọn số lượng ảnh xuất ra màn hình
		//Khởi tạo
		
		//Khởi tạo dữ liệu cho dialog
		comboBox = (HWND)GetDlgItem(hWnd, IDC_COMBO_NUMBEROFIMAGE);
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"03");
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"05");
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"11");
		SendMessage(comboBox, CB_ADDSTRING, NULL, (LPARAM)L"21");
		SendMessage(comboBox, CB_SETCURSEL, NULL, NULL);
		SendMessage(comboBox, CB_GETLBTEXT, NULL, (LPARAM)str);

		//Phân biệt đang ở yêu cầu 1, 2
		//Phân biệt cơ sở dữ liệu
		required = 2;
		colorID = 1;
		typeofcolor = 1;
		numImages = 3;

		//Set default cho rút trích đặc trưng màu, quantize
		SendDlgItemMessage(hWnd, IDC_CHECK_QUANTITATIVE, BM_SETCHECK, TRUE, 0);

		//Label để xuất thông tin xử lí ra màn hình
		hStatus = GetDlgItem(hWnd, IDC_STATUS);

		//Xử lí group radio button
		CheckRadioButton(hWnd, 
			IDC_RADIO_COLORFREATURE, IDC_RADIO_EDGEFEATURE,
			IDC_RADIO_EDGEFEATURE);

		//Ẩn chọn quantize
		EnableWindow(GetDlgItem(hWnd, IDC_CHECK_QUANTITATIVE), FALSE);
		CheckDlgButton(hWnd, IDC_CHECK_QUANTITATIVE, TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_LOADDB), FALSE); // enable button truy vấn

		EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_GETDATABASE), FALSE); // enable button truy vấn


		SetDlgItemText(hWnd, IDC_EDIT_PATHFOLDER, L"a");
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
			else
				SetWindowText(GetDlgItem(hWnd, IDC_EDIT_PATHFILE), srcPath.c_str());
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
		
		case IDC_EDIT_PATHFOLDER:
		{
			GetDlgItemText(hWnd, IDC_EDIT_PATHFOLDER, str, MAX_LOADSTRING);
			PathName = wstring(str);
			if (!PathName.empty() && PathFileExists(str))
			{
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_GETDATABASE), TRUE); // enable button truy vấn
			
				//Kiểm tra DB có tồn tại trước đó không
				wifstream fin1, fin2;
				fin1.open(dbPathReq1, ios::in | ios::binary);
				fin2.open(dbPathReq2, ios::in | ios::binary);
				if ((!fin1 && !fin2) || (fin1.eof() && fin2.eof()))
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_LOADDB), FALSE); // enable button truy vấn
				else
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_LOADDB), TRUE); // enable button truy vấn
				fin1.close();
				fin2.close();
			}
			else
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_GETDATABASE), FALSE); // enable button truy vấn
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
		
		case IDC_BUTTON_LOADIFOLDER:
		{
			TCHAR databasePath[MAX_LOADSTRING];
			if (OpenChooseDatabaseDialog(hWnd, databasePath) && PathFileExists(databasePath)) // mở hộp thoại thư mục
			{
				PathName = databasePath;
				SetWindowText(GetDlgItem(hWnd, IDC_EDIT_PATHFOLDER), databasePath); //set đường dẫn vào ô text Database
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_GETDATABASE), TRUE); // enable button truy vấn
			}
		}
		break; 

		case IDC_BUTTON_GETDATABASE:
		{
			files.clear();
			//Lấy đường dẫn folder từ người dùng nhập
			GetDlgItemText(hWnd, IDC_EDIT_PATHFOLDER, str, MAX_LOADSTRING);
			PathName = wstring(str);

			SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi trong khi lấy dữ liệu!");
			//Lấy toàn bộ file từ đường dẫn trên, lưu vào mảng files

			auto start = steady_clock::now();
			files = getFiles(PathName);
			auto end = steady_clock::now();
			wofstream fTime;
			fTime.open("timeLog.txt", ios::app);
			fTime << "Read list of files: " << duration_cast<seconds>(end - start).count() << " seconds\n";
			//Loại trừ trường hợp folder trống hoặc không tồn tại
			if (files.empty())
			{
				SetDlgItemText(hWnd, IDC_STATUS, L"Đường dẫn không tồn tại\n");
				break;
			}
			

			if (required == 1)
			{
				fTime << "Tao CSDL cho yeu cau 1: \n";
				colorID = typeofcolor;
				Mat dstImage;
				dbHists.clear();
				int i = 0;
				auto startRutTrich = steady_clock::now();
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

				auto endRutTrich = steady_clock::now();
				fTime << "Rut trich: " << duration_cast<seconds>(endRutTrich - startRutTrich).count() << " seconds\n";
				SetDlgItemText(hWnd, IDC_STATUS, L"Đang lưu cơ sở dữ liệu...");

				//Lưu db lên file
				ofstream fout;
				fout.open(dbPathReq1, ios::out);
				float val;
				auto startGhiFile = steady_clock::now();
				fout << files.size() << " ";
				for (int index = 0; index < files.size(); index++)
				{
					fout << dbHists[index].rows << " ";
					fout << dbHists[index].cols << " ";
					for (int i = 0; i < dbHists[index].rows; i++)
					{
						for (int j = 0; j < dbHists[index].cols; j++)
						{
							val = 1.0*dbHists[index].at<float>(i, j);
							fout << val << " ";
						}

					}
				}
				auto endGhiFile = steady_clock::now();
				fTime << "Ghi File: " << duration_cast<seconds>(endGhiFile - startGhiFile).count() << " seconds\n";
				fout.close();
				SetDlgItemText(hWnd, IDC_STATUS, L"Thêm cơ sở dữ liệu thành công!");
			}
			
			if (required == 2)
			{
				fTime << "Tao CSDL cho yeu cau 2: \n";
				ofstream fout;
				short value;
				Mat img;

				dbDescriptors.clear();
				SetDlgItemText(hWnd, IDC_STATUS, L"Đang thêm cơ sở dữ liệu yêu cầu 2!");
				//Lấy dữ mảng descriptor
				auto startRutTrich = steady_clock::now();
				for (int i = 0; i < files.size(); i++)
				{
					img = readImage(files[i]);
					dbDescriptors.push_back(getDesciptor(img));
					wsprintf(str, L"Đang rút trích đặc trưng hình dáng, cạnh: %d / %d", i, files.size());
					SetDlgItemText(hWnd, IDC_STATUS, str);
				}

				auto endRutTrich = steady_clock::now();
				fTime << "Rut trich: " << duration_cast<seconds>(endRutTrich - startRutTrich).count() << " seconds\n";
				SetDlgItemText(hWnd, IDC_STATUS, L"Đang lưu cơ sở dữ liệu...");

				fout.open(dbPathReq2, ios::out);
				auto startGhiFile = steady_clock::now();
				fout << dbDescriptors.size() << " ";
				for (int index = 0; index < dbDescriptors.size(); index++)
				{
					fout << dbDescriptors[index].rows << " ";
					fout << dbDescriptors[index].cols << " ";
					for (int i = 0; i < dbDescriptors[index].rows; i++)
					{
						for (int j = 0; j < dbDescriptors[index].cols; j++)
						{
							value = (short)dbDescriptors[index].at<uchar>(i, j);
							fout << value << " ";
						}
					}
				}

				auto endGhiFile = steady_clock::now();
				fTime << "Ghi file: " << duration_cast<seconds>(endGhiFile - startGhiFile).count() << " seconds\n";
				fout.close();
				EnableWindow(GetDlgItem(hWnd, IDC_BUTTON_LOADDB), TRUE); // enable button truy vấn
				SetDlgItemText(hWnd, IDC_STATUS, L"Thêm cơ sở dữ liệu yêu thành công!");
			}

			fTime.close();
		}
		break;

		case IDC_BUTTON_LOADDB:
		{
			files.clear();
			auto start = steady_clock::now();
			files = getFiles(PathName);
			auto end = steady_clock::now();
			wofstream fTime;
			fTime.open("timeLog.txt", ios::app);
			fTime << "Read list of files: " << duration_cast<seconds>(end - start).count() << " seconds\n";
			if (required == 1)
			{
				fTime << "Load CSDL cho yeu cau 1: \n";
				colorID = typeofcolor;
				short rows, cols;
				float rdDBvalue;
				//Reset danh sách lưu trữ, tránh lầm file này với file khác
				SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi xóa data cũ!");
				dbHists.clear();
				dbValuesHist.clear();

				//Khởi tạo lại dữ liệu từ file đã lưu trữ
				SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi load files!");
				short dbSize;
				ifstream fin(dbPathReq1, ios::binary || ios::in); 
				fin >> dbSize;

				if (files.size() == dbSize)
				{
					auto startRutTrich = steady_clock::now();
					SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi load CSDL!");
					for (int index = 0; index < dbSize; index++)
					{
						fin >> rows;
						fin >> cols;
						MatND dbHist(rows, cols, CV_32FC1);
						for (int i = 0; i < dbHist.rows; i++)
						{
							for (int j = 0; j < dbHist.cols; j++)
							{
								fin >> rdDBvalue;
								dbHist.at<float>(i, j) = rdDBvalue;
							}
						}
						dbHists.push_back(dbHist);
					}
					auto endRutTrich = steady_clock::now();
					fTime << "Rut trich: " << duration_cast<seconds>(endRutTrich - startRutTrich).count() << " seconds\n";
					SetDlgItemText(hWnd, IDC_STATUS, L"Load CSDL thành công!");
				}
				else
				{
					SetDlgItemText(hWnd, IDC_STATUS, L"Cơ sở dữ liệu không phù hợp, vui lòng tạo lại!");
				}
				fin.close();

			}
			if (required == 2)
			{
				fTime << "Load CSDL cho yeu cau 2: \n";
				short rdDBvalue;
				short dbReadSize;
				ifstream fin;
				short rows, cols;

				//Reset danh sách lưu trữ, tránh lầm file này với file khác
				SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi xóa data cũ!");
				dbDescriptors.clear();
				//Khởi tạo lại dữ liệu từ file đã lưu trữ

				SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi load files!");
				fin.open(dbPathReq2,  ios::in);
				fin >> dbReadSize;

				if (files.size() == dbReadSize)
				{
					auto startRutTrich = steady_clock::now();
					SetDlgItemText(hWnd, IDC_STATUS, L"Vui lòng đợi load CSDL!");
					for (int index = 0; index < dbReadSize; index++)
					{
						fin >> rows;
						fin >> cols;
						Mat descrip(rows, cols, CV_8UC1);
						for (int i = 0; i < descrip.rows; i++)
						{
							for (int j = 0; j < descrip.cols; j++)
							{
								fin >> rdDBvalue;
								descrip.at<uchar>(i, j) = rdDBvalue;
							}
						}
						dbDescriptors.push_back(descrip);
					}
					auto endRutTrich = steady_clock::now();
					fTime << "Rut trich: " << duration_cast<seconds>(endRutTrich - startRutTrich).count() << " seconds\n";
					SetDlgItemText(hWnd, IDC_STATUS, L"Load CSDL thành công!");
				}
				else
				{
					SetDlgItemText(hWnd, IDC_STATUS, L"Cơ sở dữ liệu không phù hợp, vui lòng tạo lại!");
				}
				fin.close();				
			}
		}
		break;

		case IDOK:
		{
			wofstream fTime;
			fTime.open("timeLog.txt", ios::app);
			auto start = steady_clock::now();
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
				fTime << "Yeu cau 1: ";
				if (dbHists.empty())
				{
					MessageBox(hWnd, L"Cơ sở dữ liệu đặc trừng màu rỗng", L"Vui lòng tạo hoặc load CSDL", MB_OK);
					return (INT_PTR)TRUE;
				}
				dbDescriptors.clear();
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
					if (srcHist.rows != dbHists[i].rows)
					{
						if (colorID == 1)
							colorID = 2;
						else
							colorID = 1;
						SetDlgItemText(hWnd, IDC_STATUS, L"Không cùng loại histogram, vui lòng chọn lại quantize");
						return (INT_PTR)TRUE;
					}
					dbValuesHist.push_back(compareHist(srcHist, dbHists[i], CV_COMP_CORREL));
				}
				wsprintf(str, L"Đang sắp xếp... ");
				SetDlgItemText(hWnd, IDC_STATUS, str);
				wofstream fout;
				fout.open("log_IDOK.txt", ios::out);
				//Sắp xếp mảng giảm giần từ giống nhiều về ít giống 
				shellSort(dbValuesHist, files,dbHists);
				for (int i = 0; i < dbHists.size();i++)
					fout << dbValuesHist[i] << "\t " << files[i] << endl;
				fout.close();
				wsprintf(str, L"Truy vấn hoàn thành");
				SetDlgItemText(hWnd, IDC_STATUS, str);

			}

			if (required == 2)
			{
				fTime << "Yeu cau 1: ";
				if (dbDescriptors.empty())
				{
					MessageBox(hWnd, L"Cơ sở dữ liệu đặc trưng hình dáng, cạnh rỗng", L"Vui lòng tạo hoặc load CSDL", MB_OK);
					return (INT_PTR)TRUE;
				}
				dbHists.clear();
				dbMatchs.clear();//Xóa dữ liệu mảng giá trị so khớp cũ
				Mat src = readImage(srcPath),img;
				if (src.empty())
				{
					SetDlgItemText(hWnd, IDC_STATUS, L"Nhập ảnh không hợp lệ!");
					return (INT_PTR)TRUE;
				}
				Mat srcDescriptor = getDesciptor(src);
				int normalSize, goodSize;
				for (int i = 0; i < dbDescriptors.size(); i++)
				{
					dbMatchs.push_back(getMatchValue(srcDescriptor, dbDescriptors[i], goodSize, normalSize));
					wsprintf(str, L"Descriptor process: %d / %d", i+1, files.size());
					SetDlgItemText(hWnd, IDC_STATUS, str);
				}
				wsprintf(str, L"Đang sắp xếp... ");
				SetDlgItemText(hWnd, IDC_STATUS, str);
				SetDlgItemText(hWnd, IDC_STATUS, str);
				wofstream fout;
				fout.open("log_IDOK.txt", ios::out);
				shellSort(dbMatchs, files,dbDescriptors);
				for (int i = 0; i < dbMatchs.size(); i++)
					fout << dbMatchs[i] << "\t " << files[i] << endl;
				fout.close();
				wsprintf(str, L"Truy vấn hoàn thành");
				SetDlgItemText(hWnd, IDC_STATUS, str);
			}

			auto end = steady_clock::now();
			fTime << "Thoi gian search: " << duration_cast<milliseconds>(end - start).count() << " milliseconds\n";
			wsprintf(str, L"Thời gian truy vấn: %d milliseconds", duration_cast<milliseconds>(end - start).count());
			SetDlgItemText(hWnd, IDC_STATUS, str);
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
	FlannBasedMatcher matcher; //Khởi tạo 1 biến matcher để thực hiện thuật toán FlannBased
	//Chuyển 2 descriptors về định dạng Float để chuẩn bị cho việc khai triển thuật toán FlannBased.
	if (descriptorsSrc.type() != CV_32F) {
		descriptorsSrc.convertTo(descriptorsSrc, CV_32F);
	}

	if (descriptorsDst.type() != CV_32F) {
		descriptorsDst.convertTo(descriptorsDst, CV_32F);
	}
	vector<vector<DMatch>> matches; //Tạo 1 mảng vector 2 chiều lưu trữ các matches.
	matcher.knnMatch(descriptorsSrc, descriptorsDst, matches, 2); //Tìm 2 matches tốt nhất cho mỗi descriptor để làm phép thử với second neighbor.
	vector< DMatch > good_matches;
	for (unsigned int i = 0; i < matches.size(); ++i)
	{
		if (matches[i][0].distance < matches[i][1].distance * 0.75) // kiểm tra nếu như match phù hợp nhất tỉ lệ gần hơn match phù hợp thứ 2 thì match đó sẽ đúng và sau đó sẽ được lưu vào vector good matches.
			good_matches.push_back(matches[i][0]);
	}
	goodSize = good_matches.size();
	normalSize = matches.size();
	return good_matches.size(); // Trả về số lượng điểm good match .
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
	Mat check;
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
				wstring s = PathName + L"\\" + find.cFileName;
				if(!readImage(s).empty())
					files.push_back(s);
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
