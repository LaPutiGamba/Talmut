#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TGameMode.generated.h"

UCLASS()
class TALMUT_API ATGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATGameMode();

	virtual void StartPlay() override;

protected:
	virtual void BeginPlay() override;

private:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	TObjectPtr<class ATGameState> TGameState;

	uint8 PlayerStartIndex;
};
