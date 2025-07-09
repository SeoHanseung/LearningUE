// 한승쨩

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CombatAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) \
	
	

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth)

	// GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UCombatAttributeSet, Health)
	// GAMEPLAYATTRIBUTE_VALUE_GETTER(Health);
	// GAMEPLAYATTRIBUTE_VALUE_SETTER(Health)
	// GAMEPLAYATTRIBUTE_VALUE_INITTER(Health)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	UCombatAttributeSet();

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
};
