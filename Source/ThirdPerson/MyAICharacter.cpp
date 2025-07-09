
#include "MyAICharacter.h"

#include "Net/UnrealNetwork.h"

void AMyAICharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TestNum);
}

void AMyAICharacter::OnRep_TestNum()
{
	UE_LOG(LogTemp, Warning, TEXT("new test num: %d"), TestNum);
}
