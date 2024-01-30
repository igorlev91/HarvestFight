


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
			if (Player->GetSmite() == nullptr)
			{
				Smite* smite = new Smite();
				smite->SmiteData = SmiteData;
				Player->Multi_SetSmite(smite);
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
	Super::Tick(DeltaTime);
	Timer -= DeltaTime;
	if (Timer <= 0)
	{
		SetPlayerSmites();
		Timer = TimeBetweenChecks;
	}

}

