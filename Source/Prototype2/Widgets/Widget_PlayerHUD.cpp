// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_PlayerHUD.h"
#include "Widget_IngameMenu.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Prototype2GameMode.h"

void UWidget_PlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto* gameMode = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameModeRef = gameMode;
	}
}

void UWidget_PlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GameModeRef)
	{
		Minutes->SetText(FText::FromString(FString::FromInt(GameModeRef->Minutes)));
		Seconds->SetText(FText::FromString(FString::FromInt(GameModeRef->Seconds)));
	}
}

void UWidget_PlayerHUD::EnableDisableMenu()
{
	IngameMenu->EnableDisableMenu();
}
