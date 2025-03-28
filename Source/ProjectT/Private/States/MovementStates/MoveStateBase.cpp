// Fill out your copyright notice in the Description page of Project Settings.


#include "States/MovementStates/MoveStateBase.h"
#include "States/StateMachine.h"
#include "Characters/PlayerChar.h"
#include "UObject/UObjectBaseUtility.h"


void UMoveStateBase::Init(TObjectPtr<UStateMachine> StateMachine, APlayerChar* Owner, EMovementState State)
{
	Machine = StateMachine;
	MyState = State;
	Char = Owner;
	check(Machine != nullptr);
	check(Char != nullptr);
}


EMovementState UMoveStateBase::GetState()
{
	return MyState;
}