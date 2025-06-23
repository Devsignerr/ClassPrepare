
#include "C_NotifyFireProjectile.h"

#include "GameFramework/Character.h"
#include "ProjectC/Data/C_TableRows.h"
#include "ProjectC/SkillObject/C_SkillObject.h"
#include "ProjectC/Utils/C_GameUtil.h"

void UC_NotifyFireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner());
		if (!OwnerCharacter)
			return;

		FC_SkillObjectTableRow* SkillTableRow = FC_GameUtil::GetSkillObjectData(SKillObjectId);
		if (!SkillTableRow)
			return;
		
		const APlayerController* PlayerController = CastChecked<APlayerController>(OwnerCharacter->GetController());

		USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
		check(SkeletalMeshComponent);
	
		FVector Location = SkeletalMeshComponent->GetSocketLocation(TEXT("hand_l"));
		FRotator Rotation = PlayerController->GetControlRotation();

		FTransform Transform;
		Transform.SetLocation(Location);
		Transform.SetRotation(Rotation.Quaternion());
	
		AC_SkillObject* SkillObject = OwnerCharacter->GetWorld()->SpawnActorDeferred<AC_SkillObject>(SkillTableRow->SkillObjectActor, Transform, OwnerCharacter, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		SkillObject->OwnerCharacter = OwnerCharacter;
		SkillObject->SkillObjectId = SKillObjectId;
		SkillObject->FinishSpawning(Transform);

		FC_GameUtil::SpawnEffectAtLocation(OwnerCharacter->GetWorld(), FireFX, Location, Rotation);
	}
}
