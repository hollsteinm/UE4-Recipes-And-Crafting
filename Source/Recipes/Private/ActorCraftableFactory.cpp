// © 2016 Martin Hollstein, All Rights Reserved.

#include "RecipesPrivatePCH.h"
#include "ActorCraftableFactory.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"

UActorCraftableFactory::UActorCraftableFactory() :
	SpawnDistance(300.0f),
	SpawnHeight(50.0f),
	SpawnCollisionHandlingOverride(ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
{

}

UObject* UActorCraftableFactory::CreateInternal_Implementation(AController* Instigator, AActor* Owner) const
{
	if (Owner)
	{
		UWorld* World = Owner->GetWorld();
		if (World)
		{
			AActor* Spawned = World->SpawnActor<AActor>(*ActorClassToSpawn, GetSpawnTransform(Instigator, Owner), GetSpawnParameters(Instigator, Owner));
			return Spawned;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

FTransform UActorCraftableFactory::GetSpawnTransform(AController* Instigator, AActor* Owner) const
{
	return GetSpawnTransformInternal(Instigator, Owner);
}

FTransform UActorCraftableFactory::GetSpawnTransformInternal_Implementation(AController* Instigator, AActor* Owner) const
{
	FVector Forward = FVector::ForwardVector;
	FVector Location = FVector::ZeroVector;

	if (Owner)
	{
		Forward = Owner->GetActorForwardVector();
		Location = Owner->GetActorLocation();
	}
	
	FVector SpawnLocation = Location + Forward * SpawnDistance;
	SpawnLocation.Z += SpawnHeight;
	return FTransform(SpawnLocation);
}

FActorSpawnParameters UActorCraftableFactory::GetSpawnParameters(AController* Instigator, AActor* Owner) const
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = SpawnCollisionHandlingOverride;
	return Params;
}