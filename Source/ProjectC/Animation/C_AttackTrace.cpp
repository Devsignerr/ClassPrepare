
#include "C_AttackTrace.h"

#include "ProjectC/Character/ClassCharacter.h"
#include "ProjectC/Character/Component/C_BattleComponent.h"


void UC_AttackTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (IC_AnimationAttackInterface* AttackPawn = Cast<IC_AnimationAttackInterface>(MeshComp->GetOwner()))
		{
			AttackPawn->AttackTrace(bStart, TraceBoneName);
		}
	}
}
