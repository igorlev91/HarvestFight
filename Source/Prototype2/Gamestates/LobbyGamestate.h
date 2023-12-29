#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGamestate.generated.h"


UCLASS()
class PROTOTYPE2_API ALobbyGamestate : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALobbyGamestate();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldServerTravel{false};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int LobbyLengthMinutes{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float LobbyLengthSeconds{10.0f};

private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool IsCountingDown{};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool PreviousServerTravel{};
};
