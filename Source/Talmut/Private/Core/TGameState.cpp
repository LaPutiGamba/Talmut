#include "Core/TGameState.h"
#include "TCard.h"
#include "TDeck.h"
#include "TDiscardPile.h"
#include "TPawn.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ATGameState::ATGameState()
{
	StateOfTheGame = EStateOfTheGame::GS_Uninitialized;

	CurrentPlayerIndex = 0;
	LastPlayerIndex = -1;
}

void ATGameState::SpawnDeckAndDiscardPile()
{
	Deck = GetWorld()->SpawnActor<ATDeck>(DeckClass);
	DiscardPile = GetWorld()->SpawnActor<ATDiscardPile>(DiscardPileClass);
}

void ATGameState::InitializeDeck()
{
	//Algo::RandomShuffle(CardTextures);
	DeckCards.Reserve(CardTextures.Num());

	for (uint8 i = 0; i < CardTextures.Num(); i++) {
		ATCard* Card = GetWorld()->SpawnActor<ATCard>(CardClass);
		Card->SetOwner(this);
		Card->MulticastInitialize(CardTextures[i], i);

		DeckCards.Add(Card);
	}

	ServerSetStateOfTheGame(EStateOfTheGame::GS_WaitingForPlayers);
}

void ATGameState::StartGame()
{
	DealInitialCards();

	ServerSetStateOfTheGame(EStateOfTheGame::GS_Dealing);
}

void ATGameState::DealInitialCards()
{
	for (ATPawn* Pawn : Pawns) {
		for (uint8 i = 0; i < 4; i++) {
			ATCard* Card = DeckCards.Pop();
			Card->SetOwner(Pawn);
			Pawn->AddCardToHand(Card);

			FVector TargetLocation = Pawn->CalculateHandTargetLocation();
			FRotator TargetRotation = Pawn->GetActorRotation();
			TargetRotation.Roll = 180.f;
			if (i == 0) {
				Card->ServerMoveCard(TargetLocation, TargetRotation);
			} else {
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, [Card, TargetLocation, TargetRotation]() {
					Card->ServerMoveCard(TargetLocation, TargetRotation);
				}, 1.f * i, false);
			}
		}
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]() {
		ServerSetStateOfTheGame(EStateOfTheGame::GS_StartShowingInitialCards);
	}, 5.5f, false);
}

void ATGameState::MulticastShowInitialCards_Implementation()
{
	if (HasAuthority())
		ServerSetStateOfTheGame(EStateOfTheGame::GS_ShowingInitialCards);

	if (ATPawn* Pawn = Cast<ATPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		for (uint8 i = 0; i < Pawn->GetHand().Num(); i += 2) {
			if (Pawn->GetHand().IsValidIndex(i)) {
				FVector CardLocation = Pawn->GetHand()[i]->GetActorLocation();
				CardLocation.Z += 10.f;
				FRotator CardRotation = Pawn->GetHand()[i]->GetActorRotation();
				CardRotation.Roll = 0.f;
				Pawn->GetHand()[i]->MoveCard(CardLocation, CardRotation);
			}
		}

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, [this, Pawn]() {
			for (uint8 i = 0; i < Pawn->GetHand().Num(); i += 2) {
				if (Pawn->GetHand().IsValidIndex(i)) {
					FVector CardLocation = Pawn->GetHand()[i]->GetActorLocation();
					CardLocation.Z -= 10.f;
					FRotator CardRotation = Pawn->GetHand()[i]->GetActorRotation();
					CardRotation.Roll = 180.f;
					Pawn->GetHand()[i]->MoveCard(CardLocation, CardRotation);
				}
			}

			if (HasAuthority())
				ServerSetStateOfTheGame(EStateOfTheGame::GS_StartPlaying);
		}, 3.0f, false);
	}
}

void ATGameState::StartPlayerTurn()
{
	if (Pawns.IsValidIndex(CurrentPlayerIndex)) {
		Pawns[CurrentPlayerIndex]->ClientStartTurn();
	}
}

void ATGameState::EndPlayerTurn()
{
	if (Pawns.IsValidIndex(CurrentPlayerIndex)) {
		Pawns[CurrentPlayerIndex]->ClientEndTurn();

		CurrentPlayerIndex = (CurrentPlayerIndex + 1) % Pawns.Num();

		if (CurrentPlayerIndex == LastPlayerIndex) {
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this]() {
				ServerShowAllPlayerHands();

				ServerSetStateOfTheGame(EStateOfTheGame::GS_End);
			}, 3.0f, false);
		} else {
			StartPlayerTurn();
		}
	}
}

bool ATGameState::IsPlayerTurn(ATPawn* Pawn)
{
	if (Pawns.IsValidIndex(CurrentPlayerIndex))
		return Pawns[CurrentPlayerIndex] == Pawn;

	return false;
}

void ATGameState::ServerSetStateOfTheGame_Implementation(EStateOfTheGame NewState)
{
	MulticastSetStateOfTheGame(NewState);

	if (NewState == EStateOfTheGame::GS_StartPlaying)
		StartPlayerTurn();
}

void ATGameState::MulticastSetStateOfTheGame_Implementation(EStateOfTheGame NewState)
{
	StateOfTheGame = NewState;

	switch (StateOfTheGame) {
	case EStateOfTheGame::GS_StartPlaying:
		DiscardPile->EnableBoxCollision(true);

		if (ATPawn* Pawn = Cast<ATPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
			for (ATCard* HandCard : Pawn->GetHand())
				HandCard->EnableCardCollision(true);
		}
		break;
	default:
		break;
	}
}

void ATGameState::ServerCallTalmut_Implementation(ATPawn* ATPawn)
{
	if (LastPlayerIndex == -1)
		LastPlayerIndex = CurrentPlayerIndex;
}

void ATGameState::ServerShowAllPlayerHands_Implementation()
{
	for (ATPawn* Pawn : Pawns) {
		for (ATCard* Card : Pawn->GetHand()) {
			FRotator Rotation = Card->GetActorRotation();
			Rotation.Roll = 0.f;
			Card->ServerMoveCard(Card->GetActorLocation(), Rotation);
		}
	}
}

void ATGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATGameState, Pawns);
	DOREPLIFETIME(ATGameState, DeckCards);
	DOREPLIFETIME(ATGameState, Deck);
	DOREPLIFETIME(ATGameState, DiscardCards);
	DOREPLIFETIME(ATGameState, DiscardPile);
	DOREPLIFETIME(ATGameState, CurrentPlayerIndex);
	DOREPLIFETIME(ATGameState, LastPlayerIndex);
}
