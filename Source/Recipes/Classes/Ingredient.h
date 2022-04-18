// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "StringAssetReference.h"
#include "Ingredient.generated.h"

/**
 * Data asset that describes a recipe and includes some basic metadata.
 */
UCLASS(BlueprintType, Blueprintable)
class RECIPES_API UIngredient : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/**
	* User facing Name of the Ingredient
	**/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Name;

	/**
	* User facing Description of the Ingredient
	**/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Description;

	/**
	* User facing Category of the Ingredient
	**/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText Category;

	/**
	* User facing Icon of the Ingredient
	**/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	class UTexture* Icon;
};

/**
* Instanced object that wraps the UIngredient and UIngredient derived data assets for ownership.
* This object supports replication if the direct Outer is an Actor or Actor derived class. For any
* UObject derived classes, you must replicate this via the ReplicateSubobjects interface.
**/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class RECIPES_API UIngredientInstance : public UObject
{
	GENERATED_BODY()
public:
	UIngredientInstance();

	/*
	* The number of ingredients this instance represents.
	*/
	UPROPERTY(Replicated, SaveGame, BlueprintReadWrite, EditAnywhere)
	int32 Count;

	/**UObject Interface**/
	virtual bool IsSupportedForNetworking() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UWorld* GetWorld() const override;
	/**End**/

	static UIngredientInstance* FromIngredient(UObject* InstanceOuter, const UIngredient* IngredientAsset, int32 StartingCount, TSubclassOf<UIngredientInstance> Template = UIngredientInstance::StaticClass());

	/*
	* Get the asset references Ingredient.Name
	*/
	UFUNCTION(BlueprintCallable, Category = "Ingredient")
	FText GetNameText(bool bForceLoad = true) const;

	/*
	* Get the asset references Ingredient.Description
	*/
	UFUNCTION(BlueprintCallable, Category = "Ingredient")
	FText GetDescription(bool bForceLoad = true) const;

	/*
	* Get the asset references Ingredient.Category
	*/
	UFUNCTION(BlueprintCallable, Category = "Ingredient")
	FText GetCategory(bool bForceLoad = true) const;

	/*
	* Get the asset references Ingredient.Icon
	*/
	UFUNCTION(BlueprintCallable, Category = "Ingredient")
	class UTexture* GetIcon(bool bForceLoad = true) const;

protected:
	/*
	* The Ingredient Asset that this Ingredient instance represents.
	*/
	UPROPERTY(Replicated, SaveGame, BlueprintReadOnly, EditAnywhere, Meta=(AllowedClasses="Ingredient", ExactClass="false"))
	FStringAssetReference Ingredient;

	/*
	* Gets a pointer to the Ingredient object that the asset reference is pointing to.
	*/
	UFUNCTION(BlueprintCallable, Category = "Ingredient")
	UIngredient* GetIngredient(bool bForceLoad = true) const;
};