
#include "C_NonPlayableCharacter.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/C_CrowdControlComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Damage.h"
#include "ProjectC/ProjectC.h"
#include "ProjectC/AI/C_AIController.h"
#include "ProjectC/AI/Actor/C_PatrolRoute.h"
#include "ProjectC/Utils/C_GameUtil.h"

AC_NonPlayableCharacter::AC_NonPlayableCharacter()
{
	LockOnWidgetComponent = CreateDefaultSubobject<UC_WidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetupAttachment(GetMesh(), FName("Pelvis"));

	//TODO 데이터 분리 필요
	static ConstructorHelpers::FClassFinder<UUserWidget> LockOnWidgetRef(TEXT("/Game/UI/WBP_LockOn.WBP_LockOn_C"));
	if (LockOnWidgetRef.Class)
	{
		LockOnWidgetComponent->SetWidgetClass(LockOnWidgetRef.Class);
		LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		LockOnWidgetComponent->SetDrawSize(FVector2D(30.0f, 30.0f));
		LockOnWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AC_NonPlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FC_EnemyTableRow* TableRow = FC_GameUtil::GetEnemyData(CharacterDataId))
	{
		EnemyTableRow = TableRow;
	}
	check(EnemyTableRow);
	
	if (USkeletalMeshComponent* MeshComponent = GetMesh())
	{
		MeshComponent->SetSkeletalMesh(EnemyTableRow->SkeletalMesh);
		MeshComponent->SetAnimClass(EnemyTableRow->AnimInstance);
	}

	ResetState();
}

void AC_NonPlayableCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsTurning)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			// const float CurveValue = AnimInstance->GetCurveValue(TEXT("DistanceToPivot"));
			// UE_LOG(LogProjectC, Log, TEXT("%.2f"), CurveValue);
			// const float MaxCurveVal = -FMath::Abs(TurnDegree);
			//
			// // 3. 회전 진행률 (비율)
			// float TurnAlpha = 1.f - FMath::Clamp(CurveValue / MaxCurveVal, 0.f, 1.f);  // 0.0 ~ 1.0
			//
			// // 5. 회전 적용
			// float FinalYaw = TurnStartYaw + (TurnDegree * TurnAlpha);
			// FRotator NewRot = FRotator(0.f, FinalYaw, 0.f);
			// NewRot = FMath::RInterpTo(GetActorRotation(), NewRot, DeltaSeconds, 10.f);
			// SetActorRotation(NewRot);
		}
	}
}

void AC_NonPlayableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

FC_EnemyTableRow* AC_NonPlayableCharacter::GetEnemyData()
{
	if (EnemyTableRow)
		return EnemyTableRow;
	
	FC_EnemyTableRow* TableRow = FC_GameUtil::GetEnemyData(CharacterDataId);
	ensure(TableRow);
	
	return TableRow;
}

void AC_NonPlayableCharacter::ResetState()
{
	ChangeState(EC_EnemyStateType::Patrol);
}

void AC_NonPlayableCharacter::RequestChangeState(EC_EnemyStateType StateType)
{
	if (!CanChangeState(StateType))
		return;

	ChangeState(StateType);
}

bool AC_NonPlayableCharacter::CanChangeState(EC_EnemyStateType StateType)
{
	if (EnemyState == EC_EnemyStateType::Dead)
		return false;

	if (EnemyState == StateType)
		return false;
	
	if (EnemyState == EC_EnemyStateType::Battle)
	{
		if (StateType == EC_EnemyStateType::Investigating)
		{
			return false;
		}
	}

	if (EnemyState == EC_EnemyStateType::CrowdControlled)
	{
		if (StateType == EC_EnemyStateType::Battle)
			return false;

		if (StateType == EC_EnemyStateType::SkillUsing)
			return false;

		if (StateType == EC_EnemyStateType::Patrol)
			return false;

		if (StateType == EC_EnemyStateType::Investigating)
			return false;
	}

	if (EnemyState == EC_EnemyStateType::SkillUsing)
	{
		if (StateType == EC_EnemyStateType::Battle)
			return false;

		if (StateType == EC_EnemyStateType::Patrol)
			return false;

		if (StateType == EC_EnemyStateType::Investigating)
			return false;
	}
	
	return true;
}

void AC_NonPlayableCharacter::ChangeState(EC_EnemyStateType StateType)
{
	if (EnemyState == EC_EnemyStateType::Dead)
		return;
	
	EnemyState = StateType;
	
	if (EnemyState == EC_EnemyStateType::Patrol)
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyTableRow->MovementSpeed_Walk;
	}
	else if (EnemyState == EC_EnemyStateType::Battle)
	{
		GetCharacterMovement()->MaxWalkSpeed = EnemyTableRow->MovementSpeed_Run;
	}
	else if (EnemyState == EC_EnemyStateType::Investigating)
	{  
		GetCharacterMovement()->MaxWalkSpeed = EnemyTableRow->MovementSpeed_Walk;
	}
	else if (EnemyState == EC_EnemyStateType::CrowdControlled)
	{
		IsTurning = false;
	}

	if (AAIController* AIContoller = Cast<AAIController>(GetController()))
	{
		AIContoller->GetBlackboardComponent()->SetValueAsEnum(TEXT("State"), static_cast<uint8>(EnemyState));
	}
}

EC_EnemyStateType AC_NonPlayableCharacter::GetState()
{
	return EnemyState;
}

AActor* AC_NonPlayableCharacter::GetPatrolRoute()
{
	return PatrolRoute;
}

void AC_NonPlayableCharacter::IncrementPatrolIndex()
{
	if (AC_PatrolRoute* PatrolRouteActor = Cast<AC_PatrolRoute>(PatrolRoute))
	{
		PatrolRouteActor->IncrementIndex();
	}
}

void AC_NonPlayableCharacter::SetAIAttackFinishDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AC_NonPlayableCharacter::Attack()
{
	check(EnemyTableRow);
	TArray<TObjectPtr<UAnimMontage>>& AttackAnims = EnemyTableRow->AttackAnims;

	int32 ArraySize = AttackAnims.Num();
	uint32 RandomValue = FMath::RandRange(0, ArraySize - 1);

	check(AttackAnims.IsValidIndex(RandomValue));
	
	PlayAnimMontage(AttackAnims[RandomValue]);
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::OnAttackMontageEnd);
	GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate);
}

void AC_NonPlayableCharacter::OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackFinished.ExecuteIfBound();
}

void AC_NonPlayableCharacter::OnTurnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnCharacterTurnFinished.ExecuteIfBound();
	TurnAnimMontage = nullptr;
	IsTurning = false;
}

void AC_NonPlayableCharacter::SetAITurnFinishDelegate(const FAICharacterTurnFinished& InOnTurnFinished)
{
	OnCharacterTurnFinished = InOnTurnFinished;
}

void AC_NonPlayableCharacter::TurnInPlace(float TurnAnimDegree)
{
	check(EnemyTableRow);
	if (TurnAnimDegree == 90.f)
		TurnAnimMontage = EnemyTableRow->Left90TurnAnim;
	else if (TurnAnimDegree == 180.f)
		TurnAnimMontage = EnemyTableRow->Left180TurnAnim;
	else if (TurnAnimDegree == -90.f)
		TurnAnimMontage = EnemyTableRow->Right90TurnAnim;
	else if (TurnAnimDegree == -180.f)
		TurnAnimMontage = EnemyTableRow->Right180TurnAnim;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	check(AnimInstance);
	
	IsTurning = true;
	TurnStartYaw = GetActorRotation().Yaw;
	TurnDegree = TurnAnimDegree;
	
	PlayAnimMontage(TurnAnimMontage);
	
	FOnMontageEnded EndDelegate = FOnMontageEnded::CreateUObject(this, &ThisClass::OnTurnMontageEnd);
	GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate);
}

float AC_NonPlayableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UAISense_Damage::ReportDamageEvent(
		GetWorld(), 
		this,
		DamageCauser,
		DamageAmount,
		DamageCauser->GetActorLocation(),
		GetActorLocation());
	
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Damage > KINDA_SMALL_NUMBER)
	{
		if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
		{
			AnimInst->Montage_Play(EnemyTableRow->HitReact, 1.f, EMontagePlayReturnType::MontageLength, 0.f, false);
		}
	}
	
	return Damage;
}

void AC_NonPlayableCharacter::OnStartCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer)
{
	Super::OnStartCrowdControl(CrowdControlType, Causer);

	if (EnemyState != EC_EnemyStateType::SkillUsing)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		check(AnimInstance);
		
		AnimInstance->StopAllMontages(0.1f);
	}
	
	RequestChangeState(EC_EnemyStateType::CrowdControlled);
}

void AC_NonPlayableCharacter::OnEndCrowdControl(EC_CrowdControlType CrowdControlType, AActor* Causer)
{
	Super::OnEndCrowdControl(CrowdControlType, Causer);

	check(CrowdControlComponent);
	if (!CrowdControlComponent->IsCrowdControlled())
	{
		if (AAIController* AIContoller = Cast<AAIController>(GetController()))
		{
			AIContoller->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Causer);
		}
		
		ChangeState(EC_EnemyStateType::Battle);
	}
}

void AC_NonPlayableCharacter::OnDead()
{
	Super::OnDead();

	if (AAIController* AIContoller = Cast<AAIController>(GetController()))
	{
		UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(AIContoller->GetBrainComponent());
		if (BTComponent)
		{
			BTComponent->StopTree();
		}
	}

	check(CrowdControlComponent);
	CrowdControlComponent->StopCC();

	check(WidgetComponent);
	WidgetComponent->SetVisibility(false);
	
	ChangeState(EC_EnemyStateType::Dead);
}

void AC_NonPlayableCharacter::OnLand(const FHitResult& Result)
{
	Super::OnLand(Result);

	OnLandedDelegate.Broadcast();
}

void AC_NonPlayableCharacter::OnStartSkill(uint32 SkillId)
{
	Super::OnStartSkill(SkillId);

	RequestChangeState(EC_EnemyStateType::SkillUsing);

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}

	GetCharacterMovement()->DisableMovement();
}

void AC_NonPlayableCharacter::OnEndSkill(uint32 SkillId)
{
	Super::OnEndSkill(SkillId);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	check(CrowdControlComponent);
	
	if (!CrowdControlComponent->IsCrowdControlled())
		ChangeState(EC_EnemyStateType::Battle);
}
