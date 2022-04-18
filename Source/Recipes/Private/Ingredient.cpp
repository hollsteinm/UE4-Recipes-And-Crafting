// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "Ingredient.h"
#include "UnrealNetwork.h"

UIngredientInstance::UIngredientInstance() : 
	Count(0)
{
	
}

bool UIngredientInstance::IsSupportedForNetworking() const
{
	return true;
}

void UIngredientInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UIngredientInstance, Count);
	DOREPLIFETIME(UIngredientInstance, Ingredient);
}

UIngredient* UIngredientInstance::GetIngredient(bool bForceLoad) const
{
	UIngredient* Result = nullptr;
	if (Ingredient.IsValid())
	{
		Result = Cast<UIngredient>(Ingredient.ResolveObject());
		if (!Result && bForceLoad)
		{
			Result = Cast<UIngredient>(Ingredient.TryLoad());
		}
	}
	return Result;
}

UIngredientInstance* UIngredientInstance::FromIngredient(UObject* InstanceOuter, const UIngredient* IngredientAsset, int32 StartingCount, TSubclassOf<UIngredientInstance> Template)
{
	UIngredientInstance* NewInstance = NewObject<UIngredientInstance>(InstanceOuter, *Template);
	if (NewInstance)
	{
		NewInstance->Count = StartingCount;
		NewInstance->Ingredient = FStringAssetReference(IngredientAsset);
	}
	return NewInstance;
}

FText UIngredientInstance::GetNameText(bool bForceLoad) const
{
	UIngredient* Asset = GetIngredient(bForceLoad);
	if (Asset)
	{
		return Asset->Name;
	}
	else
	{
		return FText::GetEmpty();
	}
}

FText UIngredientInstance::GetDescription(bool bForceLoad) const
{
	UIngredient* Asset = GetIngredient(bForceLoad);
	if (Asset)
	{
		return Asset->Description;
	}
	else
	{
		return FText::GetEmpty();
	}
}

FText UIngredientInstance::GetCategory(bool bForceLoad) const
{
	UIngredient* Asset = GetIngredient(bForceLoad);
	if (Asset)
	{
		return Asset->Category;
	}
	else
	{
		return FText::GetEmpty();
	}
}

UTexture* UIngredientInstance::GetIcon(bool bForceLoad) const
{
	UIngredient* Asset = GetIngredient(bForceLoad);
	if (Asset)
	{
		check(Asset->Icon)
		return Asset->Icon;
	}
	else
	{
		return nullptr;
	}
}

UWorld* UIngredientInstance::GetWorld() const
{
	return GetOuter()->GetWorld();
}