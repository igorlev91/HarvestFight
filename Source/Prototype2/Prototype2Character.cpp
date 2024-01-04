// Copyright Epic Games, Inc. All Rights Reserved.

#include "Prototype2Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GrowSpot.h"
#include "InteractInterface.h"
#include "PickUpItem.h"
#include "Prototype2GameMode.h"
#include "Prototype2PlayerController.h"
#include "Prototype2PlayerState.h"
#include "PrototypeGameInstance.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "DynamicMesh/ColliderMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetGuidLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Widget_InteractionPanel.h"
#include "Widgets/Widget_IngameMenu.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "Gamestates/Prototype2Gamestate.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Widget_PlayerHUD.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GrowableWeapon.h"
#include "SellBin_Winter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


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
	Weapon = CreateDefaultSubobject<UWeapon>(TEXT("Weapon"));
	Weapon->Mesh->SetSimulatePhysics(false);
	Weapon->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	// Area of attack indicator mesh set up
	AttackAreaIndicatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttackAreaIndicatorMesh"));
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	AttackAreaIndicatorMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	AttackAreaIndicatorMesh->SetHiddenInGame(true);
	
	ChargeAttackAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ChargeAttackAudioComponent"));
	ChargeAttackAudioComponent->SetIsReplicated(true);
	ChargeAttackAudioComponent->SetupAttachment(RootComponent);

	InteractSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	InteractSystem->SetupAttachment(RootComponent);

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
	AttackTrail_NiagaraComponent->SetupAttachment(Weapon->Mesh);
	Attack_NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Attack Component"));
	Attack_NiagaraComponent->SetupAttachment(RootComponent);
	
	// Decal component
	DecalArmSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DecalArrowArm"));
	DecalArmSceneComponent->SetupAttachment(RootComponent);
	DecalArmSceneComponent->SetIsReplicated(false);
	
	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalArrow"));
	DecalComponent->AttachToComponent(DecalArmSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DecalComponent->SetIsReplicated(false);
	
}

void APrototype2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APrototype2Character, Weapon);
	DOREPLIFETIME(APrototype2Character, HeldItem);
	DOREPLIFETIME(APrototype2Character, PlayerMat);
	DOREPLIFETIME(APrototype2Character, PlayerMesh);
	DOREPLIFETIME(APrototype2Character, PlayerID);
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
	
	ChargeAttackAudioComponent->SetSound(ChargeCue);
	ChargeAttackAudioComponent->SetIsReplicated(true);
	ChargeAttackAudioComponent->SetVolumeMultiplier(1.0f);
	ChargeAttackAudioComponent->AttenuationSettings = SoundAttenuationSettings;

	Weapon->Mesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,FName("WeaponHolsterSocket"));
	Weapon->Mesh->SetHiddenInGame(true);
	
	if (PlayerHudPrefab && !PlayerHUDRef && (GetLocalRole() == ROLE_AutonomousProxy || HasAuthority()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player HUD Created"));
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(Cast<APrototype2PlayerController>(Controller), PlayerHudPrefab);

		if (PlayerHUDRef)
			PlayerHUDRef->AddToViewport();
	}

	if (ParticleSystem)
	{
		InteractSystem->SetAsset(ParticleSystem);
	}

	// Clamp the viewing angle of the camera
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = -40.0f;
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 0.0f;

	// Set start position - for decal arrow
	StartPosition = GetActorLocation();

	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
	{
		UpdateDecalDirection(false);
	}

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

		DecalArmSceneComponent->SetIsReplicated(false);
		DecalComponent->SetIsReplicated(false);
	}

	// Particle systems
	if (WalkPoof_NiagaraSystem)
	{
		WalkPoof_NiagaraComponent->SetAsset(WalkPoof_NiagaraSystem);
	}
	if (SprintPoof_NiagaraSystem)
	{
		SprintPoof_NiagaraComponent->SetAsset(SprintPoof_NiagaraSystem);
	}
	if (Sweat_NiagaraSystem)
	{
		Sweat_NiagaraComponent->SetAsset(Sweat_NiagaraSystem);
	}	
	if (AttackTrail_NiagaraSystem)
	{
		AttackTrail_NiagaraComponent->SetAsset(AttackTrail_NiagaraSystem);
	}
	if (Attack_NiagaraSystem)
	{
		Attack_NiagaraComponent->SetAsset(Attack_NiagaraSystem);
	}
	
}

void APrototype2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (PlayerStateRef)
	{
		// Set the reference to the run animation based on the skin (Cow, Pig, etc)
		if (RunAnimations[(int32)PlayerStateRef->Character])
		{		
			RunAnimation = RunAnimations[(int32)PlayerStateRef->Character];	
		}
	}

	if (PlayerMesh)
		GetMesh()->SetSkeletalMeshAsset(PlayerMesh);
	if (PlayerMat)
		GetMesh()->SetMaterial(0, PlayerMat);

	CheckForFloorSurface();
	
	// Stun
	//if (bIsStunned)
	//{
	//	
	//	bIsChargingAttack = false;
	//
	//	// Set animation to stun
	//	
	//	StunTimer -= DeltaSeconds;
	//	if (StunTimer < 0.0f)
	//	{
	//		bIsStunned = false;
	//		//Server_Ragdoll(false);
	//		// Enable input
	//		EnableInput(this->GetLocalViewingPlayerController());
	//	}
	//}

	// Sprint
	if (bIsHoldingGold)
	{
		UpdateCharacterSpeed(GoldPlantSpeed, WalkSpeed, GoldSlowRateScale);
	}
	else
	{
		UpdateCharacterSpeed(WalkSpeed, SprintSpeed, WalkRateScale);
	}
	
	// Attack
	if (bIsChargingAttack)
	{
		AttackChargeAmount += DeltaSeconds;
		
		// Cap attack charge
		if (AttackChargeAmount > MaxAttackCharge)
		{
			AttackChargeAmount = MaxAttackCharge;
		}
		UpdateAOEIndicator();
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
		Server_CountdownTimers(DeltaSeconds);
		
		// Update sprint UI
		PlayerHUDRef->SetPlayerSprintTimer(CanSprintTimer);
		
		if (GetVelocity().Length() > 1.0f)
		{
			InteractSystem->Activate();		
		}
		else
		{
			InteractSystem->Deactivate();
		}
	}

	//if (auto gameState = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
	//{
	//	if (gameState->HasGameFinished)
	//	{
	//		GetMovementComponent()->SetActive(false);
	//		GetController()->SetIgnoreLookInput(true);
	//		GetController()->SetIgnoreMoveInput(true);
	//	}
	//}

	// Update decal rotation
	if (bDecalOn && GetLocalRole() == ROLE_AutonomousProxy)
	{
		UpdateDecalAngle();
	}

	// Walk poof VFX deactivate if not walking
	if (GetCharacterMovement()->Velocity.Size() < 50.0f)
	{
		if (WalkPoof_NiagaraComponent)
		{
			WalkPoof_NiagaraComponent->Deactivate();
		}
	}
}

void APrototype2Character::Server_CountdownTimers_Implementation(float DeltaSeconds)
{
	if (InteractTimer >= 0)
		InteractTimer -= DeltaSeconds;
	if (AttackTimer >= 0)
		AttackTimer -= DeltaSeconds;
	if (SprintTimer >= 0)
		SprintTimer -= DeltaSeconds;
	if (CanSprintTimer >= 0)
		CanSprintTimer -= DeltaSeconds;
}

void APrototype2Character::ChargeAttack()
{
	Server_StartAttack();
}

void APrototype2Character::ReleaseAttack()
{
	Server_ReleaseAttack();
}

void APrototype2Character::ExecuteAttack(float AttackSphereRadius)
{
	// create tarray for hit results
	TArray<FHitResult> outHits;

	FVector inFrontOfPlayer;
	
	// start and end locations
	if (!Weapon->Mesh->bHiddenInGame)
	{
		inFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * AttackSphereRadius) + (GetActorForwardVector() * WeaponReach);
	}
	else
	{
		inFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * AttackSphereRadius) + (GetActorForwardVector() * MeleeReach);
	}
	
	FVector sweepStart = inFrontOfPlayer;
	FVector sweepEnd = inFrontOfPlayer;

	// create a collision sphere
	FCollisionShape colSphere = FCollisionShape::MakeSphere(AttackSphereRadius);
	
	FVector downVector = {inFrontOfPlayer.X, inFrontOfPlayer.Y, GetMesh()->GetComponentLocation().Z};
	TriggerAttackVFX(downVector, AttackSphereRadius, AttackChargeAmount);	
	
	// For Debugging
	// UE_LOG(LogTemp, Warning, TEXT("Sphere Radius = %s"), *FString::SanitizeFloat(AttackSphereRadius));
	
	// draw collision sphere
	// DrawDebugSphere(GetWorld(), inFrontOfPlayer, colSphere.GetSphereRadius(), 50, FColor::Purple, false, 2.0f);
	
	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(outHits, sweepStart, sweepEnd, FQuat::Identity, ECC_Pawn, colSphere);

	bool isPlayerHit = false;
	
	if (isHit)
	{
		// loop through TArray
		for (auto& hit : outHits)
		{
			if (auto* hitPlayer = Cast<APrototype2Character>(hit.GetActor()))
			{
				if (hitPlayer != this)
				{
					// screen log information on what was hit
					// UE_LOG(LogTemp, Warning, TEXT(" %s  was hit by an attack!"), *hit.GetActor()->GetName());

					FVector attackerLocation = GetActorLocation();
					hitPlayer->GetHit(AttackChargeAmount, attackerLocation);

					isPlayerHit = true;
				}
			}
			else if (auto* hitSellBin = Cast<ASellBin_Winter>(hit.GetActor()))
			{
				FVector attackerLocation = GetActorLocation();
				hitSellBin->GetHit(AttackChargeAmount, MaxAttackCharge ,attackerLocation);
			}
		}
	}

	// Lower weapon durability
	if (isPlayerHit)
	{
		WeaponCurrentDurability--;
		PlayerHUDRef->SetWeaponDurability(WeaponCurrentDurability);
		if (WeaponCurrentDurability <= 0)
		{
			Multi_DropWeapon();

			//// Update UI
			//PlayerHUDRef->UpdateWeaponUI(EPickup::NoWeapon);
		}
	}
	
	// Reset Attack Timer
	AttackTimer = AttackTimerTime;

	// Reset Attack variables
	bIsChargingAttack = false;
	AttackChargeAmount = 0.0f;

	// audio
	ChargeAttackAudioComponent->Stop();
	Server_ToggleChargeSound(false);
	PlaySoundAtLocation(GetActorLocation(), ExecuteCue);

	// Stop the player Interacting while "executing attack"
	InteractTimer = InteractTimerTime;

	bCanAttack = true;

	UpdateDecalDirection(false); // Turn off decal as dropped any item

	Server_SocketItem(Weapon->Mesh, FName("WeaponHeldSocket"));
}

void APrototype2Character::Interact()
{
	if(!HeldItem)
	{
		PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
		UpdateDecalDirection(false);
	}
	if (!Weapon)
	{
		PlayerHUDRef->UpdateWeaponUI(EPickup::None);
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
	
	// Debug draw collision sphere
	//FCollisionShape colSphere = FCollisionShape::MakeSphere(InteractRadius);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), colSphere.GetSphereRadius(), 50, FColor::Purple, false, 3.0f);
}

void APrototype2Character::Sprint()
{
	Server_Sprint();
}

void APrototype2Character::UpdateCharacterSpeed(float _WalkSpeed, float _SprintSpeed, float _BaseAnimationRateScale)
{
	// If not sprinting
	if (SprintTimer < 0.0f)
	{
		GetCharacterMovement()->MaxWalkSpeed = _WalkSpeed;

		// Speed up animation
		if (RunAnimation)
		{
			RunAnimation->RateScale = _BaseAnimationRateScale;
		}

		// VFX
		SprintPoof_NiagaraComponent->Deactivate();
	}
	else // If Sprinting
	{
		GetCharacterMovement()->MaxWalkSpeed = _SprintSpeed;
		if(RunAnimation)
		{
			RunAnimation->RateScale = _BaseAnimationRateScale * SprintRateScaleScalar;
		}

		// VFX
		WalkPoof_NiagaraComponent->Deactivate();
		Sweat_NiagaraComponent->Activate();
		SprintPoof_NiagaraComponent->Activate();
	}
	
	if (CanSprintTimer < 0.0f)
	{
		Sweat_NiagaraComponent->Deactivate();
	}
}

void APrototype2Character::CheckForInteractables()
{
	TArray<FHitResult> outHits;
	FVector sweepStart = GetActorLocation();
	FVector sweepEnd = GetActorLocation();
	FCollisionShape colSphere = FCollisionShape::MakeSphere(InteractRadius * 1.5f);
	// draw collision sphere
	//DrawDebugSphere(GetWorld(), GetActorLocation(), colSphere.GetSphereRadius(), 50, FColor::Purple, false, 0.1f);
	
	if (GetWorld()->SweepMultiByChannel(outHits, sweepStart, sweepEnd, FQuat::Identity, ECC_Visibility, colSphere))
	{
		TArray<AActor*> interactableActors;
		for (auto& hit : outHits)
		{
			if (Cast<IInteractInterface>(hit.GetActor()))
			{
				interactableActors.Add(hit.GetActor());
			}
		}

		float distanceToClosest;
		auto nearestActor = UGameplayStatics::FindNearestActor(GetActorLocation(), interactableActors, distanceToClosest);
		
		if (distanceToClosest <= InteractRadius && nearestActor)
		{
			if (ClosestInteractableActor && ClosestInteractableActor != nearestActor)
			{
				EnableStencil(false);
			}
			ClosestInteractableActor = nearestActor;
			ClosestInteractableItem = Cast<IInteractInterface>(nearestActor);
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
		ClosestInteractableItem = nullptr;
		ClosestInteractableActor = nullptr;
	}
}

void APrototype2Character::EnableStencil(bool _on)
{
	if (ClosestInteractableActor)
	{
		if (auto component = ClosestInteractableActor->GetComponentByClass(UItemComponent::StaticClass()))
		{
			if (auto itemComponent = Cast<UItemComponent>(component))
			{
				itemComponent->Mesh->SetRenderCustomDepth(_on);
			}
		}
	}
}



void APrototype2Character::GetHit(float AttackCharge, FVector AttackerLocation)
{
	// Disable input
	//DisableInput(this->GetLocalViewingPlayerController());

	// Fire dizzy particle
	//Server_FireParticleSystem(Dizzy_NiagaraSystem, Dizzy_NiagaraComponent->GetComponentLocation());
	
	//Server_Ragdoll(true);
	
	// Knockback
	FVector KnockAway = GetActorUpVector()/2 + (GetActorLocation() - AttackerLocation).GetSafeNormal();

	// Set minimum attack charge for scaling knockback
	if (AttackCharge < 1.0f)
	{
		AttackCharge = 1.0f;
	}
	
	KnockAway *= AttackCharge * KnockBackAmount;
	
	UKismetSystemLibrary::PrintString(GetWorld(), "Pre limit: " + FString::SanitizeFloat(KnockAway.Size()));
	// Limit the knockback to MaxKnockBackVelocity
	if (KnockAway.Size() > MaxKnockBackVelocity)
	{
		KnockAway = KnockAway.GetSafeNormal() * MaxKnockBackVelocity;
		UKismetSystemLibrary::PrintString(GetWorld(), "Post limit: " + FString::SanitizeFloat(KnockAway.Size()));
	}

	// Knock this player away
	GetCharacterMovement()->Launch(KnockAway);
	
	// Drop item
	if (HeldItem)
	{
		Server_DropItem();
	}

	// debug attack
	//UE_LOG(LogTemp, Warning, TEXT("AttackCharge: %s"), *FString::SanitizeFloat(AttackCharge));
	
	//bIsStunned = true;
	//StunTimer = 2.0f;

	PlaySoundAtLocation(GetActorLocation(), GetHitCue);

	// VFX
	FVector AttackVFXLocation = GetActorLocation() - AttackerLocation;
	AttackVFXLocation = AttackVFXLocation.GetSafeNormal();
	AttackVFXLocation*=100.0f;
	Attack_NiagaraComponent->SetWorldLocation(AttackVFXLocation);
	Attack_NiagaraComponent->Activate();
}

void APrototype2Character::Multi_SocketItem_Implementation(UStaticMeshComponent* _object, FName _socket)
{
	_object->SetSimulatePhysics(false);
	_object->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_object->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	_object->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(_socket));
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

void APrototype2Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
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

	if (!WalkPoof_NiagaraComponent->IsActive())
	{
		WalkPoof_NiagaraComponent->Activate();
	}
}

void APrototype2Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

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
	FVector playerPos = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
	FRotator newRotation{};
	
	if (bDecalTargetShippingBin)
	{
		FVector sellPos = FVector(SellBin->GetActorLocation().X, SellBin->GetActorLocation().Y, 0);
		
		newRotation = UKismetMathLibrary::FindLookAtRotation(playerPos, sellPos);
	}
	else
	{
		FVector plotPos = FVector(StartPosition.X, StartPosition.Y, 0);
		
		newRotation = UKismetMathLibrary::FindLookAtRotation(playerPos, plotPos);
	}

	DecalArmSceneComponent->SetWorldRotation(newRotation);
}

void APrototype2Character::UpdateAOEIndicator()
{
	AttackAreaIndicatorMesh->SetHiddenInGame(false);

	float AttackSphereRadius;	
	FVector inFrontOfPlayer;
	
	// start and end locations
	if (!Weapon->Mesh->bHiddenInGame)
	{
		// Create a larger sphere of effect
		AttackSphereRadius = BaseAttackRadius + AttackChargeAmount * WeaponAttackRadiusScalar;
		inFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * AttackSphereRadius) + (GetActorForwardVector() * WeaponReach);
	}
	else
	{
		// Create a smaller sphere of effect
		AttackSphereRadius = BaseAttackRadius;
		inFrontOfPlayer = GetActorLocation() + (GetActorForwardVector() * AttackSphereRadius) + (GetActorForwardVector() * MeleeReach);
	}
	
	FVector downVector = {inFrontOfPlayer.X, inFrontOfPlayer.Y, GetMesh()->GetComponentLocation().Z};

	AttackAreaIndicatorMesh->SetWorldLocation(downVector);
	AttackAreaIndicatorMesh->SetRelativeScale3D({AttackSphereRadius,AttackSphereRadius,AttackChargeAmount * WeaponAttackRadiusScalar});
	TriggerAttackVFX(downVector, AttackSphereRadius, AttackChargeAmount);	
	
}

void APrototype2Character::UpdateDecalDirection(bool _on)
{
	DecalComponent->SetVisibility(_on);
}

void APrototype2Character::UpdateDecalDirection(bool _on, bool _targetShippingBin)
{
	DecalComponent->SetVisibility(_on);

	if (_on)
	{
		bDecalOn = true;
		bDecalTargetShippingBin = _targetShippingBin;
	}
	else
	{
		bDecalOn = false;
	}
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

	TArray<FHitResult> HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true; // Enable complex tracing for accurate physics material retrieval
	QueryParams.bReturnPhysicalMaterial = true;

	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	GetCharacterMovement()->BrakingFriction = 2.0f;
	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->GroundFriction = 8.0f;

	
	// Perform the line trace
	if (GetWorld()->LineTraceMultiByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
	{
		for(auto& result : HitResult)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Ground Check Hit %s"), *FString(HitResult.Component->GetName()));
			UPhysicalMaterial* PhysMaterial = result.PhysMaterial.Get();
			if (PhysMaterial)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Ground Check Hit Physcs Material"));
				//UKismetSystemLibrary::DrawDebugLine(GetWorld(), StartLocation, result.Location, FColor::Red, 0.1f, 5.0f);
				float Friction = PhysMaterial->Friction;
				if (Friction <= 0.5f)
				{
					GetCharacterMovement()->BrakingFriction = 0.0f;
					GetCharacterMovement()->MaxAcceleration = 2048.0f * 0.5f;
					GetCharacterMovement()->GroundFriction = 0.0f;
					//UE_LOG(LogTemp, Error, TEXT("Ground Check Hit Slippery"));
					break;
				}
			}
		}
	}
}

void APrototype2Character::PlaySoundAtLocation(FVector Location, USoundCue* SoundToPlay, USoundAttenuation* _attenation)
{
	Server_PlaySoundAtLocation(Location,SoundToPlay, _attenation );
}

void APrototype2Character::Ragdoll(bool _ragdoll)
{
	if (_ragdoll)
	{
		SetReplicateMovement(false);
		
		/* Disable all collision on capsule */
		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetActorEnableCollision(true);

		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->WakeAllRigidBodies();
		GetMesh()->bBlendPhysics = true;
		

		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp)
		{
			CharacterComp->StopMovementImmediately();
			CharacterComp->DisableMovement();
			CharacterComp->SetComponentTickEnabled(false);
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
		UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
		if (CharacterComp)
		{
			CharacterComp->SetMovementMode(EMovementMode::MOVE_Walking);
			CharacterComp->SetComponentTickEnabled(true);
		}
	}	
}

void APrototype2Character::Server_Ragdoll_Implementation(bool _ragdoll)
{
	if (_ragdoll)
	{
		LocationWhenStunned = GetActorTransform();
		MeshLocationWhenStunned = GetMesh()->GetRelativeTransform();
	}

	Multi_Ragdoll(_ragdoll);
}

void APrototype2Character::Multi_Ragdoll_Implementation(bool _ragdoll)
{
	Ragdoll(_ragdoll);
}

UWidget_PlayerHUD* APrototype2Character::GetPlayerHUD()
{
	// Update UI
	return PlayerHUDRef;
}

void APrototype2Character::PlayNetworkMontage(UAnimMontage* _montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_montage);
	Server_PlayNetworkMontage(_montage);
}

void APrototype2Character::Server_PlayNetworkMontage_Implementation(UAnimMontage* _montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_montage);
	Multi_PlayNetworkMontage(_montage);
}

void APrototype2Character::Multi_PlayNetworkMontage_Implementation(UAnimMontage* _montage)
{
	GetMesh()->GetAnimInstance()->Montage_Play(_montage);
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
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Time until you can sprint again: %f"), CanSprintTimer);
	}
}

void APrototype2Character::SocketWeapon(FName Socket)
{
	Server_SocketItem(Weapon->Mesh, Socket);
}

void APrototype2Character::Server_AddHUD_Implementation()
{
	Client_AddHUD();
}

void APrototype2Character::Multi_Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef)
    	{
    		//UE_LOG(LogTemp, Warning, TEXT("Player HUD Created"));
    
    		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), PlayerID), PlayerHudPrefab);
    
    		if (PlayerHUDRef)
    			PlayerHUDRef->AddToViewport();
    	}
}

void APrototype2Character::Server_StartAttack_Implementation()
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
			//Server_SocketItem(Weapon->Mesh, FName("WeaponHeldSocket"));
					
			Server_SocketItem(Weapon->Mesh, FName("WeaponAttackingSocket"));
		}

		Server_ToggleChargeSound(true);
	}
}

void APrototype2Character::Multi_StartAttack_Implementation()
{
	
}

void APrototype2Character::Server_ReleaseAttack_Implementation()
{
	// Create a sphere collider, check if player hit, call player hit
	
	if (bIsChargingAttack && bCanAttack)
	{
		bCanAttack = false;

		// Socket Weapon back to held
		Server_SocketItem(Weapon->Mesh, FName("WeaponHeldSocket"));
		
		// Set the radius of the sphere for attack
		int32 attackSphereRadius;
		if (!Weapon->Mesh->bHiddenInGame)
		{
			// Create a larger sphere of effect
			attackSphereRadius = BaseAttackRadius + AttackChargeAmount * WeaponAttackRadiusScalar;
			
			// VFX
			AttackTrail_NiagaraComponent->Activate();
		}
		else
		{
			// Create a smaller sphere of effect
			attackSphereRadius = BaseAttackRadius;
		}

		// If attack button is clicked without being held
		if (AttackChargeAmount < MaxAttackCharge)//InstantAttackThreshold)
		{
			// Animation
			if(ExecuteAttackMontage_LongerWindUp)
			{
				PlayNetworkMontage(ExecuteAttackMontage_LongerWindUp);
			}
			// Delayed attack
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this, attackSphereRadius] { ExecuteAttack(attackSphereRadius); }), InstantAttackDelay, false);	
		}
		else
		{
			// Animation
			if(ExecuteAttackMontage)
			{
				PlayNetworkMontage(ExecuteAttackMontage);
			}
			
			ExecuteAttack(attackSphereRadius);

		}
	}

	// empty
	//Multi_ReleaseAttack();
}

void APrototype2Character::Multi_ReleaseAttack_Implementation()
{
}

void APrototype2Character::Server_PlaySoundAtLocation_Implementation(FVector _location, USoundCue* _soundQueue, USoundAttenuation* _attenation)
{
	Multi_PlaySoundAtLocation(_location, _soundQueue, _attenation);
}

void APrototype2Character::Multi_PlaySoundAtLocation_Implementation(FVector _location, USoundCue* _soundQueue, USoundAttenuation* _attenation)
{
	if (_attenation)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _soundQueue, _location, 1, 1, 0, _attenation);
	else
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _soundQueue, _location, 1, 1, 0, SoundAttenuationSettings);
}

void APrototype2Character::Client_AddHUD_Implementation()
{
	if (PlayerHudPrefab && !PlayerHUDRef)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Player HUD Created"));
		PlayerHUDRef = CreateWidget<UWidget_PlayerHUD>(Cast<APrototype2PlayerController>(Controller), PlayerHudPrefab);

		PlayerHUDRef->AddToViewport();
	}
}

void APrototype2Character::Multi_SetPlayerColour_Implementation()
{
	if (auto* gamemode = Cast<APrototype2GameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		if (auto* gamestate = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld())))
		{
			for(auto player : gamestate->PlayerArray)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Player Colour Assigned Locally"));
				if (auto* character = Cast<APrototype2Character>(player->GetPlayerController()->GetCharacter()))
				{
					//character->GetMesh()->SetMaterial(0, gamemode->PlayerMaterials[Cast<APrototype2PlayerState>(player)->Player_ID]);
				}
			}
		}
	}
}

void APrototype2Character::TryInteract()
{
	if (ClosestInteractableItem)
	{
		ClosestInteractableItem->ClientInteract(this);
	}
}

void APrototype2Character::Server_TryInteract_Implementation()
{
	if (ClosestInteractableItem)
	{
		InteractTimer = InteractTimerTime;

		UE_LOG(LogTemp, Warning, TEXT("Attempted to Interact!"));
		ClosestInteractableItem->Interact(this);
		
		if (HeldItem)
		{
			if (PickupMontage)
			{
				PlayNetworkMontage(PickupMontage);
			}
		}
		if (Weapon)
		{
			if (!Weapon->Mesh->bHiddenInGame)
			{
				Multi_SocketItem(Weapon->Mesh, FName("WeaponHolsterSocket"));
			}
		}
	}
	else if (HeldItem && !ClosestInteractableItem)
	{
		InteractTimer = InteractTimerTime;
		Multi_DropItem();
	}
}

void APrototype2Character::Server_DropItem_Implementation()
{
	Multi_DropItem();
}

void APrototype2Character::Multi_DropItem_Implementation()
{
	// Drop into world
	if(HeldItem)
	{
		// If Item was gold, set bool to return character to normal speed in tick
		if (HeldItem->ItemComponent->gold)
		{
			bIsHoldingGold = false;
		}
		
		HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//HeldItem->SetActorLocation({HeldItem->GetActorLocation().X,HeldItem->GetActorLocation().Y,0 });
		HeldItem->ItemComponent->Mesh->SetSimulatePhysics(true);
		HeldItem->ItemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
		// So that CheckForInteractables() can see it again
		HeldItem->ItemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		
		//// Launch the HeldItem towards the player instead of droppping
		//FVector LaunchItemVelocity = GetVelocity();
		//LaunchItemVelocity = LaunchItemVelocity.GetSafeNormal();
		//LaunchItemVelocity *= ItemLaunchStrength;
		//HeldItem->ItemComponent->Mesh->AddForce(LaunchItemVelocity);
	}
	
	HeldItem = nullptr;
	
	// Set HUD image
	if (PlayerHUDRef)
	{
		PlayerHUDRef->UpdatePickupUI(EPickup::None, false);
	}
	PlaySoundAtLocation(GetActorLocation(), DropCue);
}

void APrototype2Character::Server_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item)
{
	if (HeldItem)
	{
		Multi_DropItem();
	}

	Multi_PickupItem(itemComponent, _item);
}

void APrototype2Character::Server_SocketItem_Implementation(UStaticMeshComponent* _object, FName _socket)
{
	Multi_SocketItem(_object, _socket);
}

void APrototype2Character::Server_DropWeapon_Implementation()
{
	Multi_DropWeapon();
}

void APrototype2Character::Multi_PickupItem_Implementation(UItemComponent* itemComponent, APickUpItem* _item)
{
	// Audio
	PlaySoundAtLocation(GetActorLocation(), PickUpCue);

	if (itemComponent->Mesh)
	{
		itemComponent->Mesh->SetSimulatePhysics(false);
		itemComponent->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// So that CheckForInteractables() cant see it while player is holding it
		itemComponent->Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
	
	if (itemComponent->PickupType == EPickup::Weapon)
	{
		Weapon->Mesh->SetStaticMesh(itemComponent->Mesh->GetStaticMesh());
		Weapon->Mesh->SetHiddenInGame(false);
		Weapon->Mesh->SetVisibility(true);
		HeldItem = nullptr;
		return;
	}
		
	_item->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("HeldItemSocket"));

	HeldItem = _item;
	
	if (HeldItem->ItemComponent->gold)
	{
		bIsHoldingGold = true;
	}
		
	if (PlayerHUDRef && HeldItem)
		PlayerHUDRef->UpdatePickupUI(HeldItem->ItemComponent->PickupType, bIsHoldingGold);
}

void APrototype2Character::Multi_DropWeapon_Implementation()
{
	Weapon->Mesh->SetVisibility(false);
}

void APrototype2Character::Server_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& InMaterialsArray)
{
	Multi_ReceiveMaterialsArray(InMaterialsArray);
}

void APrototype2Character::Multi_ReceiveMaterialsArray_Implementation(
	const TArray<UMaterialInstance*>& _inMaterialsArray)
{
	USkeletalMeshComponent* meshComponent = GetMesh();
	if (meshComponent && _inMaterialsArray.Num() > 0)
	{
		for (int32 i = 0; i < _inMaterialsArray.Num(); i++)
		{
			meshComponent->SetMaterial(i, _inMaterialsArray[i]);
		}
	}
}

void APrototype2Character::Server_FireParticleSystem_Implementation(UNiagaraSystem* _NiagaraSystem, FVector _Position)
{
	Multi_FireParticleSystem(_NiagaraSystem, _Position);
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

void APrototype2Character::Server_ToggleChargeSound_Implementation(bool _soundEnabled)
{
	Multi_ToggleChargeSound(_soundEnabled);
}
