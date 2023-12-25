// Fill out your copyright notice in the Description page of Project Settings.


#include "PrototypeGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

UPrototypeGameInstance::UPrototypeGameInstance()
{
	
}

void UPrototypeGameInstance::Init()
{
	Super::Init();

	if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPrototypeGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPrototypeGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPrototypeGameInstance::OnJoinSessionComplete);
			SessionInterface->OnEndSessionCompleteDelegates.AddUObject(this, &UPrototypeGameInstance::OnEndSessionComplete);
		}
	}
}

void UPrototypeGameInstance::OnCreateSessionComplete(FName sessionName, bool succeeded)
{
	if (succeeded)
	{
		GetWorld()->ServerTravel("/Game/Maps/Level_Main?listen");
	}
}

void UPrototypeGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if(APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(sessionName, JoinAddress);
		if(JoinAddress != "")
		{
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UPrototypeGameInstance::CreateServer()
{

	
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() != "NULL" ? false : true;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUsesPresence = true;
	sessionSettings.NumPublicConnections = 6;
	SessionInterface->CreateSession(0, FName("New Session"), sessionSettings);
}

void UPrototypeGameInstance::JoinServer()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UPrototypeGameInstance::StopServer()
{
	if (SessionInterface->GetNamedSession(FName("New Session")))
	{
		SessionInterface->EndSession(FName("New Session"));
	}
}

void UPrototypeGameInstance::OnFindSessionComplete(bool succeeded)
{
	
	if (succeeded)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		UE_LOG(LogTemp, Warning, TEXT("Sessions Found: %s"), *FString::FromInt(SearchResults.Num()) );
		if (SearchResults.Num() > 0)
		{
			SessionInterface->JoinSession(0, FName("New Session"), SearchResults[0]);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Not Found") );
	}
}

void UPrototypeGameInstance::OnEndSessionComplete(FName sessionName, bool succeeded)
{
	if(APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PController->ClientTravel("/Game/Maps/Level_MainMenu", ETravelType::TRAVEL_Absolute);
	}
}
