//=============================================================================
//
// メイン処理 [main.h]
// Author : GP11B243 24 人見友基
//
//=============================================================================
#ifndef _MAIN_H_
#define _MAIN_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <windows.h>
#include "d3dx9.h"
#include <stdio.h>
#include <time.h>

#define DIRECTINPUT_VERSION (0x0800)	// 警告対処用
#include "dinput.h"

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "winmm.lib")

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		("AppClass")				// ウインドウのクラス名
#define WINDOW_NAME		("DirectX9_3D_ProjectHEW")	// ウインドウのキャプション名

#define SCREEN_SCALE	(1.5f)
#define SCREEN_WIDTH	(1280 * SCREEN_SCALE)		// ウインドウの幅
#define SCREEN_HEIGHT	(720 * SCREEN_SCALE)		// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)			// ウインドウ中央X
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)			// ウインドウ中央Y

#define	NUM_VERTEX		(4)		// 頂点数
#define	NUM_POLYGON		(2)		// ポリゴン数

#define DEBUG_RECT		(50)	// デバッグ

#define MOUSE_MARGIN	(10)

// ３Ｄポリゴン頂点フォーマット
#define FVF_VERTEX_3D (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
// ２Ｄポリゴン頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{	// 3Dポリゴン頂点構造体
	D3DXVECTOR3	vtx;		// 頂点座標
	D3DXVECTOR3	nor;		// 法線ベクトル
	D3DCOLOR	diffuse;	// 反射光
	D3DXVECTOR2	tex;		// テクスチャ座標
} VERTEX_3D;

typedef struct
{	// 2Dポリゴン頂点構造体
	D3DXVECTOR3 vtx;		// 頂点座標
	float		rhw;		// テクスチャのパースペクティブコレクト用
	D3DCOLOR	diffuse;	// 反射光
	D3DXVECTOR2 tex;		// テクスチャ座標
} VERTEX_2D;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LPDIRECT3DDEVICE9 GetDevice(void);

#endif