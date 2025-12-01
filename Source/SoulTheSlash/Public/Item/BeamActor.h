// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BeamActor.generated.h"

UCLASS()
class SOULTHESLASH_API ABeamActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABeamActor();

protected:
	virtual void BeginPlay() override;

	

};
