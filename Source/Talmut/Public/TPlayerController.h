#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPlayerController.generated.h"

UCLASS()
class TALMUT_API ATPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void AcknowledgePossession(APawn* PossessedPawn) override;
};
