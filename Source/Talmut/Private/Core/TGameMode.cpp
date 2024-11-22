#include "Core/TGameMode.h"
#include "TPawn.h"
#include "Core/TGameState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ATGameMode::ATGameMode()
{
    PlayerStartIndex = 0;
}

AActor* ATGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    APlayerStart* SelectedPlayerStart = Cast<APlayerStart>(PlayerStarts[PlayerStartIndex]);
    PlayerStartIndex = (PlayerStartIndex + 1) % PlayerStarts.Num();

    if (!SelectedPlayerStart && PlayerStarts.Num() > 0)
        SelectedPlayerStart = Cast<APlayerStart>(PlayerStarts[0]);

    return SelectedPlayerStart;
}

void ATGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (TGameState) {
		if (ATPawn* PlayerPawn = Cast<ATPawn>(NewPlayer->GetPawn()))
			TGameState->Pawns.Add(PlayerPawn);
	}
}

void ATGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GameState) {
		TGameState = Cast<ATGameState>(GameState);
		if (ATPawn* ServerPawn = Cast<ATPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
			TGameState->Pawns.Add(ServerPawn);
	}
}

void ATGameMode::StartPlay()
{
	Super::StartPlay();

    if (TGameState) {
        FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this]() {
            TGameState->SpawnDeckAndDiscardPile();
			TGameState->InitializeDeck();
		}, 2.0f, false);
    }
}