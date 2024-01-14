// Fill out your copyright notice in the Description page of Project Settings.


#include "Prototype2/Widgets/Widget_StartAndEndMenu.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"

void UWidget_StartAndEndMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (auto* GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	{
		GameStateReference = GameStateCast;
	}

	/* Show text that other players are not ready yet */
	TimerText->SetVisibility(ESlateVisibility::Visible);
	TimerText->SetText(FText::FromString(FString("Waiting for other players")));

	/* Stops players from moving until everyone is ready and in-game */
	//if (auto* Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	//{
	//	Controller->SetInputMode(FInputModeUIOnly());
	//}

	/* Toggle character movement - stops movement while menu open */
	//if (auto Owner = Cast<APrototype2Character>(GetOwningPlayer()->GetCharacter()))
	//{
	//	Owner->GetCharacterMovement()->ToggleActive();
	//}
}

void UWidget_StartAndEndMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	UpdateTimerText();

	/* Reenabling player movement after start timer ends */
	//if (bHasReenabledMovement == false)
	//{
	//	if (GameStateReference->GetCountdownLengthSeconds() <= 0)
	//	{
	//		bHasReenabledMovement = true;
	//		/* Toggle character movement - stops movement while menu open */
	//		//if (auto Owner = Cast<APrototype2Character>(GetOwningPlayer()->GetCharacter()))
	//		//{
	//		//	Owner->GetCharacterMovement()->ToggleActive();
	//		//}
	//	}
	//}
}

void UWidget_StartAndEndMenu::UpdateTimerText()
{
	if (TimerText && GameStateReference->Server_Players.Num() >= GameStateReference->GetFinalConnectionCount() && ! GameStateReference->HasGameStarted())
	{
		TimerText->SetText(FText::FromString(FString::FromInt(GameStateReference->GetCountdownLengthSeconds())));
		if (GameStateReference->GetCountdownLengthSeconds() < 1)
		{
			TimerText->SetText(FText::FromString("Go!"));
		}
		CountdownTimer = GameStateReference->GetCountdownLengthSeconds();
		TimerText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else if (TimerText && GameStateReference->HasGameStarted())
	{
		TimerText->SetVisibility(ESlateVisibility::Hidden);
	}
	
	CheckForGameFinished();
}

void UWidget_StartAndEndMenu::CheckForGameFinished()
{
	if (GameStateReference->HasGameFinished() && !GameStateReference->IsGameReadyForVote() && GameStateReference->GetBriefTimesUpLengthSeconds() > 0)
	{
		TimerText->SetText(FText::FromString("Times Up!"));
		TimerText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else if (GameStateReference->HasGameFinished() && GameStateReference->IsGameReadyForVote() && GameStateReference->GetBriefTimesUpLengthSeconds() <= 0)
	{
		TimerText->SetVisibility(ESlateVisibility::Hidden);
	}
}
