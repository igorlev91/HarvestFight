#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SellBin.h"
#include "Containers/Map.h"
#include "Prototype2Character.generated.h"

class UAudioComponent;
class USoundCue;
class UItemComponent;

/* Enum for controlling particle systems */
UENUM()
enum class EParticleSystem : uint8
{
	Default = 0,

	WalkPoof,
	SprintPoof,
	Sweat,
	AttackTrail,
	Attack,
	Test,

	END
};

UCLASS(config=Game)
class APrototype2Character : public ACharacter
{
	GENERATED_BODY()
public:
	/* Public Functions */
	
	/* Constructor */
	APrototype2Character();	

	/* For networking */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Getter for when character is sprinting */
	bool IsSprinting();

	/* Checks if player is on the ground */
	void CheckForFloorSurface();

	/* Players unique ID */
	UPROPERTY(VisibleAnywhere, Replicated)
	int PlayerID{-1};

	/* Blueprint event for VFX when player attacks */
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerAttackVFX(FVector _Position, float _Radius, float _Charge);

	/* Stretches the weapon mesh on attack */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StretchWeapon();

	/* Squashes the weapon mesh on attack */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SquashWeapon();

	/* Swaps the weapon to a different socket for when attacking or on back */
	UFUNCTION(BlueprintCallable)
	void SocketWeapon(FName _Socket);

	/* Plays audio */
	void PlaySoundAtLocation(FVector _Location, USoundCue* _SoundToPlay, USoundAttenuation* _Attenuation = nullptr);

	/* Ragdolls character - not implemented yet*/
	void Ragdoll(bool _bShouldRagdoll);

	/* PlayerHUD getter */
	class UWidget_PlayerHUD* GetPlayerHUD();

	/* Stencil - on/off */
	void EnableStencil(bool _bIsOn);

	/* Decal Arrow */
	void UpdateDecalDirection(bool _bIsOn);
	void UpdateDecalDirection(bool _bIsOn, bool _bIsTargetingShippingBin);

	/* Player teleporter for start and end of game */
	void TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation = FRotator::ZeroRotator);

	/* Activating and Deactivating particle systems */
	void ActivateParticleSystemFromEnum(EParticleSystem _NewSystem);
	void DeActivateParticleSystemFromEnum(EParticleSystem _NewSystem);

	/* Public Variables */
	
	/* Camera used for end of the game */
	class AEndGameCamera* EndGameCamera;

	/* PlayerHUD reference */
	UPROPERTY(VisibleAnywhere)
	UWidget_PlayerHUD* PlayerHUDRef;

	/* Player State Reference */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class APrototype2PlayerState* PlayerStateRef;

	/* Audio */
	UPROPERTY(EditAnywhere, Replicated)
	USoundAttenuation* SoundAttenuationSettings;
	UPROPERTY(EditAnywhere, Replicated)
	UAudioComponent* ChargeAttackAudioComponent;
	UPROPERTY(EditAnywhere)
	USoundCue* ChargeCue;
	UPROPERTY(EditAnywhere)
	USoundCue* ExecuteCue;
	UPROPERTY(EditAnywhere)
	USoundCue* PickUpCue;
	UPROPERTY(EditAnywhere)
	USoundCue* DropCue;
	UPROPERTY(EditAnywhere)
	USoundCue* SellCue;
	UPROPERTY(EditAnywhere)
	USoundCue* PlantCue;
	UPROPERTY(EditAnywhere)
	USoundCue* GetHitCue;
	UPROPERTY(EditAnywhere)
	USoundCue* MandrakeScreamCue;

	/* Player Material */
	UPROPERTY(VisibleAnywhere, Replicated)
	UMaterialInstance* PlayerMat;

	/* Player Mesh */
	UPROPERTY(VisibleAnywhere, Replicated)
	class USkeletalMesh* PlayerMesh;

	/* Weapon Mesh */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWeapon* Weapon;
	
	/* Currently held item */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APickUpItem* HeldItem;

	/* Is player holding down attack */
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsChargingAttack;

	/* Maximum amount of Attack Charge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAttackCharge = 3.0f;

	/* The Amount of charge the player currently has */
	UPROPERTY(Replicated, BlueprintReadWrite)
	float AttackChargeAmount;

	/* Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CanSprintTime = 5.0f;
	UPROPERTY(Replicated, BlueprintReadWrite, VisibleAnywhere)
	float CanSprintTimer;

	/* Weapon degrading */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int WeaponCurrentDurability;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int WeaponMaxDurability = 5;

	/* The closest interactable item for HUD showing popup text */
	class IInteractInterface* ClosestInteractableItem;

	/* Closest Interactable Actor for player interacting with things */
	UPROPERTY(VisibleAnywhere)
	AActor* ClosestInteractableActor;

	/* Bool for checking if player is holding gold to slow them down */
	UPROPERTY(Replicated, EditAnywhere)
	bool bIsHoldingGold;

	/* Varaibles needed for Decal component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* DecalArmSceneComponent;
	UPROPERTY(EditAnywhere)
	UDecalComponent* DecalComponent;
	UPROPERTY(EditAnywhere)
	FVector StartPosition{};
	UPROPERTY(EditAnywhere)
	bool bDecalOn{false};
	UPROPERTY(EditAnywhere)
	bool bDecalTargetShippingBin{false};
	UPROPERTY(EditAnywhere)
	ASellBin* SellBin;

	/* Variables needed for VFX */
	UPROPERTY(EditAnywhere, Category = VFX)
	UStaticMeshComponent* AttackAreaIndicatorMesh;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* WalkPoof_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* SprintPoof_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Sweat_NiagaraComponent;
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = VFX)
	class UNiagaraComponent* AttackTrail_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Attack_NiagaraComponent;
	UPROPERTY(Replicated, EditAnywhere, Category = VFX)
	class UNiagaraComponent* Dizzy_NiagaraComponent;
	UPROPERTY(VisibleAnywhere)
	TArray<EParticleSystem> ParticleSystemsToActivate;
	UPROPERTY(VisibleAnywhere)
	TArray<EParticleSystem> ParticleSystemsToDeActivate;
	UPROPERTY(EditAnywhere)
	bool ToggleNiagraTestComponent{false};
	
protected:
	/* Protected Functions */

	/* Input Component Setup */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Begin play function */
	virtual void BeginPlay();

	/* Tick Function*/
	virtual void Tick(float DeltaSeconds) override;

	/* Called when player presses interact key */
	void TryInteract();

	/** Called for movement input */
	void Move(const FInputActionValue& _Value);

	/** Called for looking input */
	void Look(const FInputActionValue& _Value);

	/* Called for Attack input */
	void ChargeAttack();

	/* Release Attack */
	void ReleaseAttack();

	/* Execute Attack */
	void ExecuteAttack(float _AttackSphereRadius);

	/* Pickup/Plant/Sell */
	void Interact();

	/* Activate Sprint */
	void Sprint();

	/* Handle character speed */
	void UpdateCharacterSpeed(float _WalkSpeed, float _SprintSpeed, float _BaseAnimationRateScale);
	
	/* Create a sphere collider which calculates nearest item */
	void CheckForInteractables();
	
	/* Called when hit by another player */
	UFUNCTION(BlueprintCallable)
	void GetHit(float _AttackCharge, FVector _AttackerLocation);
	
	/* UI */
	void OpenIngameMenu();
	
	/* Update decal direction */
	void UpdateDecalAngle();

	/* Updates the area of attack indicator */
	void UpdateAOEIndicator();

	/* Protected variables */
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/* Playing montages */
	void PlayNetworkMontage(UAnimMontage* _Montage);

private:
	/* Private variables */
	/* Input */
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

	/* Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/* Animation */
	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* PickupMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* ExecuteAttackMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* ExecuteAttackMontage_LongerWindUp;
	
	/* Interact radius for checking closest item */
	UPROPERTY(EditAnywhere)
	float InteractRadius = 225.0f;
	
	/* Amount of knockback applied which is multiplied by charge */
	UPROPERTY(EditAnywhere, Category = KnockBack)
	float KnockBackAmount = 1000.0f;
	UPROPERTY(EditAnywhere, Category = KnockBack)
	float MaxKnockBackVelocity = 10000.0f;
	
	/* Interact timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InteractTimerTime = 1.0f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InteractTimer{};
	
	/* Attack timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float AttackTimerTime = 1.0f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float AttackTimer{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InstantAttackDelay = 0.2f;
	UPROPERTY(EditAnywhere)
	float ItemLaunchStrength = 500000.0f;

	/* Player HUD Prefab */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWidget_PlayerHUD> PlayerHudPrefab;
	
	/* References to change speed: Sprint/Walk/Slow Walk */
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimSequence* RunAnimation;	
	UPROPERTY(EditAnywhere, Category = Animation)
	TArray<UAnimSequence*> RunAnimations;

	/* Friction for when player is on the ice */
	UPROPERTY(EditAnywhere)
	float IceFriction{0.1f};

	/* Player regular walk speed */
	float WalkSpeed = 500.f;

	/* Speed for when player is holding gold plant */
	float GoldPlantSpeed = 300.0f;

	/* Speed for when player is sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintSpeed = 750.0f;

	/* How long the player sprints for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTime = 2.0f;

	/* Timer for handling sprint */
	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTimer;

	/* Rate scales for adjusting animation speed */
	UPROPERTY(EditAnywhere, Category = RateScale)
	float WalkRateScale = 1.5f;
	UPROPERTY(EditAnywhere, Category = RateScale)
	float GoldSlowRateScale = 0.7f;
	UPROPERTY(EditAnywhere, Category = RateScale)
	float SprintRateScaleScalar = 1.5f;

	/* Attack */
	UPROPERTY(EditAnywhere)
	float InstantAttackThreshold = 1.0f;
	bool bCanAttack = true;

	/* Stun variables */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bIsStunned;	
	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float StunTimer;
	UPROPERTY(Replicated, VisibleAnywhere)
	FTransform LocationWhenStunned{};	
	UPROPERTY(Replicated, VisibleAnywhere)
	FTransform MeshLocationWhenStunned{};

	/* Vector for trajectory of being knocked away from a hit */
	UPROPERTY(EditAnywhere, Category=Attack)
	FVector KnockUp = {1.0f, 1.0f, 100000000.0f};

	/* Attack radius and reach */
	float BaseAttackRadius = 75.0f;
	float WeaponAttackRadiusScalar = 30.0f;
	float WeaponReach = 100.0f;
	float MeleeReach = 30.0f;

public: /* Pubic Networking */
	
	/* RPC for picking up items */
	UFUNCTION(Server, Reliable)
	void Server_PickupItem(UItemComponent* _ItemComponent, APickUpItem* _Item);
	void Server_PickupItem_Implementation(UItemComponent* _ItemComponent, APickUpItem* _Item);

	/* RPC for dropping items */
	UFUNCTION(Server, Reliable)
	void Server_DropItem();
	void Server_DropItem_Implementation();

	/* RPC for socketing item, used for weapon */
	UFUNCTION(Server, Reliable)
	void Server_SocketItem(UStaticMeshComponent* _Object, FName _Socket);
	void Server_SocketItem_Implementation(UStaticMeshComponent* _Object, FName _Socket);

	/* Multicast for socketing items, used for weapon*/
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SocketItem(UStaticMeshComponent* _Object, FName _Socket);
	void Multi_SocketItem_Implementation(UStaticMeshComponent* _Object, FName _Socket);

	/* RPC for dropping a weapon */
	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();
	void Server_DropWeapon_Implementation();

	/**
	 * @brief Starting attack sets bIsChargingAttack to true,
	 * Drops item if holding one, and sockets weapon to attacking socket
	 */
	 UFUNCTION(Server, Reliable)
	void Server_StartAttack();
	void Server_StartAttack_Implementation();

	/* RPC for when attack key is released */
	UFUNCTION(Server, Reliable)
	void Server_ReleaseAttack();
	void Server_ReleaseAttack_Implementation();

	/* Multicast for when attack key is released */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ReleaseAttack();
	void Multi_ReleaseAttack_Implementation();

	/* Adding a HUD for to player */
	UFUNCTION(Server, Reliable)
	void Server_AddHUD();
	void Server_AddHUD_Implementation();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Client_AddHUD();
	void Multi_Client_AddHUD_Implementation();	
	UFUNCTION(Client, Reliable)
	void Client_AddHUD();
	void Client_AddHUD_Implementation();

	/* Playing Audio */
	UFUNCTION(Server, Reliable)
	void Server_PlaySoundAtLocation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);
	void Server_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlaySoundAtLocation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);
	void Multi_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);

	/* Ragdoll */
	UFUNCTION(Server, Reliable)
	void Server_Ragdoll(bool _Ragdoll);
	void Server_Ragdoll_Implementation(bool _Ragdoll);	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Ragdoll(bool _Ragdoll);
	void Multi_Ragdoll_Implementation(bool _Ragdoll);

	/* Teleporting at start and end of game */
	UFUNCTION(Server, Reliable)
	void Server_TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation);
	void Server_TeleportToLocation_Implementation(FVector _DestinationLocation, FRotator _DestinationRotation);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation);
	void Multi_TeleportToLocation_Implementation(FVector _DestinationLocation, FRotator _DestinationRotation);

	/* Toggling particle effects */
	UFUNCTION(Server, Reliable)
	void Server_ToggleParticleSystems(const TArray<EParticleSystem>& _On, const TArray<EParticleSystem>& _Off);
	void Server_ToggleParticleSystems_Implementation(const TArray<EParticleSystem>& _On, const TArray<EParticleSystem>& _Off);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleParticleSystems(const TArray<EParticleSystem>& _On, const TArray<EParticleSystem>& _Off);
	void Multi_ToggleParticleSystems_Implementation(const TArray<EParticleSystem>& _On, const TArray<EParticleSystem>& _Off);
	
protected: /* Protected Networking */
	/* The Ideal Net Role for if human controlling */
	ENetRole IdealNetRole{ROLE_AutonomousProxy};

	/* Playing animation montages */
	UFUNCTION(Server, Reliable)
	void Server_PlayNetworkMontage(UAnimMontage* _Montage);
	void Server_PlayNetworkMontage_Implementation(UAnimMontage* _Montage);	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayNetworkMontage(UAnimMontage* _Montage);
	void Multi_PlayNetworkMontage_Implementation(UAnimMontage* _Montage);

	/* Setting player colour*/
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerColour();
	void Server_SetPlayerColour_Implementation();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlayerColour();
	void Multi_SetPlayerColour_Implementation();

	/* Sprint RPC */
	UFUNCTION(Server, Reliable)
	void Server_Sprint();
	void Server_Sprint_Implementation();

	/* Interact RPC */
	UFUNCTION(Server, Reliable)
	void Server_TryInteract();
	void Server_TryInteract_Implementation();

	/* Multicast dropping an item */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropItem();
	void Multi_DropItem_Implementation();

	/* Multicast for picking up item */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PickupItem(UItemComponent* _ItemComponent, APickUpItem* _Item);
	void Multi_PickupItem_Implementation(UItemComponent* _ItemComponent, APickUpItem* _Item);

	/* Mutlicast for dropping a weapon */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropWeapon();
	void Multi_DropWeapon_Implementation();

	/* Receiving materials for the farmer costume */
	UFUNCTION(Server, Reliable)
	void Server_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& _InMaterialsArray);
	void Server_ReceiveMaterialsArray_Implementation(const TArray<UMaterialInstance*>& _InMaterialsArray);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& _InMaterialsArray);
	void Multi_ReceiveMaterialsArray_Implementation(const TArray<UMaterialInstance*>& _InMaterialsArray);

	/* Potentially unused particle functions */
	UFUNCTION(Server, Reliable)
	void Server_FireParticleSystem(UNiagaraSystem* _NiagaraSystem, FVector _Position);
	void Server_FireParticleSystem_Implementation(UNiagaraSystem* _NiagaraSystem, FVector _Position);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_FireParticleSystem(UNiagaraSystem* _NiagaraSystem, FVector _Position);
	void Multi_FireParticleSystem_Implementation(UNiagaraSystem* _NiagaraSystem, FVector _Position);

	/* New way of activating/deactivating particle systems */
	UFUNCTION(Server, Reliable)
	void Server_SetParticleActive(UNiagaraComponent* _NiagaraComponent, bool _bIsActive);
	void Server_SetParticleActive_Implementation(UNiagaraComponent* _NiagaraComponent, bool _bIsActive);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetParticleActive(UNiagaraComponent* _NiagaraComponent, bool _bIsActive);
	void Multi_SetParticleActive_Implementation(UNiagaraComponent* _NiagaraComponent, bool _bIsActive);

	/* Charging weapon sound control */
	UFUNCTION(Server, Reliable)
	void Server_ToggleChargeSound(bool _bIsSoundEnabled);
	void Server_ToggleChargeSound_Implementation(bool _bIsSoundEnabled);	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleChargeSound(bool _bIsSoundEnabled);
	void Multi_ToggleChargeSound_Implementation(bool _bIsSoundEnabled);

	/* Countdown timers */
	UFUNCTION(Server, Reliable)
	void Server_CountdownTimers(float _DeltaSeconds);
	void Server_CountdownTimers_Implementation(float _DeltaSeconds);
};