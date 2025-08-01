// Copyright Epic Games, Inc. All Rights Reserved.

#include "C_CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Component/C_BattleComponent.h"
#include "Component\C_CrowdControlComponent.h"
#include "Component/C_StatComponent.h"
#include "Component/C_WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectC/UI/C_HpBarWidget.h"
#include "ProjectC/UI/C_LockOnWidget.h"
#include "ProjectC/Utils/C_GameUtil.h"

AC_CharacterBase::AC_CharacterBase()
{
	// 캐릭터의 Capsule Component 사이즈
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// 컨트롤러 인풋이 카메라에만 영향을 끼치도록 함 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//  true 로 하면 입력에 따라 캐릭터가 회전함 . 
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f; // 아날로그 스틱 입력값

	// Braking Deceleration Walking	멈출 때 얼마나 빨리 감속할지 결정	기본값: 2048 → 줄이면 미끄러짐
	// Ground Friction	지면 마찰력. 작을수록 더 오래 미끄러짐	기본값: 8 → 줄이면 미끄러짐 강화
	// Braking Friction Factor	브레이킹 중 마찰력 계수. 0이면 브레이크 안 걸리는 느낌	기본값: 1 → 0~0.3 추천
	//GetCharacterMovement()->BrakingDecelerationWalking = 500.f;
	//GetCharacterMovement()->GroundFriction = 2.f;
	//GetCharacterMovement()->BrakingFrictionFactor = 0.2f;
	
	BattleComponent = CreateDefaultSubobject<UC_BattleComponent>(TEXT("BattleComponent"));
	CrowdControlComponent = CreateDefaultSubobject<UC_CrowdControlComponent>(TEXT("CrowdControlComponent"));
	StatComponent = CreateDefaultSubobject<UC_StatComponent>(TEXT("StatComponent"));
	SkillComponent = CreateDefaultSubobject<UC_SkillComponent>(TEXT("SkillComponent"));
	
	Weapon_L_StaticComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon_L_StaticComponent"));
	Weapon_L_StaticComponent->SetupAttachment(GetMesh());
	Weapon_R_StaticComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon_R_StaticComponent"));
	Weapon_R_StaticComponent->SetupAttachment(GetMesh());
	
	WidgetComponent = CreateDefaultSubobject<UC_WidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(GetMesh());
	WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));

	//TODO 데이터 분리 필요
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/UI/WBP_HPBar.WBP_HPBar_C"));
	if (HpBarWidgetRef.Class)
	{
		WidgetComponent->SetWidgetClass(HpBarWidgetRef.Class);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetDrawSize(FVector2D(150.0f, 15.0f));
		WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AC_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	check(CrowdControlComponent);

	if (!CrowdControlComponent->OnStartCCDelegate.IsAlreadyBound(this, &ThisClass::OnStartCrowdControl))
		CrowdControlComponent->OnStartCCDelegate.AddDynamic(this, &ThisClass::OnStartCrowdControl);

	if (!CrowdControlComponent->OnEndCCDelegate.IsAlreadyBound(this, &ThisClass::OnEndCrowdControl))
		CrowdControlComponent->OnEndCCDelegate.AddDynamic(this, &ThisClass::OnEndCrowdControl);
	
	if (!LandedDelegate.IsAlreadyBound(this, &ThisClass::OnLand))
		LandedDelegate.AddDynamic(this, &ThisClass::OnLand);
}

void AC_CharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(StatComponent);
	check(SkillComponent);
	
	if (!StatComponent->OnDead.IsBoundToObject(this))
		StatComponent->OnDead.AddUObject(this, &ThisClass::OnDead);

	if (!StatComponent->OnStatChanged.IsBoundToObject(this))
		StatComponent->OnStatChanged.AddUObject(this, &ThisClass::ApplyStat);
	
	if (!SkillComponent->OnStartSkillDelegate.IsAlreadyBound(this, &ThisClass::OnStartSkill))
		SkillComponent->OnStartSkillDelegate.AddDynamic(this, &ThisClass::OnStartSkill);

	if (!SkillComponent->OnEndSkillDelegate.IsAlreadyBound(this, &ThisClass::OnEndSkill))
		SkillComponent->OnEndSkillDelegate.AddDynamic(this, &ThisClass::OnEndSkill);
}

void AC_CharacterBase::ApplyStat(const FC_CharacterStatTableRow& BaseStat, const FC_CharacterStatTableRow& ModifierStat)
{
	
}

void AC_CharacterBase::AttackTrace(bool bStart, FName TraceStartBoneName, FName TraceEndBoneName)
{
	check(BattleComponent);
	
	if (bStart)
		BattleComponent->StartTrace(TraceStartBoneName, TraceEndBoneName);
	else
		BattleComponent->EndTrace();
}

void AC_CharacterBase::AttackTraceWithWeapon(bool bStart, bool bRight)
{
	if (bStart)
		BattleComponent->StartTraceWithWeapon(bRight);
	else
		BattleComponent->EndTrace();
}

void AC_CharacterBase::SetupCharacterWidget(UC_UserWidget* InUserWidget)
{
	if (UC_HpBarWidget* HpBarWidget = Cast<UC_HpBarWidget>(InUserWidget))
	{
		HpBarWidget->UpdateHpBar(StatComponent->GetCurrentHp(), StatComponent->GetMaxHp());
		StatComponent->OnHpChanged.AddUObject(HpBarWidget, &UC_HpBarWidget::UpdateHpBar);
	}
}

void AC_CharacterBase::SetupLockOnWidget(UC_UserWidget* InUserWidget)
{
	if (UC_LockOnWidget* LockOnWidget = Cast<UC_LockOnWidget>(InUserWidget))
	{
		OnCharacterLocked.AddDynamic(LockOnWidget, &UC_LockOnWidget::ToggleActivation);
	}
}

void AC_CharacterBase::OnLocked(bool bLocked)
{
	OnCharacterLocked.Broadcast(bLocked);
}

bool AC_CharacterBase::HasWeapon()
{
	check(BattleComponent);
	return BattleComponent->HasWeapon();
}

void AC_CharacterBase::OnDead()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	check(AnimInstance);

	AnimInstance->StopAllMontages(0.f);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool AC_CharacterBase::IsDead()
{
	check(StatComponent);
	return StatComponent->CurrentHp < KINDA_SMALL_NUMBER;
}

TPair<FName, FName> AC_CharacterBase::GetWeaponTraceNames(bool bRight)
{
	return {BattleComponent->TraceStartBoneName, BattleComponent->TraceEndBoneName};
}

void AC_CharacterBase::OnStartCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer)
{
}

void AC_CharacterBase::OnEndCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer)
{
}

FC_OnStartSkillDelegate& AC_CharacterBase::GetOnStartSkillDelegate()
{
	check(SkillComponent);
	return SkillComponent->OnStartSkillDelegate;
}

FC_OnEndSkillDelegate& AC_CharacterBase::GetOnEndSkillDelegate()
{
	check(SkillComponent);
	return SkillComponent->OnEndSkillDelegate;
}

void AC_CharacterBase::OnStartSkill(uint32 SkillId)
{
}

void AC_CharacterBase::OnEndSkill(uint32 SkillId)
{
}

float AC_CharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,AActor* DamageCauser)
{
	const float Damage = StatComponent->ApplyDamage(DamageAmount, DamageCauser);
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AC_CharacterBase::OnLand(const FHitResult& Result)
{
	OnLandedDelegate.Broadcast();
}
