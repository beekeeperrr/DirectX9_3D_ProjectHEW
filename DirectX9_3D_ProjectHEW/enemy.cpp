//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : GP11B243-18-千坂浩太
//
//=============================================================================
#include "camera.h"
#include "debugproc.h"
#include "input.h"
#include "player.h"
#include "enemy.h"
#include "stage.h"
#include "shadow.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetEnemyAnimation(int sec);
D3DXMATRIX* EnemyLookAtMatrix(D3DXMATRIX *pout, D3DXVECTOR3 *pEye, D3DXVECTOR3 *pAt, D3DXVECTOR3 *pUp);
void SetEnemyHoming(int no, int frequency, float speedup);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9	g_pD3DTextureEnemy[ENEMY_ANIM_MAX];			// テクスチャ読み込み場所
LPD3DXMESH			g_pD3DXMeshEnemy[ENEMY_ANIM_MAX];			// ID3DXMeshインターフェイスへのポインタ
LPD3DXBUFFER		g_pD3DXBuffMatEnemy[ENEMY_ANIM_MAX];		// メッシュのマテリアル情報を格納
DWORD				g_nNumMatEnemy[ENEMY_ANIM_MAX];				// 属性情報の総数

D3DXMATRIX			g_mtxWorldEnemy;							// ワールドマトリックス

float				g_fSizeShadowE;								// 影のサイズ
D3DXCOLOR			g_colShadowE;								// 影の色

ENEMY				enemyWk[ENEMY_MAX];							// エネミー格納ワーク

int					animCnt;									// アニメカウント
int					key;										// フレームカウント
int					sp_Update;									// 更新頻度計算用


const char *FileNameEnemy[ENEMY_ANIM_MAX] =
{
	"data/MODEL/ENEMY/enemy_a00.x",		// 直立
	"data/MODEL/ENEMY/enemy_a01.x",		// 左足前１
	"data/MODEL/ENEMY/enemy_a02.x",		// 左足前２
	"data/MODEL/ENEMY/enemy_a03.x",		// 左足前３
	"data/MODEL/ENEMY/enemy_a02.x",		// 左足前２
	"data/MODEL/ENEMY/enemy_a01.x",		// 左足前１
	"data/MODEL/ENEMY/enemy_a00.x",		// 直立
	"data/MODEL/ENEMY/enemy_a11.x",		// 右足前１
	"data/MODEL/ENEMY/enemy_a12.x",		// 右足前２
	"data/MODEL/ENEMY/enemy_a13.x",		// 右足前３
	"data/MODEL/ENEMY/enemy_a12.x",		// 右足前２
	"data/MODEL/ENEMY/enemy_a11.x"		// 右足前１

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(int nType)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	ENEMY *enemy = &enemyWk[0];

	if (nType == STAGE_INIT_FAST)
	{
		for (int nCntEnemyAnim = 0; nCntEnemyAnim < ENEMY_ANIM_MAX; nCntEnemyAnim++)
		{
			g_pD3DTextureEnemy[nCntEnemyAnim] = NULL;
			g_pD3DXMeshEnemy[nCntEnemyAnim] = NULL;
			g_pD3DXBuffMatEnemy[nCntEnemyAnim] = NULL;


			// Xファイルの読み込み
			if (FAILED(D3DXLoadMeshFromX(FileNameEnemy[nCntEnemyAnim],
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				&g_pD3DXBuffMatEnemy[nCntEnemyAnim],
				NULL,
				&g_nNumMatEnemy[nCntEnemyAnim],
				&g_pD3DXMeshEnemy[nCntEnemyAnim])))
			{
				return E_FAIL;
			}

#if 0
			// テクスチャの読み込み
			D3DXCreateTextureFromFile(pDevice,					// デバイスへのポインタ
				TEXTURE_FILENAME,		// ファイルの名前
				&g_pD3DTextureModel);	// 読み込むメモリー
#endif

		}

	}

	// エネミーの初期化処理
	for (int i = 0; i < ENEMY_MAX; i++, enemy++)
	{
		PANEL *panel = GetPanel(GetPanelNumber(1, 8));
		// エネミーの視点の初期化
		enemy->Eye = panel->Pos;
		// エネミーの注視点の初期化
		enemy->At = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		// エネミーの上方向の初期化
		enemy->Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// エネミーの向きの初期化
		enemy->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		// エネミーの移動量の初期化
		enemy->move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// エネミーのスケールの初期化
		enemy->scl = D3DXVECTOR3(0.9f, 0.9f, 0.9f);

		// useフラグをtrueに設定
		enemy->use = true;

		// エネミーの移動速度初期化
		enemy->speed = VALUE_MOVE_ENEMY;

		// アニメーション番号初期化
		// 最初は直立状態に設定
		enemy->anim = 0;

		// アニメーションカウント初期化
		animCnt = 0;

		// 更新頻度初期化
		sp_Update = 0;

		// 追尾プレイヤー番号初期化
		key = 0;
		// ランダムで最初に追尾するプレイヤーを選ぶ
		key = rand() % PLAYER_MAX;

		// シャドウ用
		enemy->nIdxShadow = 0;
		enemy->fSizeShadow = 0.0f;
		enemy->colShadow = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.8f);
		enemy->bShadow = false;
	}



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	for (int nCntEnemyAnim = 0; nCntEnemyAnim < ENEMY_ANIM_MAX; nCntEnemyAnim++)
	{
		if (g_pD3DTextureEnemy[nCntEnemyAnim] != NULL)
		{// テクスチャの開放
			g_pD3DTextureEnemy[nCntEnemyAnim]->Release();
			g_pD3DTextureEnemy[nCntEnemyAnim] = NULL;
		}

		if (g_pD3DXMeshEnemy[nCntEnemyAnim] != NULL)
		{// メッシュの開放
			g_pD3DXMeshEnemy[nCntEnemyAnim]->Release();
			g_pD3DXMeshEnemy[nCntEnemyAnim] = NULL;
		}

		if (g_pD3DXBuffMatEnemy[nCntEnemyAnim] != NULL)
		{// マテリアルの開放
			g_pD3DXBuffMatEnemy[nCntEnemyAnim]->Release();
			g_pD3DXBuffMatEnemy[nCntEnemyAnim] = NULL;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	ENEMY *enemy = &enemyWk[0];
	CAMERA *camera = GetCamera();
	PANEL *panel = GetPanel(0);
	PLAYER *player = GetPlayer(0);
	
	// アニメーション
	SetEnemyAnimation(ENEMY_ANIM_SEC);


	// ボタンで追尾対象切り替える
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_1))
	{
		key = 0;
	}
	if (GetKeyboardTrigger(DIK_2))
	{
		key = 1;
	}
#endif


	// ゲージの少ないほうを追尾
	if (player[0].item < player[1].item)
	{	// 1Pのほうがゲージが少ない場合
		
		// 1Pを追尾
		key = 0;
	}
	if (player[0].item > player[1].item)
	{	// 2Pのほうがゲージが少ない場合

		// 2Pを追尾
		key = 1;
	}
	// 追尾をセット
	SetEnemyHoming(key, ENEMY_SPEED_FREQUENCY, ENEMY_SPEEDUP);


	// デバッグ時に手動でエネミー移動
#ifdef _DEBUG
	if(GetKeyboardPress(DIK_LEFT))
	{
		if(GetKeyboardPress(DIK_UP))
		{// 左前移動
			enemy->move.x -= cosf(camera->rotCamera.y + D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
			enemy->move.z += sinf(camera->rotCamera.y + D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
		}
		else if(GetKeyboardPress(DIK_DOWN))
		{// 左後移動
			enemy->move.x -= cosf(camera->rotCamera.y - D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
			enemy->move.z += sinf(camera->rotCamera.y - D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
		}
		else
		{// 左移動
			enemy->move.x -= cosf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
			enemy->move.z += sinf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
		}
	}
	else if(GetKeyboardPress(DIK_RIGHT))
	{
		if(GetKeyboardPress(DIK_UP))
		{// 右前移動
			enemy->move.x += cosf(camera->rotCamera.y - D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
			enemy->move.z -= sinf(camera->rotCamera.y - D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
		}
		else if(GetKeyboardPress(DIK_DOWN))
		{// 右後移動
			enemy->move.x += cosf(camera->rotCamera.y + D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
			enemy->move.z -= sinf(camera->rotCamera.y + D3DX_PI * 0.25f) * VALUE_MOVE_ENEMY;
		}
		else
		{// 右移動
			enemy->move.x += cosf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
			enemy->move.z -= sinf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
		}
	}
	else if(GetKeyboardPress(DIK_UP))
	{// 前移動
		enemy->move.x += sinf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
		enemy->move.z += cosf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
	}
	else if(GetKeyboardPress(DIK_DOWN))
	{// 後移動
		enemy->move.x -= sinf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
		enemy->move.z -= cosf(camera->rotCamera.y) * VALUE_MOVE_ENEMY;
	}




#endif

	// エネミー移動処理

	// 移動量に慣性をかける
	enemy->move.x += (0.0f - enemy->move.x) * RATE_MOVE_ENEMY;
	enemy->move.y += (0.0f - enemy->move.y) * RATE_MOVE_ENEMY;
	enemy->move.z += (0.0f - enemy->move.z) * RATE_MOVE_ENEMY;

	/// 位置移動
	enemy->Eye.x += enemy->move.x;
	enemy->Eye.y += enemy->move.y;
	//if (enemy->Eye.y < 5.0f)
	//{
	//	enemy->Eye.y = 5.0f;
	//}
	//if (enemy->Eye.y > 75.0f)
	//{
	//	enemy->Eye.y = 75.0f;
	//}
	enemy->Eye.z += enemy->move.z;


	// エネミーの移動制限（場外に行かないようにする）
	// Z座標のマックスとX座標のマックスで制限かける
	panel = GetPanel(GetPanelNumber(PANEL_NUM_Z, PANEL_NUM_X));		// 右上
	if (enemy->Eye.x > panel->Pos.x)
	{
		enemy->Eye.x = panel->Pos.x;
	}
	if (enemy->Eye.z > panel->Pos.z)
	{
		enemy->Eye.z = panel->Pos.z;
	}

	// Z座標のミニマムとX座標のミニマムで制限かける
	panel = GetPanel(GetPanelNumber(1, 1));							// 左下
	if (enemy->Eye.x < -panel->Pos.x)
	{
		enemy->Eye.x = -panel->Pos.x;
	}
	if (enemy->Eye.z < -panel->Pos.z)
	{
		enemy->Eye.z = -panel->Pos.z;
	}


	enemy = &enemyWk[0];
#ifdef _DEBUG
	PrintDebugProc("[エネミーの位置  ：(%f : %f : %f)]\n", enemy->Eye.x, enemy->Eye.y, enemy->Eye.z);
	//PrintDebugProc("\n");
#endif

	// シャドウ
	if (!enemy->bShadow)
	{	// シャドウ設置
		enemy->nIdxShadow = CreateShadow(enemy->Eye, 25.0f, 25.0f);
		enemy->fSizeShadow = ENEMY_SHADOW_SIZE;
		enemy->colShadow = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
		enemy->bShadow = true;
	}
	else
	{
		// シャドウ管理
		SetPositionShadow(enemy->nIdxShadow, D3DXVECTOR3(enemy->Eye.x, 0.2f, enemy->Eye.z));
		SetVertexShadow(enemy->nIdxShadow, enemy->fSizeShadow, enemy->fSizeShadow);
		SetColorShadow(enemy->nIdxShadow, enemy->colShadow);
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxRot, mtxTranslate, mtxScale;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	ENEMY *enemy = &enemyWk[0];

	for (int i = 0; i < ENEMY_MAX; i++, enemy++)
	{
		if (enemy->use == true)	// 使用状態なら描画する
		{
			// ライトをon
			pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldEnemy);

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, enemy->scl.x,
				enemy->scl.y,
				enemy->scl.z);
			D3DXMatrixMultiply(&g_mtxWorldEnemy,
				&g_mtxWorldEnemy, &mtxScale);


			// 回転を反映
			//D3DXMatrixRotationYawPitchRoll(&mtxRot, enemy->rot.y, enemy->rot.x, enemy->rot.z);
			//D3DXMatrixMultiply(&g_mtxWorldEnemy, &g_mtxWorldEnemy, &mtxRot);
			EnemyLookAtMatrix(&mtxRot, &enemy->Eye, &enemy->At, &enemy->Up);

			D3DXMatrixMultiply(&g_mtxWorldEnemy, &g_mtxWorldEnemy, &mtxRot);
			
			//// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, enemy->Eye.x, enemy->Eye.y, enemy->Eye.z);
			D3DXMatrixMultiply(&g_mtxWorldEnemy, &g_mtxWorldEnemy, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldEnemy);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアル情報に対するポインタを取得
			// 今は直立を設定してる０
			pD3DXMat = (D3DXMATERIAL*)g_pD3DXBuffMatEnemy[enemy->anim]->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_nNumMatEnemy[enemy->anim]; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_pD3DTextureEnemy[enemy->anim]);

				// 描画
				g_pD3DXMeshEnemy[enemy->anim]->DrawSubset(nCntMat);

			}

			// ライトをoff
			pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		}

	}

	{// マテリアルをデフォルトに戻す
		D3DXMATERIAL mat;

		mat.MatD3D.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		mat.MatD3D.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		mat.MatD3D.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

		pDevice->SetMaterial(&mat.MatD3D);
	}

}
//=============================================================================
// エネミー取得関数
//=============================================================================
ENEMY *GetEnemy(int no)
{
	return(&enemyWk[no]);
}
//=============================================================================
// エネミーの回転行列算出関数
//=============================================================================
D3DXMATRIX* EnemyLookAtMatrix(D3DXMATRIX *pout, D3DXVECTOR3 *pEye, D3DXVECTOR3 *pAt, D3DXVECTOR3 *pUp)
{
	D3DXVECTOR3 X, Y, Z, D;
	D = *pAt - *pEye;
	D3DXVec3Normalize(&D, &D);
	D3DXVec3Cross(&X, D3DXVec3Normalize(&Y, pUp), &D);
	D3DXVec3Normalize(&X, &X);
	D3DXVec3Normalize(&Z, D3DXVec3Cross(&Z, &X, &Y));


	pout->_11 = X.x; pout->_12 = X.y; pout->_13 = X.z; pout->_14 = 0;
	pout->_21 = Y.x; pout->_22 = Y.y; pout->_23 = Y.z; pout->_24 = 0;
	pout->_31 = Z.x; pout->_32 = Z.y; pout->_33 = Z.z; pout->_34 = 0;
	pout->_41 = 0.0f; pout->_42 = 0.0f; pout->_43 = 0.0f; pout->_44 = 1.0f;

	return pout;
}
//=============================================================================
// エネミーアニメーション設定関数
// 引数：アニメーション一巡にかかる秒数）
//=============================================================================
void SetEnemyAnimation(int sec)
{
	ENEMY *enemy = &enemyWk[0];

	// アニメーションカウント
	animCnt++;

	// 秒数の絶対値を求める
	sec = abs(sec);

	// アニメーションを切り替えるフレーム数を求める
	sec = (60 * sec) / ENEMY_ANIM_MAX;

	if (animCnt % sec == 0)
	{	// アニメーションを切り替える
		enemy->anim++;
		if (enemy->anim >= ENEMY_ANIM_MAX)
		{	// 一巡したら最初に戻す
			enemy->anim = 0;
		}
	}

}
//=============================================================================
// エネミー追尾設定関数
// 引数１：追尾したいプレイヤー番号
// 引数２：移動速度の更新頻度（とりまフレーム数渡す）
// 引数３：更新一回当たりの移動速度の変化量
//=============================================================================
void SetEnemyHoming(int no, int frequency, float speedup)
{
	ENEMY *enemy = &enemyWk[0];

	// 移動速度更新頻度カウント
	sp_Update++;

	// 追尾対象にエネミーの注視点をセット
	enemy->At = GetPosPlayer(no);

	// 追尾対象への移動ベクトルを求める
	enemy->move = GetPosPlayer(no) - enemy->Eye;

	// 移動ベクトルを正規化
	D3DXVec3Normalize(&enemy->move, &enemy->move);

	// エネミーの速度調整

	// 速度を一定間隔で更新
	if (sp_Update % frequency == 0)
	{	// 速度up
		enemy->speed += speedup;
		// 移動速度更新頻度カウントをゼロに戻す
		sp_Update = 0;
	}

	// 速度設定
	enemy->move *= enemy->speed;

}