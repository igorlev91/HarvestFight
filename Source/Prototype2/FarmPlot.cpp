// Fill out your copyright notice in the Description page of Project Settings.


#include "FarmPlot.h"

// Sets default values
AFarmPlot::AFarmPlot()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
}

// Called when the game starts or when spawned
void AFarmPlot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFarmPlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

