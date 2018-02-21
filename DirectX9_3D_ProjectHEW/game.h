//=============================================================================
//
// ゲーム処理 [game.h]
// Author : GP11B243 24 人見友基
//
//=============================================================================
#ifndef _GAME_H_
#define _GAME_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

/*******************************************************************************
* 構造体定義
*******************************************************************************/

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGame(int nType);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);

#endif
