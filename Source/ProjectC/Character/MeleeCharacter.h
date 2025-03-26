// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


//Unreal Engine에서 자주 쓰이는 핵심 타입, 매크로, 기능들만 최소한으로 모아놓은 헤더파일이에요.
//ObjectMacros.h	UCLASS, UPROPERTY, UFUNCTION 같은 매크로
//EngineMinimal.h	게임 엔진 관련 최소 헤더
//Math 관련 클래스	FVector, FRotator, FTransform, FQuat 등 수학 클래스
//기본 컨테이너	TArray, TMap, TSet, FString, FName 등
//스마트 포인터	TSharedPtr, TWeakObjectPtr, TUniquePtr 등
//로깅 시스템	UE_LOG() 사용 가능
//assert 및 디버깅	check(), ensure(), UE_DEBUG_BREAK() 등

#include "CoreMinimal.h"
#include "ClassCharacter.h"
#include "InputActionValue.h"
#include "MeleeCharacter.generated.h"


UCLASS(config=Game)
class AMeleeCharacter : public AClassCharacter
{
	GENERATED_BODY()

public:
	AMeleeCharacter();
	

protected:
	virtual void Attack(const FInputActionValue& Value) override;

protected:
	virtual void BeginPlay();

public:
	UFUNCTION(BlueprintCallable)
	void ComboAttackSave();

	UFUNCTION(BlueprintCallable)
	void ResetCombo();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool SaveAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int AttackCount = 0;

	//언리얼 엔진이 내부적으로 그 변수(멤버)를 인식하고 관리할 수 있도록 해줌
	//. Garbage Collection (GC, 가비지 컬렉션)을 위해
	//언리얼은 UObject 기반 클래스들을 자동으로 메모리 관리해.
	//그런데 이걸 가능하게 하려면, 이 객체가 어디에서 참조되고 있는지 알아야 해.
	//UPROPERTY()를 붙이지 않으면, 언리얼은 이 멤버 변수를 GC에서 추적하지 않음.
	//⇒ 메모리 누수 or 크래시(특히 삭제된 오브젝트를 참조할 때)가 발생할 수 있음.

	//리플렉션이란?
	//코드가 자기 자신을 런타임에 인식하고 다룰 수 있는 시스템"**이에요.
	//즉, 언리얼 엔진에서 어떤 클래스, 변수, 함수 등이 있는지, 그리고 그 이름, 타입, 값 등을 런타임이나 툴(에디터, 블루프린트 등) 에서 동적으로 알아내고 제어할 수 있는 기능이에요.
	//언리얼은 단순히 코드를 실행하는 엔진이 아니고,
	//에디터, 블루프린트, 세이브 시스템, 리플리케이션(네트워크), 가비지 컬렉션 등 다양한 시스템과 연동되기 때문에…
	//UnrealHeaderTool이라는 툴을 이용해, 그 정보를 메타데이터로 추출해서 저장해.
	//이 정보는 .generated.h 파일 등에 포함되고, 런타임/에디터/블루프린트 등에서 사용돼.

	// generated.h 란?
	//*.generated.h 파일은 언리얼의 리플렉션 시스템과 자동 코드 생성을 가능하게 해주는 핵심 파일
	//이 파일 누가 만들어요?	UnrealHeaderTool이 빌드 과정에서 자동 생성
	//왜 필요한가요?	언리얼이 C++ 코드에서 리플렉션 정보를 추출해 쓸 수 있게 하기 위해
	//무슨 정보 들어있나요?	리플렉션 메타데이터, 함수 등록, GC 처리, StaticClass 생성 등
	
	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages; 
};

