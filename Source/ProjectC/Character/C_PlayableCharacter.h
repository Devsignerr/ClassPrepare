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
#include "C_CharacterBase.h"
#include "InputActionValue.h"
#include "ProjectC/Interface/C_CharacterHUDInterface.h"
#include "C_PlayableCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;

UCLASS(config=Game)
class AC_PlayableCharacter : public AC_CharacterBase , public IC_CharacterHUDInterface
{
	GENERATED_BODY()

public:
	AC_PlayableCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void SetupHUDWidget(class UC_HUDWidget* InHUDWidget) override;
	
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

public:
	virtual void Attack(const FInputActionValue& Value);
	
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
	
	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;
};

