// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/PlayerStates.h"
#include "UObject/Interface.h"
#include "MoveStateBase.generated.h"

class APlayerChar;
class UStateMachine;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PROJECTT_API UMoveStateBase : public UObject
{
	GENERATED_BODY()
public:
	void Init(TObjectPtr<UStateMachine> StateMachine, APlayerChar* Owner, EMovementState State); 
	virtual void OnStateEnter() PURE_VIRTUAL(,);
	virtual void OnStateTick(float DeltaTime) PURE_VIRTUAL(,);
	virtual void OnStateExit() PURE_VIRTUAL(,);
	virtual EMovementState GetState();
	UPROPERTY()
	TObjectPtr<UStateMachine> Machine;
	UPROPERTY()
	TObjectPtr<APlayerChar> Char;
protected:
	UPROPERTY()
	EMovementState MyState = EMovementState::Ems_MaxNum;
};
