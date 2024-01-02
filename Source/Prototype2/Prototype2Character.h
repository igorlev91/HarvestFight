// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Prototype2Character.generated.h"

UCLASS(config=Game)
class APrototype2Character : public ACharacter
{
	GENERATED_BODY()
public:
	APrototype2Character();	
	
protected: // Protected Functions
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/* Called for Attack input */
	void ChargeAttack();

	/* Release Attack */
	void ReleaseAttack();

	/* Execute Attack */
	void ExecuteAttack(float AttackSphereRadius);

	/* Pickup/Plant/Sell */
	void Interact();

	/* Create a sphere collider which calculates nearest item */
	void CheckForInteractables();

	/* Called when hit by another player */
	void GetHit(float AttackCharge, FVector AttackerLocation);
	
	/* UI */
	void OpenIngameMenu();
	
	void PlayNetworkMontage(UAnimMontage* _montage);
	UFUNCTION(Server, Reliable)
	void Server_PlayNetworkMontage(UAnimMontage* _montage);
	void Server_PlayNetworkMontage_Implementation(UAnimMontage* _montage);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayNetworkMontage(UAnimMontage* _montage);
	void Multi_PlayNetworkMontage_Implementation(UAnimMontage* _montage);
	
	UFUNCTION(Server, Reliable)
	void Server_AddHUD();
	void Server_AddHUD_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_AddHUD();
	void Client_AddHUD_Implementation();
	
private: // Input actions
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/* UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MenuAction;

	/* Attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChargeAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReleaseAttackAction;

	/* PickUp/Plant/Sell */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

private: // Animation
	UPROPERTY(EditAnywhere)
	class UAnimMontage* PickupMontage;
	
	UPROPERTY(EditAnywhere)
	class UAnimMontage* AttackMontage;
	
private: // Private variables

	/* Interact radius for checking closest item */
	UPROPERTY(EditAnywhere)
	float InteractRadius = 200.0f;

	/* Maximum amount of Attack Charge */
	UPROPERTY(EditAnywhere)
	float MaxAttackCharge = 5.0f;

	/* Amount of knockback applied which is multiplied by charge */
	UPROPERTY(EditAnywhere)
	float KnockBackAmount = 1000.0f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWidget_PlayerHUD> PlayerHudPrefab;
	UWidget_PlayerHUD* PlayerHUDRef;

public:
	/* Weapon Held */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AWeapon* Weapon;
	
	/* Currently held item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APickUpItem* HeldItem;
	
protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

private:
	class IInteractInterface* ClosestInteractableItem;
	bool bIsChargingAttack;
	float AttackChargeAmount;
	bool bIsStunned;
	float StunTimer;
};
