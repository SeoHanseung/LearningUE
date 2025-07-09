// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystemInterface.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ThirdPersonCharacter.generated.h"

struct FAttributeDefaults;
class UGameplayAbility;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_STATUS_MOVEMENT_DISABLED);

UCLASS(config=Game)
class AThirdPersonCharacter : public ACharacter, public IGameplayTagAssetInterface, public IGenericTeamAgentInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	AThirdPersonCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	void MyAbilityPressed();
	void MyAbilityReleased();

	UFUNCTION(Server, Reliable)
	void Server_Test();

	UFUNCTION(Client, Reliable)
	void Client_Test();
	
	
protected:
	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> RetargetSourceMesh;
	
	UPROPERTY(EditAnywhere)
	int32 InputID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ability System")
	TSubclassOf<UGameplayAbility> MyAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGenericTeamId TeamId;
	
	UPROPERTY(EditAnywhere, Category="Ability System")
	TArray<FAttributeDefaults>	AttributeSets;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** My Ability Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MyAction;
};

