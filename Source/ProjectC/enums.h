#pragma once

UENUM()
enum class EEffectType : int32
{
	None,
	Hit,
	Attack,
	Num
};

UENUM()
enum class EC_DataTableType : int32
{
	None,
	CharacterStat,
	Num
};

UENUM()
enum class EC_CharacterType : int32
{
	None,
	Player,
	Enemy,
	Num
};