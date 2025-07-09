// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/MyAbility.h"
#include "GameplayAbilities/Public/AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/AIModule/Classes/AIController.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_STATUS_MOVEMENT_DISABLED, "Status.Movement.Disabled", "Tag for blocking movement input");

//////////////////////////////////////////////////////////////////////////
// AThirdPersonCharacter

void AThirdPersonCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

AThirdPersonCharacter::AThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	RetargetSourceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RetargetSourceMesh"));
	RetargetSourceMesh->SetupAttachment(GetCapsuleComponent());
	RetargetSourceMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	TeamId = FGenericTeamId(0);
}

void AThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// AbilitySystemComponent->InitAbilityActorInfo(this, this);
		//
		// // CDO (Class Default Object)
		//
		// if (MyAbility)
		// {
		// 	UGameplayAbility* AbilityCDO = MyAbility->GetDefaultObject<UMyAbility>();
		// 	
		// 		FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1, InputID);
		// 		AbilitySystemComponent->GiveAbility(AbilitySpec);
		//
		// 	for (int32 i=0; i < AttributeSets.Num(); ++i)
		// 	{
		// 		if (AttributeSets[i].Attributes && AttributeSets[i].DefaultStartingTable)
		// 		{
		// 			UAttributeSet* Attributes = NewObject<UAttributeSet>(this, AttributeSets[i].Attributes);
		// 			AbilitySystemComponent->AddAttributeSetSubobject(Attributes);
		// 			Attributes->InitFromMetaDataTable(AttributeSets[i].DefaultStartingTable);
		// 		}
		// 	}
		// }
	}
}

void AThirdPersonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AThirdPersonCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
	}
}

bool AThirdPersonCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AThirdPersonCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AThirdPersonCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

UAbilitySystemComponent* AThirdPersonCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FGenericTeamId AThirdPersonCharacter::GetGenericTeamId() const
{
	return TeamId;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonCharacter::Client_Test_Implementation()
{
}

void AThirdPersonCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);

		// Activating My Ability
		EnhancedInputComponent->BindAction(MyAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::MyAbilityPressed);
		EnhancedInputComponent->BindAction(MyAction, ETriggerEvent::Completed, this, &AThirdPersonCharacter::MyAbilityReleased);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AThirdPersonCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		if (!HasMatchingGameplayTag(TAG_STATUS_MOVEMENT_DISABLED))
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void AThirdPersonCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AThirdPersonCharacter::MyAbilityPressed()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->PressInputID(InputID);
	}
}

void AThirdPersonCharacter::MyAbilityReleased()
{
}

void AThirdPersonCharacter::Server_Test_Implementation()
{
}
