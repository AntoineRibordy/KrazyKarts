// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
		float Throttle;

	UPROPERTY()
		float SteeringThrow;

	UPROPERTY()
		float DeltaTime;

	UPROPERTY()
		float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FVector GetVelocity() { return Velocity; };
	void SetVelocity(FVector InputVelocity) { Velocity = InputVelocity; };
	float GetThrottle() { return Throttle; };
	void SetThrottle(float InputThrottle) { Throttle = InputThrottle; };
	float GetSteeringThrow() { return SteeringThrow; };
	void SetSteeringThrow(float InputSteeringThrow) { SteeringThrow = InputSteeringThrow; };
	FGoKartMove GetLastMove() { return LastMove; };

	void SimulateMove(FGoKartMove Move);

private:
	// The mass of the car (in kg)
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// The force applied to the car whe the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// Higher means more drag (kg/m)
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// Higher means more rolling resistance
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	// Minimum radius of the kart turning circle at full lock (m)
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 5;

	FVector Velocity;

	float Throttle;
	float SteeringThrow;

	float AccelerationDueToGravity;

	FGoKartMove LastMove;

	FVector CalculateForceOnCar(FGoKartMove Move);
	void UpdateLocationFromVelocity(float DeltaTime);

	void UpdateRotationFromSteering(float DeltaTime, float SteeringThrow);

	
	FGoKartMove CreateMove(float DeltaTime);
};
