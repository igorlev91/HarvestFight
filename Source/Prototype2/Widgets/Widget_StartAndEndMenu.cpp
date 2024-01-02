// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2/Widgets/Widget_StartAndEndMenu.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

void UWidget_StartAndEndMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateTimerText();
}

void UWidget_StartAndEndMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (auto* gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateRef = gameState;
	}
}

void UWidget_StartAndEndMenu::UpdateTimerText()
{
	if (TimerText && GameStateRef->Server_Players.Num() >=  GameStateRef->FinalConnectionCount && ! GameStateRef->GameHasStarted)
	{
		TimerText->SetText(FText::FromString(FString::FromInt((int)GameStateRef->CountdownLengthSeconds)));
		TimerText->SetVisibility(ESlateVisibility::HitTestInvisible);
		BackgroundBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else if (TimerText)
	{
		TimerText->SetVisibility(ESlateVisibility::Hidden);
		BackgroundBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}
