// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "Engine.h"
#include "Online.h"
#include "Online/CoreOnline.h"
#include "PrototypeGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSearchResults
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class ECharacters : uint8
{
	COW,
	PIG,
	CHICKEN,
	DUCK
};

UENUM(BlueprintType)
enum class ECharacterColours : uint8
{
	RED,
	BLUE,
	GREEN,
	YELLOW
};

UCLASS()
class PROTOTYPE2_API UPrototypeGameInstance : public UGameInstance
{
	GENERATED_BODY()

	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
    
    /** Handle to registered delegate for joining a session */
    FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	
public:
	UPrototypeGameInstance();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void HostOnlineGame();
	
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void FindOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void DestroySessionAndLeaveGame();

	FSearchResults LatestSearchResults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxPlayersOnServer{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FinalConnectionCount{0};

	TArray<FString> FinalPlayerNames;
	TArray<ECharacters> FinalCharacters;
	TArray<ECharacterColours> FinalColours;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacters Character{ECharacters::COW};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterColours CharacterColour{ECharacterColours::RED};
	
protected:
	bool HostSession(FUniqueNetIdRepl UserId, FName SessionName, bool bIsLAN = true, bool bIsPresence = true, int32  = 12);
	void FindSessions(FUniqueNetIdRepl UserId, bool bIsLAN = true, bool bIsPresence = true);
	bool JoinSession(FUniqueNetIdRepl UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);
	
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
		
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	IOnlineSessionPtr SessionInterface;
	

	
};
