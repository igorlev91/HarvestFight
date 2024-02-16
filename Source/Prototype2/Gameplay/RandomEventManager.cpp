

#include "RandomEventManager.h"

#include "GrowSpot.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/DataAssets/DataAssetWorldOverride.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/DataAssets/WorldOverrideData.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/Pickups/Plant.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2/Pickups/ItemComponent.h"
#include "Prototype2/Pickups/Seed.h"
#include "Prototype2/Spawning/SeedSpawner.h"

URandomEventManager::URandomEventManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URandomEventManager::BeginPlay()
{
	Super::BeginPlay();
	if (auto Gamestate = UGameplayStatics::GetGameState(GetWorld()))
	{
		if (APrototype2Gamestate* CastedGamestate = Cast<APrototype2Gamestate>(Gamestate))
		{
			GameStateRef = CastedGamestate;
		}
	}
	
	RandomEventTimer = RandomEventInterval + (rand() % RandomEventIntervalVarience);
}

void URandomEventManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Return if game has ended OR Game has not started
	if ((GameStateRef->MatchLengthSeconds <= 0  && GameStateRef->MatchLengthMinutes <= 0)
		|| GameStateRef->CountdownLengthSeconds > 0)
		return;
	
	if (GameStateRef->MatchLengthSeconds <= 29 && GameStateRef->MatchLengthMinutes <= 0
		&& GameStateRef->MatchLengthSeconds > 0)
	{
		TriggerEvent(ERandomEvent::DOUBLE_POINTS);
	}
	
	if (RandomEventDurationTimer > 0)
	{
		RandomEventDurationTimer -= DeltaTime;
	}
	
	if (RandomEventTimer > 0)
	{
		RandomEventTimer -= DeltaTime;

		if (RandomEventTimer <= 0)
		{
			uint8 RandomNum = (rand() % ((uint8)ERandomEvent::END -1)) + 1;
			ERandomEvent RandomEvent = (ERandomEvent)RandomNum;
			TriggerEvent(RandomEvent);
		}
	}
}

void URandomEventManager::TriggerEvent(ERandomEvent _Event)
{
	if (RandomEventTimer <= 0)
	{
		RandomEventTimer = RandomEventInterval + (rand() % RandomEventIntervalVarience);
		RandomEventDurationTimer = RandomEventDuration;
	
		ActiveEvent = _Event;
	
		switch(_Event)
		{
		case ERandomEvent::DOUBLE_POINTS:
			{
				TriggerDoublePoints();
				break;
			}
		case ERandomEvent::ALL_GOLD:
			{
				TriggerAllGold();
				break;
			}
		case ERandomEvent::INSTANT_GROW:
			{
				TriggerInstantGrow();
				break;
			}
		case ERandomEvent::RAINING_SEEDS:
			{
				TriggerRainingSeeds();
				break;
			}
		//case ERandomEvent::RANDOM_GROWTH:
		//	{
		//		TriggerRandomGrowth();
		//		break;
		//	}
		//case ERandomEvent::PLANT_MUTATION:
		//	{
		//		TriggerPlantMutation();
		//		break;
		//	}
		default:
			break;
		}
	}
}

void URandomEventManager::TriggerDoublePoints()
{
	RandomEventDurationTimer = 0.0f;
	ActiveEvent = ERandomEvent::DOUBLE_POINTS;
	UE_LOG(LogTemp, Warning, TEXT("EVENT: Trigger Double Points"));
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("EVENT: Trigger Double Points"));	
}

void URandomEventManager::TriggerAllGold()
{
	ActiveEvent = ERandomEvent::ALL_GOLD;
	RandomEventDurationTimer = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("EVENT: Trigger All Gold"));
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("EVENT: Trigger All Gold"));	

	TArray<AActor*> FoundGrowSpots{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGrowSpot::StaticClass(), FoundGrowSpots);
	for(auto Growspot : FoundGrowSpots)
	{
		// Only do every third growspot
		if (rand() % 3 != 0)
			return;
		
		if (AGrowSpot* SomeGrowspot = Cast<AGrowSpot>(Growspot))
		{
			if (SomeGrowspot->GrowingItemRef)
			{
				if (SomeGrowspot->ConcretedHealth <= 0)
					SomeGrowspot->GrowingItemRef->ItemComponent->bGold = true;
			}
		}
	}
	
	Multi_MakePlantsGold();
}

void URandomEventManager::TriggerInstantGrow()
{
	ActiveEvent = ERandomEvent::INSTANT_GROW;
	RandomEventDurationTimer = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("EVENT: Trigger Instant Grow"));
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("EVENT: Trigger Instant Grow"));	

	TArray<AActor*> FoundGrowSpots{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGrowSpot::StaticClass(), FoundGrowSpots);
	for(auto Growspot : FoundGrowSpots)
	{
		if (AGrowSpot* SomeGrowspot = Cast<AGrowSpot>(Growspot))
		{
			if (SomeGrowspot->GrowingItemRef)
			{
				if (SomeGrowspot->ConcretedHealth <= 0)
					SomeGrowspot->GrowTimer = 0.1f;
			}
		}
	}
}

void URandomEventManager::TriggerRainingSeeds()
{
	ActiveEvent = ERandomEvent::RAINING_SEEDS;
	RandomEventDurationTimer = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("EVENT: Trigger Raining Seeds"));
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("EVENT: Trigger Raining Seeds"));
	
	TArray<AActor*> FoundSeedSpawners{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASeedSpawner::StaticClass(), FoundSeedSpawners);
	for(auto SeedSpawner : FoundSeedSpawners)
	{
		if (ASeedSpawner* SomeSeedSpawner = Cast<ASeedSpawner>(SeedSpawner))
		{
			for(int i = 0; i < 5; i++)
			{
				SomeSeedSpawner->SpawnSeeds_RAW();
			}
		}
	}
}

void URandomEventManager::TriggerRandomGrowth()
{
	//ActiveEvent = ERandomEvent::RANDOM_GROWTH;
	RandomEventDurationTimer = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("EVENT: Trigger Random Growth"));
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("EVENT: Trigger Random Growth"));	
	
	TArray<AActor*> FoundGrowSpots{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGrowSpot::StaticClass(), FoundGrowSpots);
	for(auto Growspot : FoundGrowSpots)
	{
		if (AGrowSpot* SomeGrowspot = Cast<AGrowSpot>(Growspot))
		{
			if (!SomeGrowspot->GrowingActor && SomeGrowspot->ConcretedHealth <= 0)
			{
				if (ADataAssetWorldOverride* DataAssetWorldOverride = GamemodeOwner->DataAssetWorldOverride)
				{
					ASeed* NewSeed = Cast<ASeed>(GetWorld()->SpawnActor(ASeed::StaticClass()));

					if (rand() % 3 == 0)
						NewSeed->SeedData = DataAssetWorldOverride->WorldOverrideData->WeaponSeeds[rand() % DataAssetWorldOverride->WorldOverrideData->WeaponSeeds.Num()];
					else
						NewSeed->SeedData = DataAssetWorldOverride->WorldOverrideData->PlantSeeds[rand() % DataAssetWorldOverride->WorldOverrideData->PlantSeeds.Num()];

					//NewSeed->Multi_SetSeedData(NewSeed->SeedData, EPickupActor::SeedActor);
					SomeGrowspot->PlantASeed(NewSeed);
				}
			}
		}
	}
}

void URandomEventManager::TriggerPlantMutation()
{
	//ActiveEvent = ERandomEvent::PLANT_MUTATION;
	RandomEventDurationTimer = 0.0f;
	UE_LOG(LogTemp, Warning, TEXT("EVENT: Trigger Plant Mutation"));

	if(GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("EVENT: Trigger Plant Mutation"));	
	
	TArray<AActor*> FoundGrowSpots{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGrowSpot::StaticClass(), FoundGrowSpots);
	for(auto Growspot : FoundGrowSpots)
	{
		if (AGrowSpot* SomeGrowspot = Cast<AGrowSpot>(Growspot))
		{
			if (SomeGrowspot->GrowingItemRef && SomeGrowspot->ConcretedHealth <= 0)
			{
				APickUpItem* SomePlant = SomeGrowspot->GrowingItemRef;
				SomeGrowspot->GrowSpotState = EGrowSpotState::Empty;
				SomeGrowspot->GrowingItemRef = nullptr;
				SomeGrowspot->GrowingActor = nullptr;
				SomeGrowspot->Multi_SetPlantReadySparkle(false);
				SomePlant->Destroy();
				
				if (ADataAssetWorldOverride* DataAssetWorldOverride = GamemodeOwner->DataAssetWorldOverride)
				{
					ASeed* NewSeed = Cast<ASeed>(GetWorld()->SpawnActor(ASeed::StaticClass()));
					
					if (rand() % 3 == 0)
						NewSeed->SeedData = DataAssetWorldOverride->WorldOverrideData->WeaponSeeds[rand() % DataAssetWorldOverride->WorldOverrideData->WeaponSeeds.Num()];
					else
						NewSeed->SeedData = DataAssetWorldOverride->WorldOverrideData->PlantSeeds[rand() % DataAssetWorldOverride->WorldOverrideData->PlantSeeds.Num()];

					//NewSeed->Multi_SetSeedData(NewSeed->SeedData, EPickupActor::SeedActor);
					SomeGrowspot->PlantASeed(NewSeed);
				}
			}
		}
	}
}

void URandomEventManager::Multi_MakePlantsGold_Implementation()
{
	TArray<AActor*> FoundPlants{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlant::StaticClass(), FoundPlants);
	
	for(auto Plant : FoundPlants)
	{
		if (APlant* SomePlant = Cast<APlant>(Plant))
		{
			for (int i = 0; i < SomePlant->ItemComponent->Mesh->GetNumMaterials(); i++)
			{
				if (SomePlant->SeedData->BabyGoldMaterials.Num() > i)
					SomePlant->ItemComponent->Mesh->SetMaterial(i, SomePlant->SeedData->BabyGoldMaterials[i]);
				else if (SomePlant->SeedData->BabyGoldMaterials.Num() > 0)
					SomePlant->ItemComponent->Mesh->SetMaterial(i, SomePlant->SeedData->BabyGoldMaterials[0]);
			}
		}
	}

}

int8 URandomEventManager::GetSellMultiplier() const
{
	switch(ActiveEvent)
	{
	case ERandomEvent::DOUBLE_POINTS:
		{
			return 2;
			break;
		}
	default:
		{
			return 1;
			break;
		}
	}
}

