// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "CraftableFactory.h"

UObject* UCraftableFactory::CreateInternal_Implementation(class AController* Instigator, class AActor* Owner) const PURE_VIRTUAL(UCraftableFactory::CreateInternal_Implementation, return nullptr;)

UObject* UCraftableFactory::Create(class AController* Instigator, class AActor* Owner) const
{
	return CreateInternal(Instigator, Owner);
}
