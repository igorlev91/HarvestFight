#include "Prototype2Gamestate.h"

#include "Net/UnrealNetwork.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"

APrototype2Gamestate::APrototype2Gamestate()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APrototype2Gamestate::BeginPlay()
{
	Super::BeginPlay();
}

void APrototype2Gamestate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickCountdownTimer(DeltaSeconds);
	TickMatchTimer(DeltaSeconds);
	TickEndGameTimer(DeltaSeconds);
}

void APrototype2Gamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, MatchLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, CountdownLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, CountdownLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, bGameHasStarted);
	DOREPLIFETIME(APrototype2Gamestate, bHasGameFinished);
	DOREPLIFETIME(APrototype2Gamestate, bGameReadyForVote);

	DOREPLIFETIME(APrototype2Gamestate, EndGameLengthMinutes);
	DOREPLIFETIME(APrototype2Gamestate, EndGameLengthSeconds);
	DOREPLIFETIME(APrototype2Gamestate, BriefTimesUpEndGameLengthSeconds);
	
	DOREPLIFETIME(APrototype2Gamestate, bIsCountingDown);
	DOREPLIFETIME(APrototype2Gamestate, bPreviousServerTravel);
	
	DOREPLIFETIME(APrototype2Gamestate, Server_Players);
	
	DOREPLIFETIME(APrototype2Gamestate, MaxPlayersOnServer);
	DOREPLIFETIME(APrototype2Gamestate, FinalConnectionCount);

	DOREPLIFETIME(APrototype2Gamestate, PlayerWinner);
}

void APrototype2Gamestate::TickCountdownTimer(float DeltaSeconds)
{
	if (HasAuthority() && !bGameHasStarted)
	{
		if (Server_Players.Num() >= FinalConnectionCount)
		{
			if (CountdownLengthSeconds > 0)
			{
				CountdownLengthSeconds -= DeltaSeconds;
			}
			else
			{
				CountdownLengthMinutes--;
				CountdownLengthSeconds = 60;

				
			}
			
			if (CountdownLengthSeconds <= 0 && CountdownLengthMinutes <= 0)
			{
				bGameHasStarted = true;
				UE_LOG(LogTemp, Warning, TEXT("Countdown completed"));
			}
		}

		//UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), ( GameHasStarted ? TEXT("true") : TEXT("false") ));
	}
}

void APrototype2Gamestate::TickMatchTimer(float DeltaSeconds)
{
	//UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), ( GameHasStarted ? TEXT("true") : TEXT("false") ));
	if (HasAuthority() && bGameHasStarted && !bHasGameFinished)
	{
		if (MatchLengthSeconds > 0)
		{
			MatchLengthSeconds -= DeltaSeconds;

			if (MatchLengthMinutes <= 0 && MatchLengthSeconds <= 0)
			{
				bHasGameFinished = true;
				PupeteerCharactersForEndGame();
			}
		}
		else
		{
			MatchLengthMinutes--;
			MatchLengthSeconds = 60;
		}
	}
}

void APrototype2Gamestate::TickEndGameTimer(float DeltaSeconds)
{
	// check if game has "finished"
	if (bHasGameFinished && !bGameReadyForVote)
	{
		if (EndGameLengthSeconds > 0)
		{
			EndGameLengthSeconds -= DeltaSeconds;
		}
		else
		{
			EndGameLengthMinutes--;
			EndGameLengthSeconds = 60;
		}

		if (EndGameLengthSeconds <= 0.0f && EndGameLengthMinutes <= 0)
		{
			bGameReadyForVote = true;
		}
	}
}

int32 APrototype2Gamestate::GetFinalConnectionCount() const
{
	return FinalConnectionCount;
}

void APrototype2Gamestate::SetFinalConnectionCount(int32 _FinalConnectionCount)
{
	FinalConnectionCount = _FinalConnectionCount;
}

void APrototype2Gamestate::SetMaxPlayersOnServer(int32 _FinalConnectionCount)
{
	MaxPlayersOnServer = _FinalConnectionCount;
}

void APrototype2Gamestate::SetPlayerWinner(int32 _WinnerID)
{
	PlayerWinner = _WinnerID;
}

int32 APrototype2Gamestate::RegisterPlayer(APrototype2PlayerState* _Player)
{
	return Server_Players.Add(_Player);
}

void APrototype2Gamestate::UnRegisterPlayer(APrototype2PlayerState* _Player)
{
	Server_Players.Remove(_Player);
}

int32 APrototype2Gamestate::GetCurrentConnectionCount()
{
	return Server_Players.Num();
}

bool APrototype2Gamestate::HasGameFinished()
{
	return bHasGameFinished;
}

bool APrototype2Gamestate::HasGameStarted()
{
	return bGameHasStarted;
}

bool APrototype2Gamestate::IsGameReadyForVote()
{
	return bGameReadyForVote;
}

int32 APrototype2Gamestate::GetMatchLengthMinutes()
{
	return MatchLengthMinutes;
}

int32 APrototype2Gamestate::GetMatchLengthSeconds()
{
	return MatchLengthSeconds;
}

int32 APrototype2Gamestate::GetCountdownLengthMinutes()
{
	return CountdownLengthMinutes;
}

int32 APrototype2Gamestate::GetCountdownLengthSeconds()
{
	return CountdownLengthSeconds;
}

int32 APrototype2Gamestate::GetBriefTimesUpLengthSeconds()
{
	return BriefTimesUpEndGameLengthSeconds;
}

int32 APrototype2Gamestate::GetPlayerWinner()
{
	return PlayerWinner;
}

void APrototype2Gamestate::UpdatePlayerDetails(int32 _Player, FCharacterDetails _CharacterDetails)
{
	Server_Players[_Player]->Details = _CharacterDetails;
}

void APrototype2Gamestate::PupeteerCharactersForEndGame()
{
	AActor* EndGamePodiumActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEndGamePodium::StaticClass());
	AEndGamePodium* EndGamePodiumActorCasted = Cast<AEndGamePodium>(EndGamePodiumActor);
	
	int32 WinningPlayer{};
	int32 HighestCoins{};
		
	for(auto PlayerState : PlayerArray)
	{
		if (APrototype2PlayerState* CastedPlayerstate = Cast<APrototype2PlayerState>(PlayerState))
		{
			if (CastedPlayerstate->Coins > HighestCoins)
			{
				HighestCoins = CastedPlayerstate->Coins;
			}
		}
	}

	if (EndGamePodiumActorCasted)
	{
		for(int32 i = 0; i < PlayerArray.Num(); i++)
		{
			if (APrototype2PlayerState* CastedPlayerState = Cast<APrototype2PlayerState>(PlayerArray[i]))
			{
				if (APrototype2Character* CastedPlayerCharacter = Cast<APrototype2Character>(CastedPlayerState->GetPlayerController()->GetCharacter()))
				{
					if (HighestCoins > 0 && CastedPlayerState->Coins == HighestCoins)
						CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetWinPosition(WinningPlayer)->GetComponentTransform());
					else
						CastedPlayerCharacter->TeleportToEndGame(EndGamePodiumActorCasted->GetLossPosition(i)->GetComponentTransform());
					
					if (auto ControllerCast = Cast<APrototype2PlayerController>(CastedPlayerState->GetPlayerController()))
					{
						ControllerCast->SetViewTarget_Networked(EndGamePodiumActorCasted->EndGameCamera);
					}
				}
			}
		}
	}
}

