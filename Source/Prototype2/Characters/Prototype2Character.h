

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Prototype2/Gameplay/SellBin.h"
#include "Containers/Map.h"
#include "Prototype2/DataAssets/WeaponData.h"
#include "Prototype2/DataAssets/SeedData.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2Character.generated.h"

class APrototype2PlayerController;
class UAnimationData;
class UAudioComponent;
class USoundCue;
class UItemComponent;
class UDebuffComponent;
class Smite;

/* Enum for controlling particle systems */
UENUM()
enum class EParticleSystems : uint8
{
	Default = 0,

	WalkPoof,
	SprintPoof,
	Sweat,
	AttackTrail,
	Attack,
	Dizzy,
	Test,

	END
};

UCLASS(config=Game)
class APrototype2Character : public ACharacter
{
	GENERATED_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///												Initialization													 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/* Adding a HUD to player */
	UFUNCTION(Server, Unreliable)
	void Server_AddHUD();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_Client_AddHUD();
	UFUNCTION(Client, Unreliable)
	void Client_AddHUD();

	/* PlayerHUD reference */
	UPROPERTY(VisibleAnywhere)
	UWidget_PlayerHUD* PlayerHUDRef;

	/* Player State Reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class APrototype2PlayerState* PlayerStateRef;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	UDebuffComponent* DebuffComponent;
	
protected:
	/* Constructor */
	APrototype2Character();	

	/* For networking */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/* Begin play function */
	virtual void BeginPlay();

	/* Input Component Setup */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void AssignHelperReferences();
	UFUNCTION()
	void InitInputMappingContext();
	UFUNCTION()
	void InitPlayerNameWidgetComponent();
	UFUNCTION()
	void InitEmoteWidgetComponent();
	UFUNCTION()
	void InitWeapon();
	UFUNCTION()
	void InitPlayerHUD();
	UFUNCTION()
	void InitShippingBin();
	UFUNCTION()
	void InitDecals();
	UFUNCTION()
	void InitCharacterMovementComponent();
	UFUNCTION()
	void InitCameraStuff();
	UFUNCTION()
	void InitMeshAndCapsule();
	UFUNCTION()
	void InitNiagraComponents();
	UFUNCTION()
	void InitMiscComponents();

	void SyncCharacterColourWithPlayerState();

	/* Setting player colour*/
	UFUNCTION(Server, Unreliable)
	void Server_SetPlayerColour();
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SetPlayerColour();

	UPROPERTY()
	APrototype2PlayerController* PlayerController;
	
	UPROPERTY()
	APrototype2Gamestate* GameState;

	/* Camera used for end of the game */
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* EndGameCamera;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/* Display name widget (above head) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* PlayerNameWidgetComponent;
	
	UPROPERTY()
	class UWidget_PlayerName* PlayerNameWidget{nullptr};

	/* Emote above head */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UWidgetComponent* EmoteWidgetComponent;

	UPROPERTY()
	class UWidget_PlayerEmote* EmoteWidget{nullptr};
	
	/* Player HUD Prefab */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<class UWidget_PlayerHUD> PlayerHudPrefab;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{nullptr},{nullptr},{nullptr},{nullptr}};

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstanceDynamic*> PlayerMaterialsDynamic{};

	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimationData*> AllAnimationDatas;

	// Smite
	Smite* smite = nullptr;

public:
	void Multi_SetSmite(Smite* _smite);
	Smite* GetSmite() const {return smite;}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///														Tick													 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void TogglePlayerStencil();
	
	void DelayedBeginPlay();

	UFUNCTION()
	void ClearInteractionText();
	
	/* Checks if player is on the ground */
	void CheckForFloorSurface(); 

	/* Checks whether player is falling, and when they've landed */
	void CheckForFalling(float _DeltaTime);
	
	UFUNCTION()
	void HandleAttackChargeBehavior(float _DeltaSeconds);
	
	/* Create a sphere collider which calculates nearest item */
	void CheckForInteractables();
	
	UFUNCTION()
	void UpdatePlayerHUD();

	UFUNCTION()
	void UpdateWeaponMeshSkin();
	
	UFUNCTION()
	void UpdateParticleSystems();

	/* Update decal direction */
	void UpdateDecalAngle(); //todo: we planning on using this?

	/* Billboarding player names + removing current players name */
	void UpdatePlayerNames();

	/* Billboarding emote */
	void UpdateEmote();

	/* Timers on tick */
	UFUNCTION()
	void TickTimers(float _DeltaSeconds);
	UFUNCTION()
	void DeltaDecrement(float& _Variable, float& _DeltaSeconds);
	UFUNCTION()
	void DecrementSprintTimers(float _DeltaSeconds);
	
	UFUNCTION()
	void ToggleParticleSystems();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///														Input													 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:	
	/* Used to stop player input moving the player */
	UPROPERTY(Replicated, VisibleAnywhere)
	bool bAllowMovementFromInput = true;

	UPROPERTY(Replicated, VisibleAnywhere)
	bool bIsHoldingInteract = false;

	
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& _Value);

	/** Called for looking input */
	void Look(const FInputActionValue& _Value);

	/* Activate Sprint */
	void Sprint();
	/* End Sprint */
	void EndSprint();

	/* Jump */
	void Jump();
	
	/* Called for Attack input */
	void ChargeAttack();
	
	/* Release Attack */
	void ReleaseAttack();
	
	/* Pickup/Plant/Sell */
	void Interact();
	UFUNCTION(Server, Reliable)
	void Server_Interact();

	/* For holding interact functionality*/
	void HoldInteract();
	
	/* For the releasing of holding interact functionality*/
	void ReleaseHoldInteract();
	
	/* UI */
	void OpenIngameMenu();
	void ShowRadialEmoteMenu();
	void DisableRadialEmoteMenu();

	/* Canceled by being hit by other player */
	void CancelChargeAttack();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///													Interact													 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/* Pickup function for doing stuff that doens't need rpc/multi, but calls the rpc which calls multi */
	void PickupItem(APickUpItem* _Item, EPickupActor _PickupType);

	/* Client side Drop Item function */
	void DropItem(float WhoopsyStrength = 500.0f);

	bool HasClaimedPlot();
	void SetClaimedPlot(class ARadialPlot* _Plot);

	UFUNCTION(Client, Reliable)
	void Client_BroadcastPlotClaimed(ARadialPlot* _Plot);
	
	/* Client RPC for picking up items */
	UFUNCTION(Client, Reliable)
	void Client_PickupItem(APickUpItem* _Item, EPickupActor _PickupType);
	
	/* RPC for picking up items */
	UFUNCTION(Server, Reliable)
	void Server_PickupItem(APickUpItem* _Item, EPickupActor _PickupType);

	/* Client RPC for dropping items */
	UFUNCTION(Client, Reliable)
	void Client_DropItem();
	
	/* RPC for dropping items */
	UFUNCTION(Server, Reliable)
	void Server_DropItem(float WhoopsyStrength = 500.0f);
	
	/* For holding interact functionality*/
	UFUNCTION(Server, Reliable)
	void Server_HoldInteract();
	
	/* For the releasing of holding interact functionality*/
	UFUNCTION(Server, Reliable)
	void Server_ReleaseHoldInteract();

	/* Delegate for claiming plot on hold interact */
	UFUNCTION(Client, Reliable)
	void Client_OnClaimingPlot(float _CurrentClaimTime, float _TotalClaimDuration);
	/* Delegate for claiming plot on hold interact */
	UFUNCTION(Client, Reliable)
	void Client_OnStoppedClaimingPlot();
	
	/* Currently held item */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APickUpItem* HeldItem;

	/* Bool for checking if player is holding gold to slow them down */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Gold")
	bool bIsHoldingGold;

	
	/* The closest interactable item for HUD showing popup text */
	class IInteractInterface* ClosestInteractableItem;

	/* Closest Interactable Actor for player interacting with things */
	UPROPERTY(VisibleAnywhere)
	AActor* ClosestInteractableActor;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	class ARadialPlot* ClaimedPlot{};

protected:
	/* Multicast dropping an item */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropItem();

	/* Multicast for picking up item */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PickupItem(APickUpItem* _Item, EPickupActor _PickupType);

	/* Interact radius for checking closest item */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float InteractRadius = 150.0f;
	
	/* Interact timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InteractTimerTime = 1.0f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InteractTimer{};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///													Combat														 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/* Swaps the weapon to a different socket for when attacking or on back */
	UFUNCTION(BlueprintCallable)
	void SocketWeapon(FName _Socket);
	
	/* Called when hit by another player */
	UFUNCTION(BlueprintCallable)
	void GetHit(float _AttackCharge, FVector _AttackerLocation, UWeaponData* _OtherWeaponData);
	
	/* Allows for client only functionality when dropping weapon */
	void DropWeapon();

	/* RPC for socketing item, used for weapon */
	UFUNCTION(Server, Reliable)
	void Server_SocketItem(UStaticMeshComponent* _Object, FName _Socket);

	/* Multicast for socketing items, used for weapon*/
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SocketItem(UStaticMeshComponent* _Object, FName _Socket);

	/* Client RPC for dropping a weapon */
	UFUNCTION(Client, Reliable)
	void Client_DropWeapon();
	
	/* RPC for dropping a weapon */
	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();

	/**
	 * @brief Chargeattack sets bIsChargingAttack to true,
	 * Drops item if holding one, and sockets weapon to attacking socket
	 */
	UFUNCTION(Server, Reliable)
	void Server_ChargeAttack();

	void SetPlayerAimingMovement(bool _bIsAiming);
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerAimingMovement(bool _bIsAiming);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPlayerAimingMovement(bool _bIsAiming);
	UFUNCTION(Client, Reliable)
	void Client_SetPlayerAimingMovement(bool _bIsAiming);

	UFUNCTION(Server, Reliable)
	void Server_CancelChargeAttack();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_CancelChargeAttack();

	/* RPC for when attack key is released */
	UFUNCTION(Server, Reliable)
	void Server_ReleaseAttack(FVector _CachedActorLocation, FVector _CachedForwardVector);

	/* Multicast for when attack key is released */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ReleaseAttack();

	/* Mutlicast for dropping a weapon */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_DropWeapon();

	/* Called from ExecuteAttack() in UWeapon derived class to reset attack variables */
	void ResetAttack();
	UFUNCTION(Server, Reliable)
	void Server_ResetAttack();
	
	/* Execute Attack */
	void ExecuteAttack(float _AttackSphereRadius, float _AttackChargeAmount, bool _bSprinting);
	
	/* RPC for executing attack*/
	UFUNCTION(Server, Reliable)
	void Server_ExecuteAttack(float _AttackSphereRadius, float _AttackChargeAmount, bool _bSprinting);
	
	UFUNCTION(Client, Reliable)
	void Client_BroadcastOvercharge();

	void FollowThroughOnAttack();
	bool bIsFollowingThroughAttack = false;

	void SetCanAttack(bool _InbCanAttack) { bCanAttack = _InbCanAttack; }

	UFUNCTION()
	void OnRep_ChargeStateChanged();

	UFUNCTION(Server, Reliable)
	void Server_SetAOEIndicatorVisibility(bool _bIsVisible);
	
	void ThrowItem();
	UFUNCTION(Server, Reliable)
	void Server_ThrowItem();
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ThrowItem();
	UPROPERTY(EditDefaultsOnly)
	float ThrowItemStrength = 500.0f;
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyHoneyGoopSlow();
	
	UPROPERTY(Replicated)
	bool bShouldWeaponFlashRed;
	UPROPERTY(Replicated)
	float WeaponFlashTimer;
	float WeaponFlashDuration = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterial* FlashRedMaterial;
	
	/* Weapon Mesh */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWeapon* Weapon;
	
	/* Is player holding down attack */
	UPROPERTY(BlueprintReadOnly)
	bool bIsChargingAttack;

	UPROPERTY(ReplicatedUsing=OnRep_ChargeStateChanged)
	bool bChargeAnimationState;

	/* Maximum amount of Attack Charge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAttackCharge = 3.0f;

	/* The Amount of charge the player currently has */
	UPROPERTY(BlueprintReadWrite)
	float AttackChargeAmount;
	
	/* The Amount of charge the player currently has */
	UPROPERTY(BlueprintReadWrite)
	float AutoAttackTimer;
	
	/* The Amount of charge the player currently has */
	UPROPERTY()
	float AutoAttackDuration = 2.0f;
		
	/* Weapon degrading */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int WeaponCurrentDurability;

	/* Default Weapon Data Asset is no weapon (punching with fists) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	USeedData* DefaultWeaponSeedData;
	
	/* Bool for checking if player is holding gold weapon */
	UPROPERTY(Replicated, VisibleAnywhere, Category="Gold")
	bool bIsHoldingGoldWeapon = false;
		
	/* The current weapon data to use data from */
	UPROPERTY(Replicated, EditDefaultsOnly, meta = (AllowPrivateAccess))
	class USeedData* CurrentWeaponSeedData;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* WeaponMesh;

protected:
	/* Invincible timer - how long the player can't be hit after being hit*/
	UPROPERTY(EditDefaultsOnly)
	float InvincibilityTimer = 0.0f;
	UPROPERTY(EditDefaultsOnly)
	float InvincibilityDuration = 0.5f;
	
	/* Attack timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float AttackTimerTime = 1.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float AttackTimer{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float InstantAttackDelay = 0.2f;

	/* Attack */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float InstantAttackThreshold = 1.0f;
	UPROPERTY(meta = (AllowPrivateAccess))
	bool bCanAttack = true;


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///														Sprint													 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UFUNCTION()
	void SetMaxWalkSpeed(float _Speed);
	UFUNCTION(Server, Reliable)
	void Server_SetMaxWalkSpeed(float _Speed);

	UFUNCTION(Server, Reliable)
	void Server_SetSprintState(bool _NewSprintAnimationState);
	
	/* Public access to update speed */
	UFUNCTION()
	void RefreshCurrentMaxSpeed();

	UFUNCTION(Client, Reliable)
	void Client_RefreshCurrentMaxSpeed();
	
	/* Sprint */
	UPROPERTY(VisibleAnywhere)
	bool bSprinting{false};

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bSprintAnimationState = false;
	
	UPROPERTY()
	float CurrentMaxWalkSpeed = 500.0f; // hard code to walk speed for starting.
	
protected:	
	void UpdateCanSprintUI();
	
	/* Client cansprint */
	UFUNCTION()
	void SetCanSprint(bool _bCanSprint);

	/* Player regular walk speed */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	float WalkSpeed = 500.f;

	/* Speed for when player is holding gold plant */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	float GoldPlantSpeed = 300.0f;

	/* Speed for when player is sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintSpeed = 1000.0f;
	/* How long the player sprints for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTime = 2.0f;
	/* Timer for handling sprint */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float SprintTimer{};

	/* Delay before sprint regens*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float DelayedSprintRegenTimer{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float DelayedSprintRegenTotalDuration = 0.5f;
	
	/* Allows telling the player HUD one time that sprint is ready*/
	UPROPERTY()
	bool bHasNotifiedCanSprint = false;
	UPROPERTY()
	bool bCanSprint = true;
	
	/* Rate scales for adjusting animation speed */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = RateScale)
	float WalkRateScale = 1.5f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = RateScale)
	float GoldSlowRateScale = 0.7f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess), Category = RateScale)
	float SprintRateScaleScalar = 1.5f;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///														Delegates												 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/* Delegate for telling the animation blueprint when charge is cancelled*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFallOnButt);
	UPROPERTY(BlueprintAssignable)
	FOnFallOnButt OnFallOnButtDelegate;
	
	/* Delegate for telling the animation blueprint when charge is cancelled*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeCancelled);
	UPROPERTY(BlueprintAssignable)
	FOnChargeCancelled OnChargeCancelledDelegate;
	
	/* Delegate for telling the animation blueprint when charge has started*/
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChargeStateChanged, bool, IsCharging);
	UPROPERTY(BlueprintAssignable)
	FOnChargeStateChanged OnChargeStateChangedDelegate;
	
	/* Delegate for telling the Player Hud widget when to emphasize UI */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecuteAttack);
	UPROPERTY(BlueprintAssignable)
	FOnExecuteAttack OnExecuteAttackDelegate;
	
	/* Delegate for telling the Player Hud widget when player tried to sprint but it wasn't available */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailedToSprint);
	UPROPERTY(BlueprintAssignable)
	FOnFailedToSprint OnFailedToSprintDelegate;
	
	/* Delegate for telling the Player Hud widget when player tried to sprint but it wasn't available */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprintReady);
	UPROPERTY(BlueprintAssignable)
	FOnSprintReady OnSprintReadyDelegate;
	
	/* Delegate for telling the Player Hud widget when player picks an item up */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickUpItem);
	UPROPERTY(BlueprintAssignable)
	FOnPickUpItem OnPickUpItemDelegate;
	
	/* Delegate for telling the Player Hud widget when player picks a weapon up */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickupWeapon);
	UPROPERTY(BlueprintAssignable)
	FOnPickupWeapon OnPickUpWeaponDelegate;
			
	/* Delegate for telling HUD when this player started claiming a plot */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClaimingPlot, float, CurrentClaimTime, float, TotalClaimTime);
	UPROPERTY(BlueprintAssignable)
	FOnClaimingPlot OnClaimingPlotDelegate;

	/* Delegate for telling HUD when this player failed claiming a plot */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailedClaimingPlot);
	UPROPERTY(BlueprintAssignable)
	FOnFailedClaimingPlot OnStoppedClaimingPlotDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOverCharge, float, OverChargeAmount, float, TotalOverChargeDuration);
	UPROPERTY(BlueprintAssignable)
	FOnOverCharge OnOverChargeDelegate;

	/* Delegate for telling HUD when this player has claimed a plot */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlotClaimed, ARadialPlot*, Plot);
	UPROPERTY(BlueprintAssignable)
	FOnPlotClaimed OnPlotClaimedDelegate;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///														Getters 												 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
public:
	UFUNCTION(BlueprintPure)
	bool GetIsPunching ();

	UFUNCTION(BlueprintPure)
	bool GetIsWeaponGold();
	
	/* Getter for when character is sprinting */
	UFUNCTION(BlueprintCallable)
	bool IsSprinting();
	
	UFUNCTION(BlueprintCallable)
	class USeedData* GetWeaponData() const { return CurrentWeaponSeedData; }

	/* PlayerHUD getter */
	class UWidget_PlayerHUD* GetPlayerHUD();

	/* Getters for walkspeed and ratescale for debuff component */
	float GetWalkSpeed(){ return WalkSpeed; }
	float GetWalkRateScale() { return WalkRateScale; }

	class UCameraComponent* ReturnFollowCamera();
	
	UFUNCTION()
	bool HasIdealRole();

	bool GetHasCrown();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///														VFX		 												 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/* Blueprint event for VFX when player attacks */
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerAttackVFX(FVector _Position, float _Radius, float _Charge);

	/* Stretches the weapon mesh on attack */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StretchWeapon();

	/* Squashes the weapon mesh on attack */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SquashWeapon();

	/* Stencil - on/off */
	void EnableStencil(bool _bIsOn);

	/* Decal Arrow */
	void UpdateDecalDirection(bool _bIsOn);
	void UpdateDecalDirection(bool _bIsOn, bool _bIsTargetingShippingBin);

	/* Activating and Deactivating particle systems */
	void ActivateParticleSystemFromEnum(EParticleSystems _NewSystem);
	void DeActivateParticleSystemFromEnum(EParticleSystems _NewSystem);

	UFUNCTION()
	void OnRep_UpdateAOE();
	
	/* Toggling particle effects */
	UFUNCTION(Server, Unreliable)
	void Server_ToggleParticleSystems(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ToggleParticleSystems(const TArray<EParticleSystems>& _On, const TArray<EParticleSystems>& _Off);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UVFXComponent* VFXComponent;
	
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
	UPROPERTY(ReplicatedUsing=OnRep_UpdateAOE, EditAnywhere, Category = VFX) 
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
	TArray<EParticleSystems> ParticleSystemsToActivate{};
	UPROPERTY(VisibleAnywhere)
	TArray<EParticleSystems> ParticleSystemsToDeActivate{};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///														SFX		 												 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/* Plays audio */
	void PlaySoundAtLocation(FVector _Location, USoundCue* _SoundToPlay, USoundAttenuation* _Attenuation = nullptr);
	UFUNCTION(Server, Unreliable)
	void Server_PlaySoundAtLocation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_PlaySoundAtLocation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation);

	void PlayWeaponSound(USoundCue* _SoundToPlay);
	UFUNCTION(Server, Reliable)
	void Server_PlayWeaponSound(USoundCue* _SoundToPlay);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayWeaponSound(USoundCue* _SoundToPlay);
	
	/* Audio */
	UPROPERTY(EditAnywhere)
	USoundAttenuation* SoundAttenuationSettings;
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

	/* One audio component for charge/attack/get hit sounds*/
	UPROPERTY(EditDefaultsOnly)
	UAudioComponent* WeaponAudioComponent;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///													Animation 													 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/* Playing montages */
	void PlayNetworkMontage(UAnimMontage* _Montage);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimationData* AnimationData;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	EWeaponAnimation CurrentWeaponAnimation{};
	
protected:	
	/* Playing animation montages */
	UFUNCTION(Server, Reliable)
	void Server_PlayNetworkMontage(UAnimMontage* _Montage);
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayNetworkMontage(UAnimMontage* _Montage);

	/* When player has landed after falling */
	void LandAfterfalling();
	
	UPROPERTY(Replicated)
	float FallTimer;
	UPROPERTY(EditDefaultsOnly)
	float FallTimerThreshold = 2.0f;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///													Start/End Game												 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void TeleportToEndGame(FTransform _EndGameTransform);
	
	/* Player teleporter for start and end of game */
	void TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation = FRotator::ZeroRotator);
	
	/* Teleporting at start and end of game */
	UFUNCTION(Server, Unreliable)
	void Server_TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation);
	
protected:
	UFUNCTION(Server, Unreliable)
	void Server_TeleportToEndGame(FTransform _EndGameTransform);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_TeleportToEndGame(FTransform _EndGameTransform);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///													Input Mapping												 ///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	/* Input Actions*/
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShowEmoteAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DisableEmoteAction;
	/* Attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChargeAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReleaseAttackAction;
	/* PickUp/Plant/Sell */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* HoldInteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReleaseHoldInteractAction;
	/* Sprint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EndSprintAction;


	//		\/\/\/\/\/\/\/\/	todo: Not used? can delete?		\/\/\/\/\/\/\/\/\/\/
protected: 
	/* Receiving materials for the farmer costume */
	UFUNCTION(Server, Unreliable)
	void Server_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& _InMaterialsArray);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_ReceiveMaterialsArray(const TArray<UMaterialInstance*>& _InMaterialsArray);
	UFUNCTION(NetMulticast, Unreliable)
	void Multi_SyncCharacterColour();
	/* Friction for when player is on the ice */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float IceFriction{0.1f};
	
};


