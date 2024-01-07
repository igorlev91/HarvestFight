

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "GameFramework/Character.h"
#include "LobbyCharacter.generated.h"

class UWidgetComponent;
UCLASS()
class PROTOTYPE2_API ALobbyCharacter : public ACharacter
{
	GENERATED_BODY()

	/* Public Functions */
public:
	void SetPlayerMat(UMaterialInstance* _NewPlayerMat);
	void SetPlayerStateRef(class ALobbyPlayerState* _NewLobbyPlayerState);

	/* Protected Functions */
protected:
	ALobbyCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* _PlayerInputComponent) override;

	UFUNCTION(Server, Reliable)
	void Server_SetCharacterMesh();
	void Server_SetCharacterMesh_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetCharacterMesh();
	void Multi_SetCharacterMesh_Implementation();

	/* Public Variables */
public:
	UMaterialInstance* GetPlayerMat();
	
	/* Protected Variables */
protected:
	UPROPERTY(VisibleAnywhere, Replicated)
	UMaterialInstance* PlayerMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ALobbyPlayerState* PlayerStateRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<USkeletalMesh*> PlayerMeshes;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* PlayerNameWidgetComponent;
	
};
