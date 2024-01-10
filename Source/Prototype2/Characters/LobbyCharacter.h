

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
	void SetPlayerStateRef(class ALobbyPlayerState* _NewLobbyPlayerState);
	void SetNameWidget(FString _name);
	void SetNameWidgetPlayerRef();
	
	/* Protected Functions */
protected:
	ALobbyCharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* _PlayerInputComponent) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetNameWidget(const FString& _name);
	
	UFUNCTION(Server, Reliable)
	void Server_SetCharacterMesh();
	void Server_SetCharacterMesh_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetCharacterMesh();
	void Multi_SetCharacterMesh_Implementation();

	void SyncCharacterSkin();

	/* Public Variables */
public:
	/* Haybale to stand on (if player 4+ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* HayBaleMeshComponent;
	
	/* Protected Variables */
protected:
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ALobbyPlayerState* PlayerStateRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<USkeletalMesh*> PlayerMeshes;

	/* Display name widget (above head) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* PlayerNameWidgetComponent;
	
	UPROPERTY()
	class UWidget_PlayerName* PlayerNameWidget{nullptr};

	
	/* Ready button widget (above head) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* ReadyImageWidgetComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstanceDynamic*> PlayerMaterialsDynamic{};
};
