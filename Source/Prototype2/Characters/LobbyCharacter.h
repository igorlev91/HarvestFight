#pragma once

#include "CoreMinimal.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "GameFramework/Character.h"
#include "LobbyCharacter.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	UMaterialInstance* GetPlayerMat();
	void SetPlayerMat(UMaterialInstance* _NewPlayerMat);
	void SetPlayerStateRef(class ALobbyPlayerState* _NewLobbyPlayerState);
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
	
	UPROPERTY(VisibleAnywhere, Replicated)
	UMaterialInstance* PlayerMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ALobbyPlayerState* PlayerStateRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<USkeletalMesh*> PlayerMeshes;
};
