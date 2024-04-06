


#include "Prototype2/Gameplay/SmiteManager.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ASmiteManager::ASmiteManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASmiteManager::SetPlayerSmites() const
{
	TArray<AActor*> PlayerCharacters{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototype2Character::StaticClass(), PlayerCharacters);

	for (const auto PlayerCharacter : PlayerCharacters)
	{
		if (auto Player = Cast<APrototype2Character>(PlayerCharacter))
		{
			// check if smite
			if (Player->smite == nullptr)
			{
				USmite* smite = NewObject<USmite>();
				smite->SmiteData = SmiteData;
				smite->SetPlayer(Player);
				ASmiteCloud* SmiteCloudActor = GetWorld()->SpawnActor<ASmiteCloud>(SmiteCloud, FVector(0, 0, 0), {});
				SmiteCloudActor->AttachToActor(Player, FAttachmentTransformRules::KeepWorldTransform);
				SmiteCloudActor->SetActorRelativeLocation({-200, 0, 600});
				SmiteCloudActor->SetActorRelativeRotation({0, 90, 0});
				SmiteCloudActor->AssignedPlayer = Player;
				smite->SmiteCloud = SmiteCloudActor;
				Player->Multi_SetSmite(smite);
			}
		}
	}
}

void ASmiteManager::ClearPlayerSmites()
{
	bDoChecks = false;
	TArray<AActor*> PlayerCharacters{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APrototype2Character::StaticClass(), PlayerCharacters);

	for (const auto PlayerCharacter : PlayerCharacters)
	{
		if (const auto Player = Cast<APrototype2Character>(PlayerCharacter))
		{
			// check if smite
			if (Player->smite != nullptr)
			{
				Player->Multi_RemoveSmite();
			}
		}
	}
}

// Called when the game starts or when spawned
void ASmiteManager::BeginPlay()
{
	Super::BeginPlay();
	Timer = TimeBetweenChecks;
	
}

// Called every frame
void ASmiteManager::Tick(float DeltaTime)
{
	if (!bDoChecks)
		return;
	
	Super::Tick(DeltaTime);
	Timer -= DeltaTime;
	if (Timer <= 0)
	{
		SetPlayerSmites();
		Timer = TimeBetweenChecks;
	}

}

