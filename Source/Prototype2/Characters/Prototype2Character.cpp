.

#include "Prototype2Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Prototype2/Gameplay/GrowSpot.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/Pickups/PickUpItem.h"
#include "Prototype2/Gamemodes/Prototype2GameMode.h"
#include "Prototype2/Controllers/Prototype2PlayerController.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/Pickups/Weapon.h"
#include "Components/SphereComponent.h"
#include "DynamicMesh/ColliderMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prototype2/Widgets/Widget_InteractionPanel.h"
#include "Prototype2/Widgets/Widget_IngameMenu.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Net/UnrealNetwork.h"
#include "Prototype2/Widgets/Widget_PlayerHUD.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/Image.h"
#include "Components/WidgetComponent.h"
#include "Prototype2/Pickups/GrowableWeapon.h"
#include "Prototype2/Gameplay/SellBin_Winter.h"
#include "Prototype2/Gameplay/Endgame/EndGameCamera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "Prototype2/Pickups/Weapons/WeaponLeekSword.h"
#include "Prototype2/Pickups/Weapons/WeaponPunching.h"
#include "Prototype2/VFX/SquashAndStretch.h"
#include "Prototype2/Widgets/Widget_PlayerName.h"

APrototype2Character::APrototype2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Set collisions	
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Weapon
	Weapon = CreateDefaultSubobject<UWeaponPunching>(TEXT("Weapon"));
	
	// Weapon Mesh
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// Area of attack indicator mesh set up
	AttackAreaIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackAreaIndicatorMesh"));
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	AttackAreaIndicatorMesh->SetHiddenInGame(true);
	
	ChargeAttackAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ChargeAttackAudioComponent"));
	ChargeAttackAudioComponent->SetIsReplicated(true);
	ChargeAttackAudioComponent->SetupAttachment(RootComponent);

	// Niagara Components
	Dizzy_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Dizzy Component"));
	Dizzy_NiagaraComponent->SetupAttachment(GetMesh(), FName("Base-HumanHead"));
	WalkPoof_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WalkPoof Component"));
	WalkPoof_NiagaraComponent->SetupAttachment(RootComponent);
	SprintPoof_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SprintPoof Component"));
	SprintPoof_NiagaraComponent->SetupAttachment(RootComponent);
	Sweat_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Sweat Component"));
	Sweat_NiagaraComponent->SetupAttachment(RootComponent);
	AttackTrail_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AttackTrail Component"));
	AttackTrail_NiagaraComponent->SetupAttachment(WeaponMesh);
	Attack_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Attack Component"));
	Attack_NiagaraComponent->SetupAttachment(RootComponent);
	
	// Decal component
	DecalArmSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DecalArrowArm"));
	DecalArmSceneComponent->SetupAttachment(RootComponent);
	DecalArmSceneComponent->SetIsReplicated(false);
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalArrow"));
	DecalComponent->AttachToComponent(DecalArmSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DecalComponent->SetIsReplicated(false);

	/* Display name widget (above head) */
	PlayerNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameWidgetComponent"));
	PlayerNameWidgetComponent->SetupAttachment(RootComponent);
	PlayerNameWidgetComponent->SetIsReplicated(false);
	PlayerNameWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APrototype2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2Character, Weapon);
	DOREPLIFETIME(APrototype2Character, HeldItem);
	DOREPLIFETIME(APrototype2Character, PlayerMat);
	DOREPLIFETIME(APrototype2Character, PlayerMesh);
	DOREPLIFETIME(APrototype2Character, bIsChargingAttack);
	DOREPLIFETIME(APrototype2Character, AttackChargeAmount);
	DOREPLIFETIME(APrototype2Character, bIsStunned);
	DOREPLIFETIME(APrototype2Character, StunTimer);
	DOREPLIFETIME(APrototype2Character, LocationWhenStunned);
	DOREPLIFETIME(APrototype2Character, CanSprintTimer);
	DOREPLIFETIME(APrototype2Character, SprintTimer);
	DOREPLIFETIME(APrototype2Character, WeaponCurrentDurability);
	DOREPLIFETIME(APrototype2Character, SoundAttenuationSettings);
	DOREPLIFETIME(APrototype2Character, ChargeAttackAudioComponent);
	DOREPLIFETIME(APrototype2Character, bIsHoldingGold);
	DOREPLIFETIME(APrototype2Character, AttackTimer);
	DOREPLIFETIME(APrototype2Character, InteractTimer);
	
	// Niagara Components
	DOREPLIFETIME(APrototype2Character, Dizzy_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, WalkPoof_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, SprintPoof_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, Sweat_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, AttackTrail_NiagaraComponent);
	DOREPLIFETIME(APrototype2Character, Attack_NiagaraComponent);
}

void APrototype2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (PlayerNameWidgetComponent)
	{
		if (auto Widget = PlayerNameWidgetComponent->GetWidget())
		{
			if (auto NameWidget = Cast<UWidget_PlayerName>(Widget))
			{
				PlayerNameWidget = NameWidget;
			}
		}
		
		/* Turn off local players display name */
		if (IsLocallyControlled())
		{
			PlayerNameWidgetComponent->SetVisibility(false);
		}
	}
	
	ChargeAttackAudioComponent->SetSound(ChargeCue);
	ChargeAttackAudioComponent->SetIsReplicated(true);
	ChargeAttackAudioComponent->SetVolumeMultiplier(1.0f);
	ChargeAttackAudioComponent->AttenuationSettings = SoundAttenuationSettings;

	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("WeaponHolsterSocket"));
	AttackAreaIndicatorMesh->SetStaticMesh(DefaultWeaponData->AOEIndicatorMesh);
	
	if (PlayerHudPrefab && !PlayerHUDRef && (GetLocalRole() == ROLE_AutonomousProxy || HasAuthority()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player HUD Created"));
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(Cast<APlayerController>(Controller), PlayerHudPrefab);

		if (PlayerHUDRef)
			PlayerHUDRef->AddToViewport();
	}

	// Clamp the viewing angle of the camera
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -100.0f; // lowest point looking up
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 100.0f; // Highest point looking down

	// Set start position - for decal arrow
	StartPosition = GetActorLocation();

	UpdateDecalDirection(false);

	// Find and store sell bin
	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASellBin::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			SellBin = Cast<ASellBin>(FoundActors[0]);
			UE_LOG(LogTemp, Warning, TEXT("Found shipping bin and allocated"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No shipping bin found"));
		}
	}

	DecalArmSceneComponent->SetIsReplicated(false);
	DecalComponent->SetIsReplicated(false);
	DecalComponent->SetVisibility(false);
	DecalArmSceneComponent->SetVisibility(false);

	// Assign Current Weapon Data to the default which is always punching
	CurrentWeaponData = DefaultWeaponData;
	if (PlayerHUDRef)
		PlayerHUDRef->WeaponImage->SetBrushFromTexture(CurrentWeaponData->WeaponIcon);

	for(auto Material : PlayerMaterials)
	{
		PlayerMaterialsDynamic.Add(UMaterialInstanceDynamic::Create(Material,this));
	}
}

void APrototype2Character::Tick(float _DeltaSeconds)
{
	Super::Tick(_DeltaSeconds);

	if (!PlayerStateRef)
	{
		PlayerStateRef = GetPlayerState<APrototype2PlayerState>();
	}
	if (IsValid(PlayerNameWidget) && IsValid(PlayerStateRef))
	{
		PlayerNameWidget->SetPlayerRef(PlayerStateRef);
	}

	// Clear interact text
	if (PlayerHUDRef)
	{
		PlayerHUDRef->SetHUDInteractText("");
	}

	auto GamestateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GamestateCast->HasGameStarted() && IsLocallyControlled())
	{
		StartPosition = GetActorLocation();
	}

	SyncCharacterColourWithPlayerState();
	

	// Check for ice
	CheckForFloorSurface();
	
	// Sprint
	if (bIsHoldingGold)
	{
		UpdateCharacterSpeed(GoldPlantSpeed, WalkSpeed, GoldSlowRateScale);
	}
	else
	{
		UpdateCharacterSpeed(WalkSpeed, SprintSpeed, WalkRateScale);
	}
	
	// When charging up attack increment charge amount and update AOE indicator
	if (bIsChargingAttack)
	{
		AttackChargeAmount += _DeltaSeconds;
		
		// Cap attack charge
		if (AttackChargeAmount > MaxAttackCharge)
		{
			AttackChargeAmount = MaxAttackCharge;
		}
		Weapon->UpdateAOEIndicator(this);
	}
	else
	{
		AttackAreaIndicatorMesh->SetHiddenInGame(true);
	}

	if (InteractTimer < 0.0f)
	{
		// Check if anything is around to be interacted with
		CheckForInteractables();
	}
	
	if (PlayerHUDRef)
	{
		if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
			Server_CountdownTimers(_DeltaSeconds);
		
		// Update sprint UI
		PlayerHUDRef->SetPlayerSprintTimer(CanSprintTimer);
	}
	
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
	{
		UpdateDecalAngle();

		// Walk poof particle activation/deactivation
		if (GetCharacterMovement()->Velocity.Size() < 50.0f)
		{
			DeActivateParticleSystemFromEnum(EParticleSystem::WalkPoof);
		}
		else
		{
			ActivateParticleSystemFromEnum(EParticleSystem::WalkPoof);
		}

		// Todo: Convert to flag system as to not resize arrays constantly maybe?
		// Turn particles on and off at the end of each frame
		Server_ToggleParticleSystems(ParticleSystemsToActivate, ParticleSystemsToDeActivate);
		ParticleSystemsToActivate.Empty();
		ParticleSystemsToDeActivate.Empty();
	}

	/* Update billboarding for display names */
	UpdatePlayerNames();
}

void APrototype2Character::Server_CountdownTimers_Implementation(float _DeltaSeconds)
{
	if (InteractTimer >= 0)
		InteractTimer -= _DeltaSeconds;
	if (AttackTimer >= 0)
		AttackTimer -= _DeltaSeconds;
	if (SprintTimer >= 0)
		SprintTimer -= _DeltaSeconds;
	if (CanSprintTimer >= 0)
		CanSprintTimer -= _DeltaSeconds;
}

void APrototype2Character::ChargeAttack()
{
	UpdateDecalDirection(false);
	Server_ChargeAttack();
}

void APrototype2Character::ReleaseAttack()
{
	Server_ReleaseAttack();
}

void APrototype2Character::ExecuteAttack(float _AttackSphereRadius)
{
	Weapon->ExecuteAttack(_AttackSphereRadius, this);
	
	// // Get a vector infront of the character for the attack sphere to spawn at
	// const FVector InFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * _AttackSphereRadius) + (GetActorForwardVector() * CurrentWeaponData->WeaponReach);
	//
	// // create a collision sphere
	// const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(_AttackSphereRadius);
	//
	// // Trigger the VFX in blueprint
	// const FVector DownVector = {InFrontOfPlayer.X, InFrontOfPlayer.Y, GetMesh()->GetComponentLocation().Z};
	// TriggerAttackVFX(DownVector, _AttackSphereRadius, AttackChargeAmount);	
	//
	// // create tarray for catching hit results
	// TArray<FHitResult> OutHits;
	//
	// // Set the sweep to stationary
	// const FVector SweepStart = InFrontOfPlayer;
	// const FVector SweepEnd = InFrontOfPlayer;
	//
	// // check if something got hit in the sweep
	// const bool bHasHitResult = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_Pawn, CollisionSphere);
	//
	// // For holding if anyone was hit to degrade weapon later
	// bool bIsOtherPlayerHit = false;
	//
	// if (bHasHitResult)
	// {
	// 	// Check if the hits were players or sell bin
	// 	for (auto& HitResult : OutHits)
	// 	{
	// 		if (auto* HitPlayerCast = Cast<APrototype2Character>(HitResult.GetActor()))
	// 		{
	// 			if (HitPlayerCast != this)
	// 			{
	// 				HitPlayerCast->GetHit(AttackChargeAmount, GetActorLocation(), CurrentWeaponData);
	//
	// 				bIsOtherPlayerHit = true;
	// 			}
	// 		}
	// 		else if (auto* HitSellBinCast = Cast<ASellBin_Winter>(HitResult.GetActor()))
	// 		{
	// 			HitSellBinCast->GetHit(AttackChargeAmount, MaxAttackCharge, GetActorLocation());
	// 		}
	// 	}
	// }
	//
	// // Lower weapon durability
	// if (bIsOtherPlayerHit)
	// {
	// 	WeaponCurrentDurability--;
	// 	PlayerHUDRef->SetWeaponDurability(WeaponCurrentDurability);
	// 	
	// 	if (WeaponCurrentDurability <= 0)
	// 	{
	// 		Multi_DropWeapon();
	//
	// 		//AttackTrail_NiagaraComponent->Deactivate();
	// 		DeActivateParticleSystemFromEnum(EParticleSystem::AttackTrail);
	// 	}
	// }
	//
	// // Todo: is this teh weapon cooldown?
	// // Reset Attack Timer
	// AttackTimer = AttackTimerTime;
	//
	// // Reset Attack variables
	// bIsChargingAttack = false;
	// AttackChargeAmount = 0.0f;
	//
	// PlaySoundAtLocation(GetActorLocation(), ExecuteCue);
	//
	// // Stop the player Interacting while "executing attack"
	// InteractTimer = InteractTimerTime;
	//
	// bCanAttack = true;
	//
	// Server_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));//("WeaponHeldSocket"));
}

void APrototype2Character::Interact()
{
	if(!HeldItem && !Weapon)
	{
		PlayerHUDRef->ClearPickupUI();
	}
	
	if (InteractTimer <= 0.0f)
	{
		if (!bIsChargingAttack)
		{
			TryInteract();
			Server_TryInteract();
		}
	}

	EnableStencil(false);
}

void APrototype2Character::Sprint()
{
	Server_Sprint();
}

void APrototype2Character::UpdateCharacterSpeed(float _WalkSpeed, float _SprintSpeed, float _BaseAnimationRateScale)
{
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
	{
		// If not sprinting
		if (SprintTimer < 0.0f)
		{
			GetCharacterMovement()->MaxWalkSpeed = _WalkSpeed;

			// Speed up animation
			if (AnimationData->Run &&
				AnimationData->RunWithWeapon &&
				AnimationData->Sprint &&
				AnimationData->SprintHoldingItem &&
				AnimationData->SprintWithWeapon)
			{
				//RunAnimation->RateScale = _BaseAnimationRateScale;
				AnimationData->Run->RateScale = _BaseAnimationRateScale;
				AnimationData->RunWithWeapon->RateScale = _BaseAnimationRateScale;
				AnimationData->Sprint->RateScale = _BaseAnimationRateScale;
				AnimationData->SprintHoldingItem->RateScale = _BaseAnimationRateScale;
				AnimationData->SprintWithWeapon->RateScale = _BaseAnimationRateScale;
			}
			
			DeActivateParticleSystemFromEnum(EParticleSystem::SprintPoof);
		}
		else // If Sprinting
		{
			GetCharacterMovement()->MaxWalkSpeed = _SprintSpeed;
			if (AnimationData->Run &&
				AnimationData->RunWithWeapon &&
				AnimationData->Sprint &&
				AnimationData->SprintHoldingItem &&
				AnimationData->SprintWithWeapon)
			{
				//RunAnimation->RateScale = _BaseAnimationRateScale * SprintRateScaleScalar;
				AnimationData->Run->RateScale = _BaseAnimationRateScale * SprintRateScaleScalar;
				AnimationData->RunWithWeapon->RateScale = _BaseAnimationRateScale * SprintRateScaleScalar;
				AnimationData->Sprint->RateScale = _BaseAnimationRateScale * SprintRateScaleScalar;
				AnimationData->SprintHoldingItem->RateScale = _BaseAnimationRateScale * SprintRateScaleScalar;
				AnimationData->SprintWithWeapon->RateScale = _BaseAnimationRateScale * SprintRateScaleScalar;
			}
			
			DeActivateParticleSystemFromEnum(EParticleSystem::WalkPoof);
			ActivateParticleSystemFromEnum(EParticleSystem::Sweat);
			ActivateParticleSystemFromEnum(EParticleSystem::SprintPoof);
		}
	
		if (CanSprintTimer < 0.0f)
		{
			DeActivateParticleSystemFromEnum(EParticleSystem::Sweat);
		}
	}
}

void APrototype2Character::CheckForInteractables()
{
	TArray<FHitResult> OutHits;
	const FVector SweepStart = GetActorLocation();
	const FVector SweepEnd = GetActorLocation();
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(InteractRadius);

	if (GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_Visibility, CollisionSphere))
	{
		TArray<AActor*> InteractableActors;
		for (auto& HitActor : OutHits)
		{
			if (const auto InteractInterface = Cast<IInteractInterface>(HitActor.GetActor()))
			{
				if (GetPlayerState<APrototype2PlayerState>())
				{
					if (InteractInterface->IsInteractable(GetPlayerState<APrototype2PlayerState>()))
						InteractableActors.Add(HitActor.GetActor());
				}
			}
		}

		bool bIsEmptyArray{};
		TArray<AActor*> ImportantActors;
		for(auto InteractableActor : InteractableActors)
		{
			float DistanceToOther = FVector::Distance(InteractableActor->GetActorLocation(), GetActorLocation());
			if (DistanceToOther <= InteractRadius)
			{
				if (Cast<ASellBin>(InteractableActor) || Cast<AGrowSpot>(InteractableActor))
				{
					bIsEmptyArray = true;
					ImportantActors.Add(InteractableActor);
				}
			}
		}
		if (bIsEmptyArray)
		{
			InteractableActors.Empty();
			InteractableActors = ImportantActors;
		}

		float DistanceToClosest;
		const auto NearestActor = UGameplayStatics::FindNearestActor(GetActorLocation(), InteractableActors, DistanceToClosest);
		if (DistanceToClosest <= InteractRadius && NearestActor)
		{
			if (ClosestInteractableActor && ClosestInteractableActor != NearestActor)
			{
				EnableStencil(false);
			}
			ClosestInteractableActor = NearestActor;
			ClosestInteractableItem = Cast<IInteractInterface>(NearestActor);
			return;
		}

		// else
		EnableStencil(false);
		ClosestInteractableItem = nullptr;
		ClosestInteractableActor = nullptr;
	}
	else
	{
		// null both references
		EnableStencil(false);
		ClosestInteractableItem = nullptr;
		ClosestInteractableActor = nullptr;
	}
}

void APrototype2Character::EnableStencil(bool _bIsOn)
{
	if (!ClosestInteractableActor)
	{
		return;
	}
	
	if (auto CastedComponent = ClosestInteractableActor->GetComponentByClass(UItemComponent::StaticClass()))
	{
		if (auto CastedItemComponent = Cast<UItemComponent>(CastedComponent))
		{
			CastedItemComponent->Mesh->SetRenderCustomDepth(_bIsOn);
		}
	}
	
}

void APrototype2Character::GetHit(float _AttackCharge, FVector _AttackerLocation, UWeaponData* _OtherWeaponData)
{
	UpdateDecalDirection(false);
	
	// Knockback
	//FVector KnockAway = GetActorUpVector()/3 + (GetActorLocation() - _AttackerLocation).GetSafeNormal();todo: delete when weapon data done
	FVector KnockAway = (GetActorUpVector() * _OtherWeaponData->KnockUpMultiplier) + (GetActorLocation() - _AttackerLocation).GetSafeNormal();
	
	// Set minimum attack charge for scaling knockback
	if (_AttackCharge < 1.0f)
	{
		_AttackCharge = 1.0f;
	}
	
	KnockAway *= _AttackCharge * _OtherWeaponData->KnockbackMultiplier;//KnockBackAmount; todo: delete when weapon data done
	
	// Limit the knockback to MaxKnockBackVelocity
	if (KnockAway.Size() > _OtherWeaponData->MaxKnockback) //MaxKnockBackVelocity) todo: delete when weapon data done
	{
		KnockAway = KnockAway.GetSafeNormal() * _OtherWeaponData->MaxKnockback; // MaxKnockBackVelocity; todo: delete when weapon data done
	}

	// Knock this player away
	GetCharacterMovement()->Launch(KnockAway);
	
	// Drop item
	if (HeldItem)
	{
		Server_DropItem();
	}

	PlaySoundAtLocation(GetActorLocation(), GetHitCue);

	// VFX
	FVector AttackVFXLocation = _AttackerLocation - GetActorLocation();
	AttackVFXLocation = AttackVFXLocation.GetSafeNormal();
	AttackVFXLocation *= 50.0f;
	AttackVFXLocation += GetActorLocation();
	Attack_NiagaraComponent->SetWorldLocation(AttackVFXLocation);
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
	{
		ActivateParticleSystemFromEnum(EParticleSystem::Attack);
	}
}

void APrototype2Character::Multi_SocketItem_Implementation(UStaticMeshComponent* _Object, FName _Socket)
{
	_Object->SetSimulatePhysics(false);
	_Object->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_Object->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	_Object->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(_Socket));
}

void APrototype2Character::Multi_ToggleChargeSound_Implementation(bool _soundEnabled)
{
	if (_soundEnabled && !ChargeAttackAudioComponent->IsPlaying())
	{
		ChargeAttackAudioComponent->Play();
	}
	else
	{
		ChargeAttackAudioComponent->Stop();		
	}	
}

void APrototype2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APrototype2Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APrototype2Character::Look);

		// UI
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Triggered, this, &APrototype2Character::OpenIngameMenu);

		// Attack
		EnhancedInputComponent->BindAction(ChargeAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ChargeAttack);
		EnhancedInputComponent->BindAction(ReleaseAttackAction, ETriggerEvent::Triggered, this, &APrototype2Character::ReleaseAttack);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APrototype2Character::Interact);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APrototype2Character::Sprint);
	}
}

void APrototype2Character::Move(const FInputActionValue& _Value)
{
	// input is a Vector2D
	FVector2D MovementVector = _Value.Get<FVector2D>();

	if (Controller != nullptr && bAllowMovementFromInput)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}


}

void APrototype2Character::Look(const FInputActionValue& _Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = _Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APrototype2Character::OpenIngameMenu()
{
	if (auto* playercontroller = Cast<APrototype2PlayerController>(Controller))
	{
		if (PlayerHUDRef)
			PlayerHUDRef->EnableDisableMenu();
	}
}

void APrototype2Character::UpdateDecalAngle()
{
	if (SellBin && IsLocallyControlled())
	{
		const FVector CurrentPlayerPosition = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
		FRotator NewRotation{};
	
		if (bDecalTargetShippingBin)
		{
			const FVector SellPosition = FVector(SellBin->GetActorLocation().X, SellBin->GetActorLocation().Y, 0);
		
			NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentPlayerPosition, SellPosition);
		}
		else
		{
			const FVector PlotPosition = FVector(StartPosition.X, StartPosition.Y, 0);
		
			NewRotation = UKismetMathLibrary::FindLookAtRotation(CurrentPlayerPosition, PlotPosition);
		}

		DecalArmSceneComponent->SetWorldRotation(NewRotation);
	}
}

void APrototype2Character::UpdateAOEIndicator()
{
	//AttackAreaIndicatorMesh->SetHiddenInGame(false); // Todo: Remove when individual derived weapon classes implemented
	//
	//float AttackSphereRadius = CurrentWeaponData->BaseAttackRadius + AttackChargeAmount * CurrentWeaponData->AOEMultiplier;	
	//FVector InFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * AttackSphereRadius) + (GetActorForwardVector() * CurrentWeaponData->WeaponReach);
	//
	//FVector DownVector = {InFrontOfPlayer.X, InFrontOfPlayer.Y, GetMesh()->GetComponentLocation().Z};
	//
	//AttackAreaIndicatorMesh->SetWorldLocation(DownVector);
	//AttackAreaIndicatorMesh->SetRelativeScale3D({AttackSphereRadius,AttackSphereRadius,AttackChargeAmount * 30.0f});// Magic number just to increase the height of the aoe indicator

}

void APrototype2Character::UpdatePlayerNames()
{
	const FVector CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector LookAtDirection = CameraLocation - PlayerNameWidgetComponent->GetComponentLocation();
	LookAtDirection.Normalize();

	FRotator LookAtRotation = LookAtDirection.Rotation();
	PlayerNameWidgetComponent->SetWorldRotation(LookAtRotation);
}

void APrototype2Character::SyncCharacterColourWithPlayerState()
{
	if (!PlayerStateRef)
	{
		return;
	}
	if (GetMesh()->GetMaterial(0) != PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character])
	{
		if (AnimationData)
			GetMesh()->SetSkeletalMeshAsset(AnimationData->SkeletalMesh);
		
		PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Cow Colour"), PlayerStateRef->Details.CharacterColour);
		PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Spot Colour"), PlayerStateRef->Details.CharacterSubColour);
	
		GetMesh()->SetMaterial(0, PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]);
	}
}

void APrototype2Character::ActivateParticleSystemFromEnum(EParticleSystem _NewSystem)
{
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
		ParticleSystemsToActivate.Add(_NewSystem);
}

void APrototype2Character::DeActivateParticleSystemFromEnum(EParticleSystem _NewSystem)
{
	if (HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy)
		ParticleSystemsToDeActivate.Add(_NewSystem);
}

void APrototype2Character::PickupItem(APickUpItem* _Item)
{
	if (PlayerHUDRef)
	PlayerHUDRef->SetHUDInteractText("");
	
	// Set the HUD UI pickup icon depending on seed/plant/weapon
	switch (_Item->PickupActor)
	{
	case EPickupActor::PlantActor:
		{
			if (!PlayerHUDRef || !_Item->PlantData)
			{
				break;
			}
			if (_Item->ItemComponent->bGold)
			{
				PlayerHUDRef->UpdatePickupUI(_Item->PlantData->GoldPlantIcon);
			}
			else
			{
				PlayerHUDRef->UpdatePickupUI(_Item->PlantData->PlantIcon);
			}				
			break;
		}
	case EPickupActor::WeaponActor:
		{
			// Set UI
			if (PlayerHUDRef && _Item->WeaponData)
			{
				PlayerHUDRef->WeaponImage->SetBrushFromTexture(_Item->WeaponData->WeaponIcon);
			}
			break;
		}
	case EPickupActor::SeedActor:
		{
			if (PlayerHUDRef)
			{
				if (_Item->DataAssetPickupType == EPickupDataType::WeaponData)
				{
					PlayerHUDRef->UpdatePickupUI(_Item->WeaponData->SeedIcon);
					
				}
				else if (_Item->DataAssetPickupType == EPickupDataType::PlantData)
				{
					PlayerHUDRef->UpdatePickupUI(_Item->PlantData->SeedIcon);
				}
			}
			break;
		}
	default:
		break;
	}
	
	Server_PickupItem(_Item);
}

void APrototype2Character::Server_ToggleParticleSystems_Implementation(const TArray<EParticleSystem>& _On, const TArray<EParticleSystem>& _Off)
{
	Multi_ToggleParticleSystems(_On, _Off);
}

void APrototype2Character::Multi_ToggleParticleSystems_Implementation(const TArray<EParticleSystem>& _On, const TArray<EParticleSystem>& _Off)
{
	for(auto ParticleSystemCast : _On)
	{
		switch (ParticleSystemCast)
		{
		case EParticleSystem::WalkPoof:
			{
				WalkPoof_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystem::SprintPoof:
			{
				SprintPoof_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystem::Sweat:
			{
				Sweat_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystem::AttackTrail:
			{
				AttackTrail_NiagaraComponent->Activate();
				break;
			}
		case EParticleSystem::Attack:
			{
				Attack_NiagaraComponent->Activate(true);
				break;
			}
		case EParticleSystem::Test:
			{
				//Test_NiagraComponent->Activate();
				break;
			}
		default:
			break;
		}
	}

	for(auto ParticleSystemCast : _Off)
	{
		switch (ParticleSystemCast)
		{
		case EParticleSystem::WalkPoof:
			{
				WalkPoof_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystem::SprintPoof:
			{
				SprintPoof_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystem::Sweat:
			{
				Sweat_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystem::AttackTrail:
			{
				AttackTrail_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystem::Attack:
			{
				Attack_NiagaraComponent->Deactivate();
				break;
			}
		case EParticleSystem::Test:
			{
				//Test_NiagraComponent->Deactivate();
				break;
			}
		default:
			break;
		}
	}
}

void APrototype2Character::UpdateDecalDirection(bool _bIsOn)
{
	if (IsLocallyControlled())
	{
		DecalComponent->SetVisibility(_bIsOn);
	}
}

void APrototype2Character::UpdateDecalDirection(bool _bIsOn, bool _bIsTargetingShippingBin)
{
	if (IsLocallyControlled())
	{
		DecalComponent->SetVisibility(_bIsOn);

		bDecalTargetShippingBin = _bIsTargetingShippingBin;
	}
}

void APrototype2Character::TeleportToLocation(FVector _DestinationLocation, FRotator _DestinationRotation)
{
	if (GetLocalRole() == ROLE_Authority || GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_TeleportToLocation(_DestinationLocation, _DestinationRotation);
	}
}

void APrototype2Character::Server_TeleportToLocation_Implementation(FVector _DestinationLocation, FRotator _DestinationRotation)
{
	SetActorLocation(_DestinationLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(_DestinationRotation, ETeleportType::TeleportPhysics);
}

bool APrototype2Character::IsSprinting()
{
	return FMath::RoundToInt(GetMovementComponent()->GetMaxSpeed()) == FMath::RoundToInt(SprintSpeed);
}

void APrototype2Character::CheckForFloorSurface()
{
	//UE_LOG(LogTemp, Warning, TEXT("Ground Check"));
	
	FVector StartLocation = GetActorLocation() + FVector{0,0,100}; // The start location of the line trace
	FVector EndLocation = GetActorLocation() + FVector{0,0,-100}; // The end location of the line trace

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true; // Enable complex tracing for accurate physics material retrieval
	QueryParams.bReturnPhysicalMaterial = true;

	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	GetCharacterMovement()->BrakingFriction = 2.0f;
	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->GroundFriction = 8.0f;
	
	// Perform the line trace
	if (!GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation, ECC_Visibility, QueryParams))
	{
		return;
	}
	
	for(auto& Hit : HitResults)
	{
		if (const UPhysicalMaterial* PhysicalMaterial = Hit.PhysMaterial.Get())
		{
			const float Friction = PhysicalMaterial->Friction;
			
			if (Friction <= 0.5f)
			{
				GetCharacterMovement()->BrakingFriction = 0.0f;
				GetCharacterMovement()->MaxAcceleration = 2048.0f * 0.5f;
				GetCharacterMovement()->GroundFriction = 0.0f;
				break;
			}
		}
	}
}

void APrototype2Character::PlaySoundAtLocation(FVector _Location, USoundCue* _SoundToPlay, USoundAttenuation* _Attenuation)
{
	Server_PlaySoundAtLocation(_Location, _SoundToPlay, _Attenuation );
}

void APrototype2Character::Ragdoll(bool _bShouldRagdoll)
{
	if (_bShouldRagdoll)
	{
		SetReplicateMovement(false);
		
		/* Disable all collision on capsule */
		UCapsuleComponent* CapsuleComponentReference = GetCapsuleComponent();
		CapsuleComponentReference->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetActorEnableCollision(true);

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
		

		UCharacterMovementComponent* CharacterComponentCast = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComponentCast)
		{
			CharacterComponentCast->StopMovementImmediately();
			CharacterComponentCast->DisableMovement();
			CharacterComponentCast->SetComponentTickEnabled(false);
		}
	}
	else
	{
		SetActorEnableCollision(false);
		GetMesh()->SetAllBodiesSimulatePhysics(false);
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->bBlendPhysics = false;
		SetReplicateMovement(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetRelativeTransform(MeshLocationWhenStunned);
		SetActorTransform(LocationWhenStunned);
		
		
		/* Disable all collision on capsule */
		UCapsuleComponent* CapsuleComponentReference = GetCapsuleComponent();
		CapsuleComponentReference->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		UCharacterMovementComponent* CharacterComponentCast = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComponentCast)
		{
			CharacterComponentCast->SetMovementMode(EMovementMode::MOVE_Walking);
			CharacterComponentCast->SetComponentTickEnabled(true);
		}
	}	
}

void APrototype2Character::Server_Ragdoll_Implementation(bool _Ragdoll)
{
	if (_Ragdoll)
	{
		LocationWhenStunned = GetActorTransform();
		MeshLocationWhenStunned = GetMesh()->GetRelativeTransform();
	}

	Multi_Ragdoll(_Ragdoll);
}

void APrototype2Character::Multi_Ragdoll_Implementation(bool _Ragdoll)
{
	Ragdoll(_Ragdoll);
}

UWidget_PlayerHUD* APrototype2Character::GetPlayerHUD()
{
	// Update UI
	return PlayerHUDRef;
}

void APrototype2Character::Multi_TeleportToLocation_Implementation(FVector _DestinationLocation, FRotator _DestinationRotation)
{
	SetActorLocation(_DestinationLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(_DestinationRotation, ETeleportType::TeleportPhysics);
}

void APrototype2Character::PlayNetworkMontage(UAnimMontage* _Montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_Montage);
	Server_PlayNetworkMontage(_Montage);
}

UCameraComponent* APrototype2Character::ReturnFollowCamera()
{
	return FollowCamera;
}

void APrototype2Character::DropWeapon()
{
	// Set UI
	if (PlayerHUDRef && DefaultWeaponData)
	{
		PlayerHUDRef->WeaponImage->SetBrushFromTexture(DefaultWeaponData->WeaponIcon);
	}
	Server_DropWeapon();
}

void APrototype2Character::Multi_SetPlayerAimingMovement_Implementation(bool _bIsAiming)
{
	// Allows rotation while stationary
	GetCharacterMovement()->bUseControllerDesiredRotation = _bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !_bIsAiming;
	bAllowMovementFromInput = !_bIsAiming;
}

void APrototype2Character::ResetAttack()
{
	// Reset Attack Timer
	AttackTimer = AttackTimerTime;

	// Reset Attack variables
	bIsChargingAttack = false;
	AttackChargeAmount = 0.0f;

	// Stop the player Interacting while "executing attack"
	InteractTimer = InteractTimerTime;

	bCanAttack = true;
}

void APrototype2Character::TeleportToEndGame(FTransform _EndGameTransform)
{
	if (HasAuthority())
	{
		Multi_TeleportToEndGame(_EndGameTransform);
	}
	else
	{
		Server_TeleportToEndGame(_EndGameTransform);
	}
}

void APrototype2Character::Server_TeleportToEndGame_Implementation(FTransform _EndGameTransform)
{
	Multi_TeleportToEndGame(_EndGameTransform);
}

void APrototype2Character::Multi_TeleportToEndGame_Implementation(FTransform _EndGameTransform)
{
	PlayerNameWidgetComponent->SetVisibility(true);
	
	GetCharacterMovement()->StopActiveMovement();
	GetCharacterMovement()->StopMovementImmediately();
	
	SetActorLocation(_EndGameTransform.GetLocation(), false, nullptr, ETeleportType::ResetPhysics);
	SetActorRotation(_EndGameTransform.Rotator(), ETeleportType::ResetPhysics);
}

bool APrototype2Character::HasIdealRole()
{
	return HasAuthority() || GetLocalRole() == ROLE_AutonomousProxy;
}

void APrototype2Character::Multi_SyncCharacterColour_Implementation()
{
	auto SkinColour = PlayerStateRef->Details.CharacterColour;
	auto FeaturesColour = PlayerStateRef->Details.CharacterColour / 1.5f;
	PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Cow Colour"), SkinColour);
	PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]->SetVectorParameterValue(FName("Spot Colour"), FeaturesColour);
	
	GetMesh()->SetMaterial(0, PlayerMaterialsDynamic[(int32)PlayerStateRef->Details.Character]);
}

void APrototype2Character::Server_PlayNetworkMontage_Implementation(UAnimMontage* _Montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_Montage);
	Multi_PlayNetworkMontage(_Montage);
}

void APrototype2Character::Multi_PlayNetworkMontage_Implementation(UAnimMontage* _Montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_Montage);
}

void APrototype2Character::Server_SetPlayerColour_Implementation()
{
	Multi_SetPlayerColour();
}

void APrototype2Character::Server_Sprint_Implementation()
{
	if (CanSprintTimer < 0.0f && !bIsChargingAttack)
	{
		SprintTimer = SprintTime;
		CanSprintTimer = CanSprintTime;
	}
}

void APrototype2Character::SocketWeapon(FName _Socket)
{
	Server_SocketItem(WeaponMesh, _Socket);
}

void APrototype2Character::Server_AddHUD_Implementation()
{
	Client_AddHUD();
}

void APrototype2Character::Multi_Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef && PlayerStateRef)
    {
    	PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), PlayerStateRef->Player_ID), PlayerHudPrefab);
   
    	if (PlayerHUDRef)
    		PlayerHUDRef->AddToViewport();
    }
}

void APrototype2Character::Server_ChargeAttack_Implementation()
{
	if (AttackTimer < 0.0f)
	{
		if (HeldItem)
		{
			Server_DropItem();
		}
		
		bIsChargingAttack = true;

		if (Weapon)
		{
			Server_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));//FName("WeaponAttackingSocket"));
		}
		Weapon->ChargeAttack(this);
	}
}

void APrototype2Character::Server_ReleaseAttack_Implementation()
{
	// Create a sphere collider, check if player hit, call player hit	
	if (!bIsChargingAttack || !bCanAttack)
	{
		return;
	}
	
	bCanAttack = false;
	
	// Socket Weapon back to held
	Server_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));//FName("WeaponHeldSocket"));
	
	// Set the radius of the sphere for attack
	int32 AttackSphereRadius = CurrentWeaponData->BaseAttackRadius + AttackChargeAmount * CurrentWeaponData->AOEMultiplier;
	if (CurrentWeaponData != DefaultWeaponData)
	{
		// VFX
		ActivateParticleSystemFromEnum(EParticleSystem::AttackTrail);
	}
	
	// If attack button is clicked without being held
	if (AttackChargeAmount < MaxAttackCharge)//InstantAttackThreshold)
	{
		Weapon->ReleaseAttack(false, this);
		
		// Delayed attack
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this, AttackSphereRadius] { ExecuteAttack(AttackSphereRadius); }), InstantAttackDelay, false);	
	}
	else
	{
		Weapon->ReleaseAttack(true, this);
		Weapon->ExecuteAttack(AttackSphereRadius,this);
	}
}

void APrototype2Character::Multi_ReleaseAttack_Implementation()
{
}

void APrototype2Character::Server_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation)
{
	Multi_PlaySoundAtLocation(_Location, _SoundQueue, _Attenuation);
}

void APrototype2Character::Multi_PlaySoundAtLocation_Implementation(FVector _Location, USoundCue* _SoundQueue, USoundAttenuation* _Attenuation)
{
	if (_Attenuation)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _SoundQueue, _Location, 1, 1, 0, _Attenuation);
	else
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _SoundQueue, _Location, 1, 1, 0, SoundAttenuationSettings);
}

void APrototype2Character::Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef)
	{
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(Cast<APrototype2PlayerController>(Controller), PlayerHudPrefab);

		PlayerHUDRef->AddToViewport();
	}
}

void APrototype2Character::Multi_SetPlayerColour_Implementation()
{
	// No longer used?
	/*if (auto* GamemodeCast = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (auto* GamestateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			for(auto PlayerStateCast : GamestateCast->PlayerArray)
			{
				if (auto* CharacterCast = Cast<APrototype2Character>(PlayerStateCast->GetPlayerController()->GetCharacter()))
				{
					//character->GetMesh()->SetMaterial(0, gamemode->PlayerMaterials[Cast<APrototype2PlayerState>(player)->Player_ID]);
				}
			}
		}
	}*/
}

void APrototype2Character::TryInteract()
{
	if (ClosestInteractableItem)
	{
		ClosestInteractableItem->ClientInteract(this);
	}
	
	if (ClosestInteractableItem == nullptr)
	{
		UpdateDecalDirection(false);
	}
}

void APrototype2Character::Server_TryInteract_Implementation()
{
	if (ClosestInteractableActor)
	{
		if (auto SomeComponent = ClosestInteractableActor->GetComponentByClass(USquashAndStretch::StaticClass()))
		{
			if (auto SSComponent = Cast<USquashAndStretch>(SomeComponent))
			{
				SSComponent->Boing();
			}
		}
	}
	
	if (ClosestInteractableItem)
	{
		InteractTimer = InteractTimerTime;

		UE_LOG(LogTemp, Warning, TEXT("Attempted to Interact!"));
		ClosestInteractableItem->Interact(this);

		EnableStencil(false);
		
		if (HeldItem)
		{
			if (AnimationData->Pickup)
			{
				PlayNetworkMontage(AnimationData->Pickup);
			}
			if (Weapon)
			{
				Multi_SocketItem(WeaponMesh, FName("WeaponHolsterSocket"));
			}
		}
	}
	else if (HeldItem && !ClosestInteractableItem)
	{
		//UpdateDecalDirection(false); // Turn off decal as dropped any item
		InteractTimer = InteractTimerTime;
		Server_DropItem();
	}
}

void APrototype2Character::Server_DropItem_Implementation()
{
	HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
		
	// Set HUD image
	if (PlayerHUDRef)
	{
		PlayerHUDRef->ClearPickupUI();
		PlayerHUDRef->SetHUDInteractText("");
	}
	
	if (Weapon)
	{
		Multi_SocketItem(WeaponMesh, FName("Base-HumanWeapon"));
	}
	
	Multi_DropItem();
}

void APrototype2Character::Multi_DropItem_Implementation()
{
	// Drop into world
	if(HeldItem)
	{
		// If Item was gold, set bool to return character to normal speed in tick
		if (HeldItem->ItemComponent->bGold)
		{
			bIsHoldingGold = false;
		}
		
		HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		HeldItem->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
		// So that CheckForInteractables() can see it again
		HeldItem->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

		if (auto SSComponent = HeldItem->SSComponent)
		{
			SSComponent->Boing();
		}
		
		//// Launch the HeldItem towards the player instead of droppping
		//FVector LaunchItemVelocity = GetVelocity();
		//LaunchItemVelocity = LaunchItemVelocity.GetSafeNormal();
		//LaunchItemVelocity *= ItemLaunchStrength;
		//HeldItem->ItemComponent->Mesh->AddForce(LaunchItemVelocity);
	}
	
	HeldItem = nullptr;

	PlaySoundAtLocation(GetActorLocation(), DropCue);
}

void APrototype2Character::Server_PickupItem_Implementation(APickUpItem* _Item)
{
	if (HeldItem)
	{
		Server_DropItem();
	}

	Multi_PickupItem(_Item);
}

void APrototype2Character::Server_SocketItem_Implementation(UStaticMeshComponent* _Object, FName _Socket)
{
	Multi_SocketItem(_Object, _Socket);
}

void APrototype2Character::Server_DropWeapon_Implementation()
{
	Multi_DropWeapon();
}

void APrototype2Character::Multi_PickupItem_Implementation(APickUpItem* _Item)
{
	if (!_Item)
	{
		return;
	}
	
	// Audio
	PlaySoundAtLocation(GetActorLocation(), PickUpCue);

	// Setup picked up items mesh physics and collision response
	_Item->ItemComponent->Mesh->SetSimulatePhysics(false);
	_Item->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	// So that CheckForInteractables() cant see it while player is holding it
	_Item->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// Attach plant/seed ot the HeldItemSocket, and change weapon mesh to the one passed in
	switch (_Item->PickupActor)
	{
	case EPickupActor::PlantActor:
		{
			_Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeldItemSocket"));
			HeldItem = _Item;	
			if (HeldItem->ItemComponent->bGold)
			{
				bIsHoldingGold = true;
			}
			break;
		}
	case EPickupActor::WeaponActor:
		{
			Weapon->DestroyComponent();
			if (UWeapon* NewWeapon = NewObject<UWeapon>(this, _Item->WeaponData->WeaponComponent))
			{
				NewWeapon->RegisterComponent();
				NewWeapon->CreationMethod = EComponentCreationMethod::Instance;
					
				Weapon = NewWeapon;
			}
			
			// Set the new weapons mesh to our WeaponMesh
			WeaponMesh->SetStaticMesh(_Item->WeaponData->WeaponMesh);
			
			CurrentWeaponData = _Item->WeaponData;
			WeaponCurrentDurability = CurrentWeaponData->Durability;
			Multi_SocketItem_Implementation(WeaponMesh, FName("Base-HumanWeapon"));
			AttackAreaIndicatorMesh->SetStaticMesh(_Item->WeaponData->AOEIndicatorMesh);

			CurrentWeaponAnimation = _Item->WeaponData->WeaponAnimationType;
			break;
		}
	case EPickupActor::SeedActor:
		{
			_Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeldItemSocket"));
			HeldItem = _Item;
			break;
		}
	case EPickupActor::FertilizerActor:
		{
			_Item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeldItemSocket"));
			HeldItem = _Item;
			break;
		}
	default:
		break;
	}
}

void APrototype2Character::Multi_DropWeapon_Implementation()
{
	CurrentWeaponData = DefaultWeaponData;
	Weapon->DestroyComponent();
	if (UWeapon* NewWeapon = NewObject<UWeapon>(this, CurrentWeaponData->WeaponComponent))
	{
		NewWeapon->RegisterComponent();
		NewWeapon->CreationMethod = EComponentCreationMethod::Instance;
					
		Weapon = NewWeapon;
	}
	WeaponMesh->SetStaticMesh(DefaultWeaponData->WeaponMesh);
	AttackAreaIndicatorMesh->SetStaticMesh(CurrentWeaponData->AOEIndicatorMesh);
	CurrentWeaponAnimation = CurrentWeaponData->WeaponAnimationType;
}

void APrototype2Character::Server_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& _InMaterialsArray)
{
	Multi_ReceiveMaterialsArray(_InMaterialsArray);
}

void APrototype2Character::Multi_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& _InMaterialsArray)
{
	USkeletalMeshComponent* SkellyMeshComponent = GetMesh();
	if (SkellyMeshComponent && _InMaterialsArray.Num() > 0)
	{
		for (int32 i = 0; i < _InMaterialsArray.Num(); i++)
		{
			SkellyMeshComponent->SetMaterial(i, _InMaterialsArray[i]);
		}
	}
}

void APrototype2Character::Server_FireParticleSystem_Implementation(UNiagaraSystem* _NiagaraSystem, FVector _Position)
{
	//Multi_FireParticleSystem(_NiagaraSystem, _Position);
}

void APrototype2Character::Multi_FireParticleSystem_Implementation(UNiagaraSystem* _NiagaraSystem, FVector _Position)
{
	// This crashes
	//// Spawn a one-shot emitter at the passed in location
	//UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), _NiagaraSystem, _Position);
	//NiagaraComponent->SetIsReplicated(true);
	//// Set the NiagaraComponent to auto-destroy itself after it finishes playing
	//NiagaraComponent->SetAutoDestroy(true);
	
	//DizzyComponent->SetAsset(DizzySystem);
    //DizzyComponent->Activate();
    //DizzyComponent->SetAutoDestroy(false);
}

void APrototype2Character::Server_SetParticleActive_Implementation(UNiagaraComponent* _NiagaraComponent, bool _bIsActive)
{
	Multi_SetParticleActive_Implementation(_NiagaraComponent, _bIsActive);
}

void APrototype2Character::Multi_SetParticleActive_Implementation(UNiagaraComponent* _NiagaraComponent, bool _bIsActive)
{
	if (_NiagaraComponent)
	{
		if (_bIsActive)
		{
			_NiagaraComponent->Activate();
		}
		else
		{
			_NiagaraComponent->Deactivate();
		}
	}
}

void APrototype2Character::Server_ToggleChargeSound_Implementation(bool _bIsSoundEnabled)
{
	Multi_ToggleChargeSound(_bIsSoundEnabled);
}
