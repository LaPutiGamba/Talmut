#include "TPlayerController.h"
#include "TPawn.h"
#include "Core/TGameInstance.h"

void ATPlayerController::AcknowledgePossession(APawn* PossessedPawn)
{
	Super::AcknowledgePossession(PossessedPawn);

    if (UTGameInstance* GameInstance = Cast<UTGameInstance>(GetGameInstance())) {
		if (ATPawn* GamePawn = Cast<ATPawn>(PossessedPawn)) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Game Pawn possessed"));
			FTimerHandle StopHandle;
			GetWorldTimerManager().SetTimer(StopHandle, [this, GameInstance]() {
				GameInstance->EndLoadingScreen();
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Loading screen ended - Timer"));
			}, 4.f, false);
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Loading screen ended"));
			GameInstance->EndLoadingScreen();
		}
    }
}