
#include "CullingInterface.h"
#include "Camera/CameraComponent.h"

void ICullingInterface::HandleVisibilityActor(APlayerController* _PlayerController, AActor* _Actor)
{
	// Get the player controller and camera component
	APlayerController* PlayerController = _PlayerController;
	if (!PlayerController)
	{
		return;
	}

	UCameraComponent* CameraComponent = PlayerController->GetViewTarget()->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent)
	{
		return;
	}

	// Calculate the distance between the actor and the camera
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	FVector ActorLocation = _Actor->GetActorLocation();
	float DistanceToCamera = FVector::Dist(ActorLocation, CameraLocation);
	
	
	// Check if the actor is within the culling distance
	if (DistanceToCamera <= CullingDistance)
	{
		// Set the actor's visibility to hidden
		_Actor->SetActorHiddenInGame(true);
	}
	else
	{
		// Set the actor's visibility to visible
		_Actor->SetActorHiddenInGame(false);
	}
}

void ICullingInterface::HandleVisibilityStaticMesh(APlayerController* _PlayerController, AActor* _Actor, UStaticMeshComponent* _Mesh)
{
	// Get the player controller and camera component
	APlayerController* PlayerController = _PlayerController;
	if (!PlayerController)
	{
		return;
	}

	UCameraComponent* CameraComponent = PlayerController->GetViewTarget()->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent)
	{
		return;
	}

	// Calculate the distance between the actor and the camera
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	FVector ActorLocation = _Actor->GetActorLocation();
	float DistanceToCamera = FVector::Dist(ActorLocation, CameraLocation);
	
	
	// Check if the actor is within the culling distance
	if (DistanceToCamera <= CullingDistance)
	{
		
	}
	else
	{
		
	}
}


