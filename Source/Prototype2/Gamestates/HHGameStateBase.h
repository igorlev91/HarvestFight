

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HHGameStateBase.generated.h"

UCLASS()
class PROTOTYPE2_API AHHGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable)
	void SetGameMode(int32 _Mode);

	int32 GetGameMode();
	
	UFUNCTION()
	void OnRep_Gamemode();
	
	UPROPERTY(ReplicatedUsing=OnRep_Gamemode, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 GameMode{};
};
