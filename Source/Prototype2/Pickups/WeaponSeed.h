

#pragma once

#include "CoreMinimal.h"
#include "Seed.h"
#include "WeaponSeed.generated.h"

UCLASS()
class PROTOTYPE2_API AWeaponSeed : public ASeed
{
	GENERATED_BODY()

	AWeaponSeed();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
