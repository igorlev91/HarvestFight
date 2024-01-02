// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_IngameMenu.h"
#include "Kismet/GameplayStatics.h"

void UWidget_IngameMenu::ReturnToMenu()
{
	//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("/Game/Maps/Level_MainMen")));
}

void UWidget_IngameMenu::EnableDisableMenu()
{
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);

		if (auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			controller->SetInputMode(FInputModeGameAndUI{});
			controller->bShowMouseCursor = true;
		}
	}
	else
	{
		if (auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			controller->bShowMouseCursor = false;
			controller->SetInputMode(FInputModeGameOnly{});
		}

		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UWidget_IngameMenu::DisableMenu()
{
	if (GetVisibility() == ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}
