// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKartMovementComponent.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AccelerationDueToGravity = GetWorld()->GetGravityZ() / 100;
	
}

// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If not in a multi-player situation, we are an AutonomousProxy
	if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
	
}

void UGoKartMovementComponent::SimulateMove(FGoKartMove Move)
{
	FVector Force = CalculateForceOnCar(Move);
	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * Move.DeltaTime; 

	UpdateRotationFromSteering(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocationFromVelocity(Move.DeltaTime);

}

FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.Throttle = Throttle;
	Move.SteeringThrow = SteeringThrow;
	Move.Time = GetWorld()->TimeSeconds;
	return Move;
}

FVector UGoKartMovementComponent::CalculateForceOnCar(FGoKartMove Move)
{
	FVector ForwardForce = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;
	float AirResistance = Velocity.SizeSquared() * DragCoefficient;
	FVector DragForce = Velocity.GetSafeNormal() * AirResistance;
	FVector NormalForce = -Velocity.GetSafeNormal() * Mass * RollingResistanceCoefficient * AccelerationDueToGravity;
	// Return throttle force - drag force - normal force
	return ForwardForce - DragForce - NormalForce;
}

void UGoKartMovementComponent::UpdateRotationFromSteering(float DeltaTime, float SteeringThrow)
{
	float RotationAngle = SteeringThrow * FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime / MinTurningRadius;

	// dx = dtheta x SteeringCircleRadius
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	GetOwner()->AddActorWorldRotation(RotationDelta);
	// Rotate Velocity with the RotationDelta
	Velocity = RotationDelta.RotateVector(Velocity);
}

void UGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = 100 * Velocity * DeltaTime;
	FHitResult ObjectHit;
	GetOwner()->AddActorWorldOffset(Translation, true, &ObjectHit);
	if (ObjectHit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

