#include "FarmPlot.h"

#include "GrowSpot.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Pickups/Seed.h"

AFarmPlot::AFarmPlot()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AFarmPlot::BeginPlay()
{
	Super::BeginPlay();
}

void AFarmPlot::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

}
