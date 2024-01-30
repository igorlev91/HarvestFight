

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/Gameplay/Smite.h"
#include "SmiteManager.generated.h"

UCLASS()
class PROTOTYPE2_API ASmiteManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmiteManager();
	
	void SetPlayerSmites();
	UPROPERTY(EditAnywhere)
	float TimeBetweenChecks{10};

	float Timer{0};
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};