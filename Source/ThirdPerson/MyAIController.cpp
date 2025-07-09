
#include "MyAIController.h"

#include "ThirdPersonCharacter.h"

FGenericTeamId AMyAIController::GetGenericTeamId() const
{
	if (AThirdPersonCharacter* ThirdPersonCharacter = GetPawn<AThirdPersonCharacter>())
	{
		return ThirdPersonCharacter->GetGenericTeamId();
	}

	return FGenericTeamId(255);
}
