// Fill out your copyright notice in the Description page of Project Settings.


#include "SellBin.h"

// Sets default values
ASellBin::ASellBin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

}

// Called when the game starts or when spawned
void ASellBin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASellBin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

