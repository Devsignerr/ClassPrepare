#include "MeleeCharacter.h"

AMeleeCharacter::AMeleeCharacter()
{
	
}

void AMeleeCharacter::Attack(const FInputActionValue& Value)
{
	Super::Attack(Value);

	const bool IsPressed = Value[0] != 0.f;
	if (!IsPressed)
		return;

	if (IsAttacking)
	{
		SaveAttack = true;
	}
	else
	{
		IsAttacking = true;
		AttackCount++;
		
		if (AttackCount > AttackMontages.Num())
		{
			AttackCount = 0;
		}

		if (AttackMontages.IsValidIndex(AttackCount - 1))
			PlayAnimMontage(AttackMontages[AttackCount - 1]);

		// 애님 노티파이
		// Skeletal Mesh에 맞춰서 만든 기본 애니메이션 클립.
		// 걷기, 달리기, 점프 같은 루프 가능한 애니메이션들에 적합.
		// 주로 애님 블루프린트(Anim BP) 안에서 사용.
		// 상태 머신(State Machine)에서 전환 조건에 따라 자동 재생.
		// 🎮 사용 예시:
		// 캐릭터가 걷고 있을 때 → Walk 애님 시퀀스
		// 가만히 서 있을 때 → Idle 애님 시퀀스
		
		// Anim Montage
		// 	여러 애님 시퀀스를 한데 모아 구성 가능 (섹션, 슬롯, 브랜치 포인트 등 활용)
		// 특정 이벤트나 입력 시에 직접 실행할 수 있는 애니메이션
		// 블루프린트에서 Play Montage 노드로 직접 재생
		// 노티파이, 루트 모션, 분기점 등 고급 기능 지원
		// 🎮 사용 예시:
		// 공격 애니메이션 (콤보 시스템, 무기별 공격)
		// 캐릭터가 문을 여는 동작
		// 피니시 무브나 컷신 같은 연출
	}
}

void AMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMeleeCharacter::ComboAttackSave()
{
	if (SaveAttack)
	{
		SaveAttack = false;
		AttackCount++;
		
		if (AttackCount > AttackMontages.Num())
		{
			AttackCount = 0;
		}

		if (AttackMontages.IsValidIndex(AttackCount - 1))
			PlayAnimMontage(AttackMontages[AttackCount - 1]);
	}
}

void AMeleeCharacter::ResetCombo()
{
	AttackCount = 0;
	SaveAttack = false;
	IsAttacking = false;
}
