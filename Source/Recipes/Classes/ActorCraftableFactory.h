// © 2016 Martin Hollstein, All Rights Reserved.

#pragma once

#include "CraftableFactory.h"
#include "ActorCraftableFactory.generated.h"

/**
 * Factory class that will do basic Actor spawning with some custom parameters. Can be reimplemented in derived classes.
 */
UCLASS()
class RECIPES_API UActorCraftableFactory : public UCraftableFactory
{
	GENERATED_BODY()
public:
	UActorCraftableFactory();

protected:
	/**UCraftableFactory Interface**/
	virtual UObject* CreateInternal_Implementation(class AController* Instigator, class AActor* Owner) const override;
	/**UCraftableFactory End**/

	/**
	* Return transform of the newly spawned actor.
	**/
	UFUNCTION(BlueprintCallable, Category="CraftableFactory")
	FTransform GetSpawnTransform(class AController* Instigator, class AActor* Owner) const;

	/**
	* Overridable return transform of the newly spawned actor.
	**/
	UFUNCTION(BlueprintNativeEvent, Category = "CraftableFactory")
	FTransform GetSpawnTransformInternal(class AController* Instigator, class AActor* Owner) const;

	virtual struct FActorSpawnParameters GetSpawnParameters(class AController* Instigator, class AActor* Owner) const;

	/**
	* The type of actor this factory spawns.
	**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnParameters")
	TSubclassOf<AActor> ActorClassToSpawn;

	/**
	* How far forward from the Owner it should be spawned (implemented in base class GetSpawnTransform)
	**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnParameters")
	float SpawnDistance;

	/**
	* How far up from the Owner it should be spawned (implemented in base class GetSpawnTransform)
	**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnParameters")
	float SpawnHeight;	

	/**
	* SpawnActor function collision spawning override
	**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SpawnParameters")
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;
};