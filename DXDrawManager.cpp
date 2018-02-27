#include "DXDrawManager.hpp"


#define Successful (1)
#define Failed (0)


DXDrawManager* DXDrawManager::CreateFull(HWND hwnd) {
	static DXDrawManager *ptr = new DXDrawManager();

	if (ptr->Create(hwnd, true, 0, 0) == Failed) {
		delete ptr;
		ptr = nullptr;
	}

	return ptr;
}


DXDrawManager* DXDrawManager::CreateWind(HWND hwnd, unsigned int screenW, unsigned int screenH) {
	static DXDrawManager *ptr = new DXDrawManager();
	if (ptr->Create(hwnd, true, 0, 0) == Failed) {
		delete ptr;
		ptr = nullptr;
	}

	return ptr;
}


void DXDrawManager::SetHand(bool isRight) {
	isRightHand = isRight;
}

void DXDrawManager::SetBackGround(unsigned int r, unsigned int g, unsigned int b) {
	// 最大値を255までにする
	r &= 0xff;
	g &= 0xff;
	b &= 0xff;
	SetBackGround(r|g|b);
}


void DXDrawManager::SetBackGround(unsigned long rgb) {
	rgb &= 0xffffff;
	backGroundColor = rgb;
}

void DXDrawManager::SetTextureFilter(D3DTEXTUREFILTERTYPE type) {
	d3ddev9->SetSamplerState(0, D3DSAMP_MAGFILTER, type);
	d3ddev9->SetSamplerState(0, D3DSAMP_MINFILTER, type);
}

void DXDrawManager::SetBlendMode(BLENDMODE mode) {
	switch (mode) {
		case BLENDMODE::NORMAL:
			break;
		case BLENDMODE::ADD:
			break;
		default:
			break;
	}

}

const char* DXDrawManager::GetErrMsg() const{
	return errMsg.c_str();
}


void DXDrawManager::SetLogMsgBuffer(std::string* ptr) {
	if (ptr) logMsg.reset(ptr);
}


void DXDrawManager::WriteErrMsg(const char *msg, ...) {
	char str[256];

	va_list list;
	va_start(list, msg);
	vsnprintf(str, 256, msg, list);
	va_end(list);


	errMsg.erase(errMsg.cbegin(), errMsg.cend());
	errMsg += str;
	WriteLogMsg(str);
}


void DXDrawManager::WriteLogMsg(const char *msg, ...) {
	if (!logMsg) return;

	using namespace std;

	char str[256];

	// 可変長引数を文字列に変換
	va_list list;
	va_start(list, msg);
	vsnprintf(str, 256, msg, list);
	va_end(list);

	// タイムスタンプ作成
	stringstream strTime;
	SYSTEMTIME time;
	GetLocalTime(&time);
	strTime << setw(2) << setfill('0') << '['
		<< time.wYear%100 << '/'
		<< time.wMonth << '/'
		<< time.wDay << ' '
		<< time.wHour << ':'
		<< time.wMinute << ':'
		<< time.wSecond << '.'
		<< time.wMilliseconds << '] ';
	*logMsg += strTime.str();

	*logMsg += msg;
	*logMsg += '\n';
}


DXDrawManager::DXDrawManager() {
}

DXDrawManager::~DXDrawManager() {
}

bool DXDrawManager::Create(HWND hwnd, bool isfull, unsigned int w, unsigned int h) {
	Delete();
	Clear();

	HRESULT ret;


	// Direct3D9オブジェクトの取得
	d3d9.reset(Direct3DCreate9(D3D_SDK_VERSION));
	if (d3d9 == nullptr) {
		WriteErrMsg("Failed to Create D3D9 Object");
		return false;
	}


	// ハードウェアの能力の取得
	ret = d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps9);
	if (FAILED(ret)) {
		WriteErrMsg("Failed to Get D3DCAPS9");
		return false;
	}


	// Direct3D9デバイス取得のための引数を設定
	d3dpresent.Windowed						= !isfull;
	d3dpresent.BackBufferWidth				= w;
	d3dpresent.BackBufferHeight				= h;
	if (isfull) {
		d3dpresent.BackBufferFormat			= D3DFMT_X8R8G8B8;
	}
	else {
		D3DDISPLAYMODE dispMode;
		ret = d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dispMode);
		if (FAILED(ret)) {
			WriteErrMsg("Failed to Get D3DDISPLAYMODE");
			return false;
		}
		d3dpresent.BackBufferFormat			= dispMode.Format;
	}
	d3dpresent.BackBufferCount				= 1;
	d3dpresent.MultiSampleType				= D3DMULTISAMPLE_NONE;
	d3dpresent.MultiSampleQuality			= 0;
	d3dpresent.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	d3dpresent.hDeviceWindow				= NULL;	// あとで指定
	d3dpresent.EnableAutoDepthStencil		= true;
	d3dpresent.AutoDepthStencilFormat		= D3DFMT_D24S8;
	d3dpresent.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	d3dpresent.PresentationInterval			= D3DPRESENT_INTERVAL_DEFAULT;




	/*
	Direct3D9デバイスの取得開始
	速度の早い順番でテスト

	1.ハードウェア処理&HW頂点処理
	2.ハードウェア処理&SW頂点処理
	3.ソフトウェア処理&HW頂点処理
	4.ソフトウェア処理&SW頂点処理
	*/
	for (int i=0; i<4; i++) {
		D3DDEVTYPE devtype;
		DWORD behaviorFlags;

		switch (i) {
			case 0:
				devtype = D3DDEVTYPE_HAL;
				behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
				break;
			case 1:
				devtype = D3DDEVTYPE_HAL;
				behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				break;
			case 2:
				devtype = D3DDEVTYPE_REF;
				behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
				break;
			case 3:
				devtype = D3DDEVTYPE_REF;
				behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
				break;
		}

		IDirect3DDevice9 *ptr;
		ret = d3d9->CreateDevice(D3DADAPTER_DEFAULT, devtype, hwnd, behaviorFlags, &d3dpresent, &ptr);
		if (SUCCEEDED(ret)) {
			d3ddev9.reset(ptr);

			switch (i) {
				case 0: WriteLogMsg("Succeeded to Create D3DDevice9 (DeviceType:GPU VertexProcessing:Hardware)"); break;
				case 1: WriteLogMsg("Succeeded to Create D3DDevice9 (DeviceType:GPU VertexProcessing:Software)"); break;
				case 2: WriteLogMsg("Succeeded to Create D3DDevice9 (DeviceType:CPU VertexProcessing:Hardware)"); break;
				case 3: WriteLogMsg("Succeeded to Create D3DDevice9 (DeviceType:CPU VertexProcessing:Software)"); break;
			}

			break;	// 作成終了
		}	

		if (i==3) {	// 最終ループなら
			WriteErrMsg("Failed to Create D3DDevice9");
			return false;
		}
	}


	// デフォルトステートのセット
	d3ddev9->SetRenderState( D3DRS_LIGHTING, FALSE );							// ライティング無効
	d3ddev9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);						// アルファブレンド使用可能
	d3ddev9->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );				// 転送元のブレンド設定
	d3ddev9->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );			// 転送先ブレンド設定
	if( isRightHand )
		d3ddev9->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );					// 右回りを消去(右手系)
	else
		d3ddev9->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );					// 左回りを消去(左手系)

	d3ddev9->SetSamplerState( 0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR );
	d3ddev9->SetSamplerState( 0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR );

	d3ddev9->SetTextureStageState( 0,D3DTSS_COLOROP,D3DTOP_MODULATE );
	d3ddev9->SetTextureStageState( 0,D3DTSS_COLORARG1,D3DTA_TEXTURE );
	d3ddev9->SetTextureStageState( 0,D3DTSS_COLORARG2,D3DTA_DIFFUSE );
	d3ddev9->SetTextureStageState( 0,D3DTSS_ALPHAOP,D3DTOP_MODULATE );
	d3ddev9->SetTextureStageState( 0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE );
	d3ddev9->SetTextureStageState( 0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE );


	return true;
}

void DXDrawManager::Delete() {
	d3d9.reset();
	d3ddev9.reset();
	logMsg.reset();
}

void DXDrawManager::Clear() {
	isRightHand = true;
	backGroundColor = 0xffffff;
	blendMode = BLENDMODE::NORMAL;
	logMsg->clear();
	logMsg->reserve(512);
	errMsg.clear();
	errMsg.reserve(128);
	ZeroMemory(&d3dcaps9, sizeof(d3dcaps9));
}
