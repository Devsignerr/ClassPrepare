// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ClassGameMode.generated.h"

// 언리얼 게임의 "룰북 + 관리자" 역할
// 플레이어 스폰	어떤 Pawn 또는 Character로 플레이어가 등장할지 설정
// HUD 지정	어떤 HUD 클래스 쓸지 결정
// PlayerController 설정	입력 처리할 컨트롤러 지정
// GameState 생성	전체 게임 상태 공유용 클래스 지정
// 시작 조건, 승리 조건, 규칙 등	게임의 룰을 정의함 (예: 라운드 시스템, 점수 체크 등)

//항목			GameMode				GameState
//실행 위치		서버 전용				클라이언트 & 서버 둘 다 존재
//용도			게임 규칙 & 흐름 제어		현재 게임 상태 공유 (점수, 시간 등)
//수정가능 위치	서버에서만 수정			서버에서 수정 후 클라이언트로 replicate
//예시			어떤 캐릭터 쓸까,			언제 시작할까	남은 시간, 각 팀 점수 등

UCLASS(minimalapi)
class AClassGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AClassGameMode();
};



