// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "PrototypeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROTOTYPE2_API UPrototypeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPrototypeGameInstance();

	UFUNCTION(BlueprintCallable)
    void CreateServer();
   
    UFUNCTION(BlueprintCallable)
    void JoinServer();

	UFUNCTION(BlueprintCallable)
    void StopServer();

protected:
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName sessionName, bool succeeded);
	virtual void OnFindSessionComplete(bool succeeded);
	virtual void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	virtual void OnEndSessionComplete(FName sessionName, bool succeeded);


};
