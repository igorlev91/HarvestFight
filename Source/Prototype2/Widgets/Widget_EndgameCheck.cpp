// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_EndgameCheck.h"
#include "Kismet/GameplayStatics.h"

void UWidget_EndgameCheck::EnableDisableMenu()
{
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);

		//if (auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		//{
		//	controller->SetInputMode(FInputModeGameAndUI{});
		//	controller->bShowMouseCursor = true;
		//}
	}
	else
	{
		//if (auto* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		//{
		//	controller->bShowMouseCursor = false;
		//	controller->SetInputMode(FInputModeGameOnly{});
		//}

		SetVisibility(ESlateVisibility::Hidden);
	}
}
