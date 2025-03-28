// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoveStateBase.h"
#include "HangState.generated.h"

UCLASS()
class PROJECTT_API UHangState : public UMoveStateBase
{
	GENERATED_BODY()

public:
	virtual void OnStateEnter() override;
	virtual void OnStateTick(float DeltaTime)  override;
	virtual void OnStateExit() override;

private:
	void TransitionToClimb();
	void TransitionToFall();
	bool MoveUp();
	bool FindForwardEdge(const FVector& StartLocation, const FVector& WallNormal, FVector& OutForwardHitWithOffset);
	bool FindFinalPosition(const FVector& UpDirection, FVector& OutFinal, const FVector& ForwardOffset);
	FVector FindHighestPosition(const FVector& LedgeLocationRaw, const FVector& WallNormal, const FVector& UpDirection);

	UPROPERTY()
	float MinimumLedgeDepth = 50.f;
	UPROPERTY()
	float BackwardsForce = 100.f;
};
