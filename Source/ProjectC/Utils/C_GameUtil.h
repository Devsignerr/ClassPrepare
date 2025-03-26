#pragma once

#include "CoreMinimal.h"
#include "ProjectC/enums.h"

struct FC_CharacterStatTableRow;

class PROJECTC_API FC_GameUtil
{
public:
	static FC_CharacterStatTableRow* GetCharacterStatData(EC_CharacterType CharacterType);
};
