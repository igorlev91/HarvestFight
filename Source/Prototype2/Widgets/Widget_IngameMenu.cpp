// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_IngameMenu.h"
#include "Kismet/GameplayStatics.h"

void UWidget_IngameMenu::ReturnToMenu()
{
	UGameplayStatics::OpenLevel((UObject*)GetGameInstance(), FName(TEXT("Level_MainMenu")));
}
