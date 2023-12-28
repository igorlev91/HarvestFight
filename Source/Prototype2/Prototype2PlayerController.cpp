// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Widget_PlayerHUD.h"

void APrototype2PlayerController::BeginPlay()
{
	Super::BeginPlay();


}

void APrototype2PlayerController::KickFromLobby()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/Maps/Level_MainMenu")), true, "kicked");
}
