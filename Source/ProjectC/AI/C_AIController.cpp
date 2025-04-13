// Fill out your copyright notice in the Description page of Project Settings.


#include "C_AIController.h"
#include "Actor/C_PatrolRoute.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ProjectC/Character/C_NonPlayableCharacter.h"
#include "ProjectC/Utils/C_GameUtil.h"


AC_AIController::AC_AIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	UAIPerceptionComponent* AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	SightSense = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingSense = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	DamageSense = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	PerceptionComponent->ConfigureSense(*SightSense);
	PerceptionComponent->ConfigureSense(*HearingSense);
	PerceptionComponent->ConfigureSense(*DamageSense);
	
	PerceptionComponent->SetDominantSense(SightSense->GetSenseImplementation());

	//감지되거나 감지되지 않은 즉시(expire와 상관 x)
	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ThisClass::OnPerceptionUpdate);

	//단일 감각에 대한 델리게이트
	//PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdate);
}

void AC_AIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		Blackboard->SetValueAsVector(TEXT("HomePose"), GetPawn()->GetActorLocation());

		bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

void AC_AIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AC_AIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.Code == EPathFollowingResult::Success)
	{
		IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(GetPawn());
		check(AIPawn);
		
		if (AC_PatrolRoute* PatrolRoute = Cast<AC_PatrolRoute>(AIPawn->GetPatrolRoute()))
		{
			PatrolRoute->IncrementIndex();
		}
	}
}

FC_EnemyTableRow* AC_AIController::GetEnemyData()
{
	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(GetPawn());
	check(AIPawn);
	
	FC_EnemyTableRow* EnemyTableRow = AIPawn->GetEnemyData();
	ensure(EnemyTableRow);

	return EnemyTableRow;
}

void AC_AIController::OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* UpdatedActor : UpdatedActors)
	{
		if (GetAIStimulus(UpdatedActor, EC_AISenseType::Sight).WasSuccessfullySensed())
		{
			HandleSensedSight(UpdatedActor);
			break;
		}

		FAIStimulus Stimulus = GetAIStimulus(UpdatedActor, EC_AISenseType::Hearing);
		if (Stimulus.WasSuccessfullySensed())
		{
			//자극이 발생한 위치
			HandleSensedHearing(UpdatedActor, Stimulus.StimulusLocation);
			break;
		}
		
		if (GetAIStimulus(UpdatedActor, EC_AISenseType::Damage).WasSuccessfullySensed())
		{
			HandleSensedDamage(UpdatedActor);
			break;
		}
	}
}

void AC_AIController::SetupSenseConfig()
{
	const FC_EnemyTableRow* EnemyTableRow = GetEnemyData();
	ensure(EnemyTableRow);
	
	SightSense->SightRadius = EnemyTableRow->SightRadius;
	SightSense->LoseSightRadius = EnemyTableRow->LoseSightRadius;
	SightSense->PeripheralVisionAngleDegrees = EnemyTableRow->PeripheralVisionAngleDegrees;
	SightSense->SetMaxAge(5.f);
	SightSense->DetectionByAffiliation.bDetectEnemies = true;
	
	HearingSense->HearingRange = 1500.f;
	HearingSense->DetectionByAffiliation.bDetectEnemies = true;
	HearingSense->SetMaxAge(5.f);

	DamageSense->SetMaxAge(2.f);

	//생성자에서 ConfigureSense 해주지 않으면 문제 발생해서 굳이 2번 처리 해주는것
	PerceptionComponent->ConfigureSense(*SightSense);
	PerceptionComponent->ConfigureSense(*HearingSense);
	PerceptionComponent->ConfigureSense(*DamageSense);
}

FAIStimulus AC_AIController::GetAIStimulus(AActor* Actor, EC_AISenseType AIPerceptionSense)
{
	FActorPerceptionBlueprintInfo ActorPerceptionBlueprintInfo;
	FAIStimulus ResultStimulus;

	GetPerceptionComponent()->GetActorsPerception(Actor, ActorPerceptionBlueprintInfo);

	TSubclassOf<UAISense> QuerySenseClass;
	switch (AIPerceptionSense)
	{
	case EC_AISenseType::Sight:
		QuerySenseClass = UAISense_Sight::StaticClass();
		break;
	case EC_AISenseType::Hearing:
		QuerySenseClass = UAISense_Hearing::StaticClass();
		break;
	case EC_AISenseType::Damage:
		QuerySenseClass = UAISense_Damage::StaticClass();
		break;
	default:
		break;
	}

	TSubclassOf<UAISense> LastSensedStimulusClass;

	for (const FAIStimulus& AIStimulus : ActorPerceptionBlueprintInfo.LastSensedStimuli)
	{
		LastSensedStimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, AIStimulus);
		
		if (QuerySenseClass == LastSensedStimulusClass)
		{
			ResultStimulus = AIStimulus;
			return ResultStimulus;
		}
	}
	
	return ResultStimulus;
}

void AC_AIController::HandleSensedSight(AActor* InActor)
{
	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(GetPawn());
	ensure(AIPawn);
	
	AIPawn->ChangeState(EC_EnemyStateType::Battle);
	GetBlackboardComponent()->SetValueAsEnum(TEXT("State"), static_cast<uint8>(EC_EnemyStateType::Battle));
	GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), InActor);
}

void AC_AIController::HandleSensedHearing(AActor* InActor, FVector InLocation)
{
	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(GetPawn());
	ensure(AIPawn);

	if (AIPawn->GetState() == EC_EnemyStateType::Battle)
		return;
	
	AIPawn->ChangeState(EC_EnemyStateType::Investigating);
	GetBlackboardComponent()->SetValueAsVector(TEXT("InvestigatingPos"), InLocation);
}

void AC_AIController::HandleSensedDamage(AActor* InActor)
{
	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(GetPawn());
	ensure(AIPawn);

	AIPawn->ChangeState(EC_EnemyStateType::Battle);
	GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), InActor);
}

void AC_AIController::HandleLoseTarget(AActor* InActor)
{
	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(GetPawn());
	ensure(AIPawn);

	AIPawn->ChangeState(EC_EnemyStateType::Patrol);
	GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), nullptr);
}

void AC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	const FC_EnemyTableRow* EnemyTableRow = GetEnemyData();
	ensure(EnemyTableRow);
	
	BBAsset = EnemyTableRow->BlackBoard;
	BTAsset = EnemyTableRow->BehaviorTree;

	SetupSenseConfig();
	RunAI();
}

void AC_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AActor* CurrentActor = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TEXT("Target")));
	if (!CurrentActor)
		return;

	IC_CharacterAIInterface* AIPawn = Cast<IC_CharacterAIInterface>(GetPawn());
	ensure(AIPawn);

	FAIStimulus SightStimulus = GetAIStimulus(CurrentActor, EC_AISenseType::Sight);
	FAIStimulus DamageStimulus = GetAIStimulus(CurrentActor, EC_AISenseType::Damage);

	const bool bLostSight = !SightStimulus.WasSuccessfullySensed() && SightStimulus.IsExpired();
	const bool bDamageExpired = DamageStimulus.IsExpired();

	if (bLostSight && bDamageExpired)
	{
		HandleLoseTarget(CurrentActor);
	}
}
