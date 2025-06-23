#pragma once

UENUM()
enum class EC_DataTableType : int32
{
	None,
	CharacterStat,
	Enemy,
	Weapon,
	Skill,
	Exec,
	SkillObject,
	CrowdControl,
	Num
};

UENUM()
enum class EC_CharacterType : int32
{
	None,
	Player,
	Kallari,
	Num
};

UENUM(BlueprintType)
enum class EC_EnemyStateType : uint8
{
	None = 0,
	Patrol = 1,
	Battle = 2,
	Investigating = 3,
	CrowdControlled = 4,
	Num = 3
};

UENUM(BlueprintType)
enum class EC_AISenseType : uint8
{
	None,
	Sight,
	Hearing,
	Damage,
	Num
};

UENUM(BlueprintType)
enum class EC_LockCauseType : uint8
{
	None,
	Move,
	Jump,
	Attack,
	Run,
	Roll,
	SpecialAction,
	Num,
};

UENUM(BlueprintType)
enum class EC_ActionType : uint8
{
	None,
	Move,
	Jump,
	Attack,
	Run,
	Roll,
	SpecialAction,
	Num,
};

UENUM(BlueprintType)
enum class EC_SkillObjectType : uint8
{
	None,
	Projectile_Normal,
	Projectile_Homing,
	Projectile_Bounce,
	Num,
};

UENUM(BlueprintType)
enum class EC_CharacterStanceType : uint8
{
	Sword = 0,
	Staff = 1,
};

UENUM(BlueprintType)
enum class EC_CameraType : uint8
{
	None,
	Normal,
	Aim,
	Num
};

UENUM(BlueprintType)
enum class EC_SkillTargetingType : uint8
{
	None,
	NoneTarget,
	ChainAttack,
	Multiple,
	Num
};

UENUM(BlueprintType)
enum class EC_SkillSlotType : uint8
{
	None,
	Num_1,
	Num_2,
	Num_3,
	Num_4,
	Num
};

UENUM(BlueprintType)
enum class EC_ExecType : uint8
{
	None,
	Dash,
	DashToTarget,
	Projectile,
	Pushback,
	Num
};

UENUM(BlueprintType)
enum class EC_ExecCollisionType : uint8
{
	None,
	Box,
	Sphere,
	Capsule,
	Num
};

UENUM(BlueprintType)
enum class EC_CrowdControlType : uint8
{
	None,
	Pushback,
	Stun,
	Ragdoll,
	Num
};