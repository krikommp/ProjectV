#pragma once

#define FIND_PAWN_COMP_FUNC(Name)\
	UFUNCTION(BlueprintPure) \
	static U##Name* Find##Name(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<U##Name>() : nullptr); }

#define FIND_PAWN_COMP(NAME, PAWN) U##NAME::Find##NAME((PAWN))

#define FIND_STATE_COMP(STATE_CLASS, NAME) GetGameState<STATE_CLASS>()->FindComponentByClass<U##NAME>()
