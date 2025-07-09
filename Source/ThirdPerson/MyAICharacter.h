// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdPersonCharacter.h"
#include "MyAICharacter.generated.h"

UCLASS()
class THIRDPERSON_API AMyAICharacter : public AThirdPersonCharacter
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(ReplicatedUsing=OnRep_TestNum, EditAnywhere, BlueprintReadWrite)
	int32 TestNum;

	UFUNCTION()
	void OnRep_TestNum();
};
