#pragma once

#define FIND_COMP(Name)\
	UFUNCTION(BlueprintPure) \
	static U##Name* Find##Name(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<U##Name>() : nullptr); }