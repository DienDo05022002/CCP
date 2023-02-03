#include <iostream>
#include <string>
#include <windows.h>
#include <dshow.h>
#include <cstdio>
#include <map>
#include <conio.h>
#include <thread>

#pragma comment(lib, "strmiids.lib")

using namespace std;
int cnt = 0; int check = 0; //check = 1 là đang chạy, check = 2 là tạm dừng, check = 0 là thoát bài.
map<int, wstring> MyFileMP3;

class Mp3 {
protected: 
	IGraphBuilder *pigb ;
	IMediaControl *pimc ;
	IMediaEventEx *pimex ;
	IMediaSeeking * pims;             //IMediaSeeking để lấy thời gian tổng va thời gian hiện tại của bài hát.
	__int64 duration;
	REFERENCE_TIME currentTime;
public:
	Mp3();
	~Mp3();

	bool Load(LPCWSTR file);
	void Cleanup();
	void GetFile();

	bool Play();
	bool Pause();
	bool Stop();
};

//Bắt đầu 
Mp3::Mp3() {
	pigb = NULL;
	pimc = NULL;
	pimex = NULL;
	pims = NULL;
	duration = 0;
	currentTime = 0;
}
Mp3::~Mp3() {
	Cleanup();
	cout << "Thanks you so much for using :))" << endl;
}

void Mp3::Cleanup() {
	//Release các đối tượng và giải phóng tài nguyên 
	pigb->Release();
	pimc->Release();
	pimex->Release();
	pims->Release();
	CoUninitialize();
}
bool Mp3::Load(LPCWSTR file) {
	// Khởi tạo thư viện COM bằng cách gọi CoInitializ.
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		cout << "ERROR SEVER" << endl;
	}
	// gọi CoCreateInstance để tạo Filter Graph Manager và lấy các interface cần thiết
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pigb);
	if (FAILED(hr)) {
		cout << "ERROR - Could not create the Filter Graph Manager." << endl;
	}
	hr = pigb->QueryInterface(IID_IMediaControl, (void **)&pimc);
	hr = pigb->QueryInterface(IID_IMediaEventEx, (void **)&pimex);
	hr = pigb->QueryInterface(IID_IMediaSeeking, (void**)&pims);
	if (SUCCEEDED(hr))

	// Xây đò thị âm thanh và chơi nhạc. file là tham số của hàm Load nhận được truyền vào
	hr = pigb->RenderFile(file, NULL);
	if (SUCCEEDED(hr)) {
		cout << "### Detail-Song ###" << endl;
		// pims lấy thời gian tổng va thời gian hiện tại của bài hát.
		if (pims) {
			pims->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME);
			pims->GetDuration(&duration); // returns 10,000,000 trong một giây.
			if (SUCCEEDED(hr)) {
				cout << "Total time: " << duration / 10000000.0 << " second" << endl;
			}
			//hr = pims->GetCurrentPosition(&currentTime);
			hr = pims->GetCurrentPosition(&currentTime);
			if (SUCCEEDED(hr)) {
				cout << "Current Time: " << currentTime / 10000000.0 << " second" << endl;
				//return currentTime;
			}
		}
		// chạy nhạc
		//hr = pimc->Run();
		//if (SUCCEEDED(hr)) {
			// Chờ đến kkhi hoàn thành
			//long evCode;
			//pimex->WaitForCompletion(INFINITE, &evCode);
		//}
	}
	//Cleanup();
	return true;
}
void Mp3::GetFile() {
	system("cls");
	//Mp3 mp3;
//================================================================================================
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(TEXT("C:\\Users\\VAIO\\Downloads\\MP3\\*.mp3"), &ffd);
	if (hFind == INVALID_HANDLE_VALUE){
		cout << "Error: " << GetLastError() << endl;
	}
	wstring filepath = L"C:\\Users\\VAIO\\Downloads\\MP3\\";
	cout << endl << "------------------------LIST SONG------------------------\n";
	do {
		++cnt;
		wstring file = filepath + ffd.cFileName;
		wcout << "|" << cnt << ":  " << ffd.cFileName << endl;

		// Dùng Pair-map để nạp thông tin
		MyFileMP3.insert(pair<int, wstring>(cnt, file));
		//***Lưu ý: kiểu wstring là chuỗi rộng còn LPCWSTR là con trỏ nhận kiểu dữ liệu chuỗi rộng (wstring)***
		//vì file được lưu bằng kiểu wtring nên dùng hàm c_str() để trả về một con trỏ, vì hàm DirectShow nhận một con trỏ có kiểu LPCWSTR
	} while (FindNextFile(hFind, &ffd) != 0);

	cout << endl << "----------------------------END--------------------------\n";
	
	FindClose(hFind);
}
bool Mp3::Play() {
	if (pimc) {
		cout << endl << "Playing.... " << endl;
		cout << "Press Space to pause:: ";
		HRESULT hr = pimc->Run(); check = 1;
		return SUCCEEDED(hr);
	}
	return false;
}
bool Mp3::Pause() {
	if (pimc) {
		cout << endl << "Pause! Press Space to playing:: ";
		HRESULT hr = pimc->Pause(); check = 2;
		return SUCCEEDED(hr);
	}
	return false;
}
bool Mp3::Stop() {
	if (pimc) {
		HRESULT hr = pimc->Stop(); check = 0;
		return SUCCEEDED(hr); 
	}
	return false;
}
void taskThread() {
	Mp3 mp3; mp3.Play();
}
bool WaitForCompletion() {
	thread task(taskThread);
	cout << ".." << endl;
	task.join();
	return false;
}

// Điều khiển ứng dụng tù bàn phím
class ConsoleApp{
public:
	void ConsoleAppMp3();
};

void ConsoleApp::ConsoleAppMp3() {
	Mp3 mp3;
	mp3.GetFile();
	cout << "Choice 0 to exit the application! " << endl;
	int n;
	do {
		cout << endl << "=========================================================" << endl;
		cout << "Choose a song: ";
		cin >> n;
		if (MyFileMP3.count(n)) {
			//Dùng hàm count để tìm truy cập vào gtri trong Map bằng [n];
			if (mp3.Load(MyFileMP3[n].c_str())) {
				bool flag = true;
				char ch; cout << "Press enter to stop song. " << endl;
				while (WaitForCompletion) {
					mp3.Play();
					ch = _getch();              // sau đó dùng getch() để bắt phím vừa nhấn
					if (ch == 13){             // mã ASCII: 32 = space
						flag = !flag;
					}
					if (flag == true) {
						mp3.Pause(); _getch();
					}
					else {
						mp3.Stop();
						cout << "Stop" << endl;
						break;
					}
				}
				cout << WaitForCompletion() << endl;
 				cout << "###End Song### :))" << endl;
				cout << "=========================================================" << endl;
			}
		}
		else cout << "Choice does not exist! Enter again: " << endl;
	} while (n != 0);
}
//==================================================================END==================================================================

int main() {
	ConsoleApp App;
	App.ConsoleAppMp3();
	return 0;
}