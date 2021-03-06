//=============================================================================
//
// タイトル画面処理 [titlelogo.cpp]
// Author : GP11B243 34 吉田太智
//
//=============================================================================
#include "titlelogo.h"
#include "input.h"
#include "fade.h"
#include "stage.h"
#include "sound.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexTitlelogo(LPDIRECT3DDEVICE9 pDevice);
void SetColorTitlelogoLogo(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureTitlelogo = NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffTitlelogo = NULL;		// 頂点バッファインターフェースへのポインタ
LPDIRECT3DTEXTURE9		g_pD3DTextureTitlelogoLogo = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffTitlelogoLogo = NULL;	// 頂点バッファインターフェースへのポインタ
LPDIRECT3DTEXTURE9		g_pD3DTextureStart = NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffStart = NULL;		// 頂点バッファインターフェースへのポインタ
int						g_nCountAppearStart = 0;		//
int						g_nConutDemo = 0;				//
int						g_nCountDisp = 0;				//
float					g_fAlphaLogo = 0.0f;			//
bool					g_bDispStart = false;			//
bool					g_bTitleFade;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitlelogo(int nType)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	g_nCountAppearStart = 0;
	g_nCountDisp = 0;
	g_nConutDemo = 0;
	g_fAlphaLogo = 0.0f;
	g_bDispStart = false;
	g_bTitleFade = false;

	// 頂点情報の作成
	MakeVertexTitlelogo(pDevice);

	if (nType == STAGE_INIT_FAST)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
			TEXTURE_TITLELOGO,				// ファイルの名前
			&g_pD3DTextureTitlelogo);		// 読み込むメモリー

		D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
			TEXTURE_TITLELOGO_LOGO,			// ファイルの名前
			&g_pD3DTextureTitlelogoLogo);	// 読み込むメモリー


		D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
			TEXTURE_LOGO_START,			// ファイルの名前
			&g_pD3DTextureStart);		// 読み込むメモリー
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitlelogo(void)
{
	if(g_pD3DTextureTitlelogo != NULL)
	{// テクスチャの開放
		g_pD3DTextureTitlelogo->Release();
		g_pD3DTextureTitlelogo = NULL;
	}

	if(g_pD3DVtxBuffTitlelogo != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffTitlelogo->Release();
		g_pD3DVtxBuffTitlelogo = NULL;
	}

	if(g_pD3DTextureTitlelogoLogo != NULL)
	{// テクスチャの開放
		g_pD3DTextureTitlelogoLogo->Release();
		g_pD3DTextureTitlelogoLogo = NULL;
	}

	if(g_pD3DVtxBuffTitlelogoLogo != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffTitlelogoLogo->Release();
		g_pD3DVtxBuffTitlelogoLogo = NULL;
	}

	if(g_pD3DTextureStart != NULL)
	{// テクスチャの開放
		g_pD3DTextureStart->Release();
		g_pD3DTextureStart = NULL;
	}

	if(g_pD3DVtxBuffStart != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffStart->Release();
		g_pD3DVtxBuffStart = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitlelogo(void)
{
#if 0
	if(g_nCountAppearStart >= COUNT_APPERA_START)
	{
		g_nConutDemo++;
		if(g_nConutDemo > COUNT_WAIT_DEMO)
		{
			SetFade(FADE_OUT, MODE_TITLELOGO);
		}
	}
#endif

	if(g_fAlphaLogo < 1.0f)
	{
		g_fAlphaLogo += 0.005f;
		if(g_fAlphaLogo >= 1.0f)
		{
			g_fAlphaLogo = 1.0f;
		}
		SetColorTitlelogoLogo();
	}
	else
	{
		g_nCountAppearStart++;
		if(g_nCountAppearStart > COUNT_APPERA_START)
		{
			g_nCountDisp = (g_nCountDisp + 1) % 80;
			if(g_nCountDisp > INTERVAL_DISP_START)
			{
				g_bDispStart = false;
			}
			else
			{
				g_bDispStart = true;
			}
		}
	}

	// エンターキーでステージ遷移
	if(GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(1, BUTTON_C) || IsButtonTriggered(0, BUTTON_C))
	{
		if(g_nCountAppearStart == 0)
		{// タイトル登場スキップ
			g_fAlphaLogo = 1.0f;
			SetColorTitlelogoLogo();

			g_nCountAppearStart = COUNT_APPERA_START;
		}
		else
		{// ゲームへ
			if (!g_bTitleFade)
			{
				g_bTitleFade = true;
				SetSe(SE_BUTTON, E_DS8_FLAG_NONE, CONTINUITY_ON);
				SetFade(FADE_OUT, STAGE_GAME);
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitlelogo(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// 頂点バッファをデバイスのデータストリームにバインド
    pDevice->SetStreamSource(0, g_pD3DVtxBuffTitlelogo, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pD3DTextureTitlelogo);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);


	// 頂点バッファをデバイスのデータストリームにバインド
    pDevice->SetStreamSource(0, g_pD3DVtxBuffTitlelogoLogo, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pD3DTextureTitlelogoLogo);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);

	if(g_bDispStart == true)
	{
		// 頂点バッファをデバイスのデータストリームにバインド
		pDevice->SetStreamSource(0, g_pD3DVtxBuffStart, 0, sizeof(VERTEX_2D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pD3DTextureStart);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
	}
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexTitlelogo(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
												FVF_VERTEX_2D,				// 使用する頂点フォーマット
												D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffTitlelogo,		// 頂点バッファインターフェースへのポインタ
												NULL)))						// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffTitlelogo->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

		// テクスチャのパースペクティブコレクト用
		pVtx[0].rhw =
		pVtx[1].rhw =
		pVtx[2].rhw =
		pVtx[3].rhw = 1.0f;

		// 反射光の設定
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データをアンロックする
		g_pD3DVtxBuffTitlelogo->Unlock();
	}

	// オブジェクトの頂点バッファを生成
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
												FVF_VERTEX_2D,				// 使用する頂点フォーマット
												D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffTitlelogoLogo,	// 頂点バッファインターフェースへのポインタ
												NULL)))						// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffTitlelogoLogo->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(TITLELOGO_LOGO_POS_X - TITLELOGO_LOGO_WIDTH, TITLELOGO_LOGO_POS_Y - TITLELOGO_LOGO_HEIGHT, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(TITLELOGO_LOGO_POS_X + TITLELOGO_LOGO_WIDTH, TITLELOGO_LOGO_POS_Y - TITLELOGO_LOGO_HEIGHT, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(TITLELOGO_LOGO_POS_X - TITLELOGO_LOGO_WIDTH, TITLELOGO_LOGO_POS_Y + TITLELOGO_LOGO_HEIGHT, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(TITLELOGO_LOGO_POS_X + TITLELOGO_LOGO_WIDTH, TITLELOGO_LOGO_POS_Y + TITLELOGO_LOGO_HEIGHT, 0.0f);

		// テクスチャのパースペクティブコレクト用
		pVtx[0].rhw =
		pVtx[1].rhw =
		pVtx[2].rhw =
		pVtx[3].rhw = 1.0f;

		// 反射光の設定
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データをアンロックする
		g_pD3DVtxBuffTitlelogoLogo->Unlock();
	}


	// オブジェクトの頂点バッファを生成
    if(FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
												FVF_VERTEX_2D,				// 使用する頂点フォーマット
												D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffStart,		// 頂点バッファインターフェースへのポインタ
												NULL)))						// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffStart->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(START_POS_X - START_WIDTH, START_POS_Y - START_HEIGHT, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(START_POS_X + START_WIDTH, START_POS_Y - START_HEIGHT, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(START_POS_X - START_WIDTH, START_POS_Y + START_HEIGHT, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(START_POS_X + START_WIDTH, START_POS_Y + START_HEIGHT, 0.0f);

		// テクスチャのパースペクティブコレクト用
		pVtx[0].rhw =
		pVtx[1].rhw =
		pVtx[2].rhw =
		pVtx[3].rhw = 1.0f;

		// 反射光の設定
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データをアンロックする
		g_pD3DVtxBuffStart->Unlock();
	}

	return S_OK;
}

//=============================================================================
// 頂点の作成
//=============================================================================
void SetColorTitlelogoLogo(void)
{
	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffTitlelogoLogo->Lock(0, 0, (void**)&pVtx, 0);

		// 反射光の設定
		pVtx[0].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);
		pVtx[1].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);
		pVtx[2].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_fAlphaLogo);

		// 頂点データをアンロックする
		g_pD3DVtxBuffTitlelogoLogo->Unlock();
	}

}

