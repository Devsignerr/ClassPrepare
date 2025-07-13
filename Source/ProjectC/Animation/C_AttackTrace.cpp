
#include "C_AttackTrace.h"

#include "ProjectC/Character/C_CharacterBase.h"
#include "ProjectC/Character/Component/C_BattleComponent.h"


void UC_AttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (IC_CharacterInterface* AttackPawn = Cast<IC_CharacterInterface>(MeshComp->GetOwner()))
		{
			if (AttackPawn->HasWeapon())
			{
				AttackPawn->AttackTraceWithWeapon(bStart, bRight);
			}
			else
			{
				AttackPawn->AttackTrace(bStart, TraceStartBoneName, TraceEndBoneName);
			}
		}
	}
}
