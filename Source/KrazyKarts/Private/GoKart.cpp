// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	AccelerationDueToGravity = GetWorld()->GetGravityZ() / 100;
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = CalculateForceOnCar();
	FVector Acceleration = Force / Mass;

	Velocity += Acceleration * DeltaTime;

	UpdateRotationFromSteering(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);
}

FVector AGoKart::CalculateForceOnCar()
{
	FVector ForwardForce = GetActorForwardVector() * MaxDrivingForce * Throttle;
	float AirResistance = Velocity.SizeSquared() * DragCoefficient;
	FVector DragForce = Velocity.GetSafeNormal() * AirResistance;
	FVector NormalForce = -Velocity.GetSafeNormal() * Mass * RollingResistanceCoefficient * AccelerationDueToGravity;
	// Return throttle force - drag force - normal force
	return ForwardForce - DragForce - NormalForce;
}

void AGoKart::UpdateRotationFromSteering(float DeltaTime)
{
	float RotationAngle = MaxDegreesPerSecond * DeltaTime * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));

	AddActorWorldRotation(RotationDelta);
	// Rotate Velocity with the RotationDelta
	Velocity = RotationDelta.RotateVector(Velocity);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = 100 * Velocity * DeltaTime;
	FHitResult ObjectHit;
	AddActorWorldOffset(Translation, true, &ObjectHit);
	if (ObjectHit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
}

void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
}

