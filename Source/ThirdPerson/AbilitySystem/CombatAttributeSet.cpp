// 한승쨩


#include "CombatAttributeSet.h"

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute() || Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());	
	}
}

UCombatAttributeSet::UCombatAttributeSet()
	: Health(100)
	, MaxHealth(100)
{
}