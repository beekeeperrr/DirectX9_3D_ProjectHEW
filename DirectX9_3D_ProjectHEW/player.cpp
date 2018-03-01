//-------------------------------------------------------------------
//
// プレイヤー処理 [player.cpp]
// Author : GP11B243 28 堀口昌哉
//
//-------------------------------------------------------------------
#include "bullet.h"
#include "camera.h"
#include "debugproc.h"
#include "input.h"
#include "player.h"

//*******************************************************************
// プロトタイプ宣言
//*******************************************************************

//*******************************************************************
// グローバル変数
//*******************************************************************
LPDIRECT3DTEXTURE9	D3DTexturePlayer[PLAYER_MAX];		// テクスチャ読み込み場所
LPD3DXMESH			D3DXMeshPlayer[PLAYER_MAX];			// ID3DXMeshインターフェイスへのポインタ
LPD3DXBUFFER		D3DXMatBuffPlayer[PLAYER_MAX];		// メッシュのマテリアル情報を格納
DWORD				NumMatPlayer[PLAYER_MAX];			// 属性情報の総数

D3DXMATRIX			MtxWorldPlayer;						// ワールドマトリックス
PLAYER				PlayerWk[PLAYER_MAX];				// プレイヤーワーク

char *FileNamePlayer[PLAYER_MAX] =
{
	"data/MODEL/PLAYER/player01.x",
	"data/MODEL/PLAYER/player02.x"
};

//===================================================================
// 初期化処理
//===================================================================
HRESULT InitPlayer(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	PLAYER *player = &PlayerWk[0];

	for (int i = 0; i < PLAYER_MAX; i++)
	{

		D3DTexturePlayer[i] = NULL;
		D3DXMeshPlayer[i] = NULL;
		D3DXMatBuffPlayer[i] = NULL;

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(FileNamePlayer[i],		// モデルデータ
			D3DXMESH_SYSTEMMEM,								// 使用するメモリのオプション
			Device,											// デバイス
			NULL,											// 未使用
			&D3DXMatBuffPlayer[i],							// マテリアルデータへのポインタ
			NULL,											// 未使用
			&NumMatPlayer[i],								// D3DXMATERIAL構造体の数
			&D3DXMeshPlayer[i])))							// メッシュデータへのポインタ
		{
			return E_FAIL;
		}

#if 0
		//テクスチャの読み込み
		D3DXCreateTextureFromFile(Device,
			TEXTURE_FILENAME,
			&D3DTexturePlayer);
#endif
	}

	//プレイヤーの初期化処理
	for (int i = 0; i < PLAYER_MAX; i++, player++)
	{
		player->use = true;											// useフラグをtrueに
		player->pos = D3DXVECTOR3((i+1)*165.0f, 0.0f, 100.0f);		// 位置
		player->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 角度
		player->scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);				// スケール
		player->radius = PLAYER_RADIUS;								// 半径
		player->item = 0.0f;										// アイテムを0に
	}

	return S_OK;
}

//===================================================================
// 終了処理
//===================================================================
void UninitPlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (D3DTexturePlayer[i] != NULL)
		{
			// テクスチャの開放
			D3DTexturePlayer[i]->Release();
			D3DTexturePlayer[i] = NULL;
		}

		if (D3DXMeshPlayer[i] != NULL)
		{
			// メッシュの開放
			D3DXMeshPlayer[i]->Release();
			D3DXMeshPlayer[i] = NULL;
		}

		if (D3DXMatBuffPlayer[i] != NULL)
		{
			// マテリアルの開放
			D3DXMatBuffPlayer[i]->Release();
			D3DXMatBuffPlayer[i] = NULL;
		}
	}
}

//===================================================================
// 更新処理
//===================================================================
void UpdatePlayer(void)
{
	PLAYER *player = &PlayerWk[0];
	CAMERA *camera = GetCamera();
	D3DXVECTOR3 rotCamera;

	// カメラの向き取得
	rotCamera = GetRotCamera();

	// 移動処理
	if (GetKeyboardPress(DIK_A))
	{
		for (int i = 0;i < PLAYER_MAX;i++, player++)
		{
			player->pos.x -= VALUE_MOVE_PLAYER;
			player->rot.y = rotCamera.y + D3DX_PI * 0.5f;
		}
	}
	else if (GetKeyboardPress(DIK_D))
	{
		for (int i = 0;i < PLAYER_MAX;i++, player++)
		{
			player->pos.x += VALUE_MOVE_PLAYER;
			player->rot.y = rotCamera.y - D3DX_PI * 0.5f;
		}
	}
	
	if (GetKeyboardPress(DIK_W))
	{
		for (int i = 0;i < PLAYER_MAX;i++, player++)
		{
			player->pos.z += VALUE_MOVE_PLAYER;
			player->rot.y = rotCamera.y + D3DX_PI * 1.0f;
		}
	}
	else if (GetKeyboardPress(DIK_S))
	{
		for (int i = 0;i < PLAYER_MAX;i++, player++)
		{
			player->pos.z -= VALUE_MOVE_PLAYER;
			player->rot.y = rotCamera.y + D3DX_PI * 0.0f;
		}
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_Q))
	{
		player->pos.y += VALUE_MOVE_PLAYER;
	}
	else if (GetKeyboardPress(DIK_E))
	{
		player->pos.y -= VALUE_MOVE_PLAYER;
	}
#endif


#ifdef _DEBUG
		PrintDebugProc("[プレイヤー座標 ：(X:%f Y: %f Z: %f)]\n", player->pos.x, player->pos.y, player->pos.z);
		PrintDebugProc("プレイヤー移動 : WSADQE : 前後左右上下\n");
		PrintDebugProc("\n");
#endif


	////弾発射処理
	//if (GetKeyboardTrigger(DIK_SPACE))
	//{


	//}
}

//===================================================================
// 描画処理
//===================================================================
void DrawPlayer(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX mtxRot, mtxTranslate, mtxScale;
	D3DXMATERIAL *D3DXMat;
	D3DMATERIAL9 matDef;

	PLAYER *player = &PlayerWk[0];

	for (int i = 0; i < PLAYER_MAX; i++, player++)
	{
		if (player->use == true)
		{
			// ライトをON
			Device->SetRenderState(D3DRS_LIGHTING, TRUE);

			// ワールドマトリクスの初期化
			D3DXMatrixIdentity(&MtxWorldPlayer);

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, player->scl.x, player->scl.y, player->scl.z);
			D3DXMatrixMultiply(&MtxWorldPlayer, &MtxWorldPlayer, &mtxScale);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, player->rot.y, player->rot.x, player->rot.z);
			D3DXMatrixMultiply(&MtxWorldPlayer, &MtxWorldPlayer, &mtxRot);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, player->pos.x, player->pos.y, player->pos.z);
			D3DXMatrixMultiply(&MtxWorldPlayer, &MtxWorldPlayer, &mtxTranslate);

			// ワールドマトリクスの設定
			Device->SetTransform(D3DTS_WORLD, &MtxWorldPlayer);

			// 現在のマテリアルを取得
			Device->GetMaterial(&matDef);

			// マテリアル情報に対するポインタを取得
			D3DXMat = (D3DXMATERIAL*)D3DXMatBuffPlayer[i]->GetBufferPointer();

			for (int cntMat = 0; cntMat < (int)NumMatPlayer[i]; cntMat++)
			{
				// マテリアルの設定
				Device->SetMaterial(&D3DXMat[cntMat].MatD3D);

				// テクスチャの設定
				Device->SetTexture(0, D3DTexturePlayer[i]);

				// 描画
				D3DXMeshPlayer[i]->DrawSubset(cntMat);
			}

			// ライトをOFF
			Device->SetRenderState(D3DRS_LIGHTING, FALSE);
		}
	}


	// マテリアルをデフォルトに戻す
	{
		D3DXMATERIAL mat;

		mat.MatD3D.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		mat.MatD3D.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		mat.MatD3D.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

		Device->SetMaterial(&mat.MatD3D);
	}

}

//===================================================================
// プレイヤー取得
//===================================================================
PLAYER *GetPlayer(int no)
{
	return &PlayerWk[no];
}

//===================================================================
// 位置取得
//===================================================================
D3DXVECTOR3 GetPosPlayer(void)
{
	PLAYER *player = &PlayerWk[0];
	return player->pos;
}

//===================================================================
// 向き取得
//===================================================================
D3DXVECTOR3 GetRotPlayer(void)
{
	PLAYER *player = &PlayerWk[0];
	return player->rot;
}
