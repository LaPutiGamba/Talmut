#include "TPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TCard.h"
#include "TDeck.h"
#include "TDiscardPile.h"
#include "Blueprint/UserWidget.h"
#include "Core/TGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ATPawn::ATPawn()
{
	Hand.Reserve(4);

	bIsDiscardedCard = false;

	JackActivated = 0;
	QueenActivated = 0;
	KingActivated = 0;
}

void ATPawn::BeginPlay()
{
	Super::BeginPlay();

	TGameState = GetWorld()->GetGameState<ATGameState>();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);

		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
}

void ATPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(DefaultMapping, 0);
	}

	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		EnhancedInputComponent->BindAction(ActionMouseClick, ETriggerEvent::Triggered, this, &ATPawn::OnMouseClick);
		EnhancedInputComponent->BindAction(ActionSpace, ETriggerEvent::Triggered, this, &ATPawn::OnPlayerPressSpace);
	}
}

void ATPawn::OnMouseClick()
{
	if (!TGameState)
		return;

	FHitResult HitResult;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	AActor* HitActor = HitResult.GetActor();
	if (HitActor->IsA<ATCard>()) {
		ATCard* Card = Cast<ATCard>(HitActor);

		if (ExecuteCardAbility(Card))
			return;

		if (DrawedCard) {
			// If the player has a drawn card, add it to the player's hand
			ServerChangeHandCard(this, DrawedCard, Card);

			RemoveInteractableCard(Card);
			AddInteractableCard(DrawedCard);

			FVector HandCardLocation = Card->GetActorLocation();
			FRotator HandCardRotation = Card->GetActorRotation();

			Card->ServerMoveDiscardedCard(this);
			DrawedCard->ServerMoveCard(HandCardLocation, HandCardRotation);

			if (bIsDiscardedCard) {
				bIsDiscardedCard = false;
				TGameState->DiscardPile->EnableBoxCollision(true);
			}

			ServerEndTurn();
		} else {
			// If the player clicks on a card, select it
			if (Card == SelectedCard) {
				SelectCard(SelectedCard, false);
			} else {
				if (SelectedCard)
					SelectCard(SelectedCard, false);
				SelectCard(Card, true);
			}
		}
	} else if (HitActor->IsA<ATDiscardPile>()) {
		if (DrawedCard) {
			// If the player has a drawn card, add it to the discard pile
			DrawedCard->ServerMoveDiscardedCard(this);
			RemoveInteractableCard(DrawedCard);

			ServerEndTurn();
		} else if (SelectedCard) {
			if (TGameState->GetDiscardCards().Num() > 0) {
				if (ATCard* LastDiscardedCard = TGameState->GetDiscardCards().Top()) {
					if (SelectedCard->GetCardValue() == LastDiscardedCard->GetCardValue()) {
						// If the player has a selected card, add it to the discard pile if it's the same value as the last discarded card
						SelectedCard->ServerMoveDiscardedCard(this);
						RemoveInteractableCard(SelectedCard);

						SelectedCard->GetCardMesh()->SetRenderCustomDepth(false);
						SelectedCard = nullptr;
					} else {
						// If not, the player can't discard the card and will receive a new card from the deck as a penalty
						if (Hand.Num() < 6) {
							ServerGetPenaltyCard(this);
							SelectCard(SelectedCard, false);
						}
					}
				}
			}
		} else {
			if (TGameState->IsPlayerTurn(this) && !DrawedCard) {
				ServerGetDiscardCard(this);
				TGameState->DiscardPile->EnableBoxCollision(false);
				TGameState->Deck->EnableBoxCollision(false);
				bIsDiscardedCard = true;
			}
		}
	} else if (HitActor->IsA<ATDeck>()) {
		// If it's the player's turn, get a card from the deck and add it to the player's hand
		if (TGameState->IsPlayerTurn(this) && !DrawedCard)
			ServerGetDeckCard(this);
	}
}

void ATPawn::OnPlayerPressSpace()
{
	if (TGameState) {
		switch (TGameState->GetStateOfTheGame()) {
		case EStateOfTheGame::GS_WaitingForPlayers:
			ServerStartGame();
			break;
		case EStateOfTheGame::GS_StartShowingInitialCards:
			ServerShowInitialCards();
			break;
		case EStateOfTheGame::GS_StartPlaying:
			if (TGameState->IsPlayerTurn(this))
				ServerCallTalmut();
			break;
		default:
			break;
		}
	}
}

void ATPawn::ServerGetDeckCard_Implementation(ATPawn* Pawn)
{
	if (TGameState->GetDeckCards().Num() > 0) {
		DrawedCard = TGameState->GetDeckCards().Pop();
		DrawedCard->SetOwner(this);

		FVector TargetLocation = CalculateDrawnTargetLocation();
		FRotator TargetRotation = GetActorRotation();
		TargetRotation.Roll = 180.f;
		DrawedCard->ServerMoveCard(TargetLocation, TargetRotation);

		TargetRotation.Roll = 0.f;
		DrawedCard->ServerMoveDrawedCard(TargetLocation, TargetRotation, this);

		MulticastAddInteractableCard(Pawn, DrawedCard);
	}
}

void ATPawn::ServerGetDiscardCard_Implementation(ATPawn* Pawn)
{
	if (TGameState->GetDiscardCards().Num() > 0) {
		DrawedCard = TGameState->GetDiscardCards().Pop();
		DrawedCard->SetOwner(this);

		FVector TargetLocation = CalculateDrawnTargetLocation();
		FRotator TargetRotation = GetActorRotation();
		TargetRotation.Roll = 180.f;
		DrawedCard->ServerMoveCard(TargetLocation, TargetRotation);

		TargetRotation.Roll = 0.f;
		DrawedCard->ServerMoveDrawedCard(TargetLocation, TargetRotation, this);

		MulticastAddInteractableCard(Pawn, DrawedCard);
	}
}

void ATPawn::ServerGetPenaltyCard_Implementation(ATPawn* Pawn)
{
	if (TGameState->GetDeckCards().Num() > 0) {
		DrawedCard = TGameState->GetDeckCards().Pop();
		DrawedCard->SetOwner(this);

		Hand.Add(DrawedCard);
		MulticastAddInteractableCard(Pawn, DrawedCard);

		FVector TargetLocation = CalculateHandTargetLocation();
		FRotator TargetRotation = GetActorRotation();
		TargetRotation.Roll = 180.f;
		DrawedCard->ServerMoveCard(TargetLocation, TargetRotation);

		DrawedCard = nullptr;
	}
}

void ATPawn::ServerStartGame_Implementation() {
	if (TGameState)
		TGameState->StartGame();
}

void ATPawn::ServerShowInitialCards_Implementation()
{
	if (TGameState)
		TGameState->MulticastShowInitialCards();
}

void ATPawn::ClientStartTurn_Implementation()
{
	if (!TGameState)
		return;

	TGameState->Deck->EnableBoxCollision(true);
}

void ATPawn::ClientEndTurn_Implementation()
{
	if (!TGameState)
		return;

	DrawedCard = nullptr;

	TGameState->Deck->EnableBoxCollision(false);
}

void ATPawn::ServerEndTurn_Implementation()
{
	if (TGameState)
		TGameState->EndPlayerTurn();
}

void ATPawn::SelectCard(ATCard* CardToSelect, bool bSelectIt)
{
	if (Hand.Contains(CardToSelect)) {
		if (bSelectIt) {
			SelectedCard = CardToSelect;
			SelectedCard->SetSelected(true);
			SelectedCard->GetCardMesh()->SetRenderCustomDepth(true);
		} else {
			CardToSelect->GetCardMesh()->SetRenderCustomDepth(false);
			SelectedCard->SetSelected(false);
			SelectedCard = nullptr;
		}
	}
}

bool ATPawn::ExecuteCardAbility(ATCard* Card)
{
	if (JackActivated > 0) {
		JackActivated--;
		FlipCard(Card);

		return true;
	}
	if (QueenActivated > 0) {
		QueenActivated--;

		FlipCard(Card);

		TGameState->DiscardPile->EnableBoxCollision(true);
		for (ATPawn* Pawn : TGameState->GetPawns()) {
			for (ATCard* HandCard : Pawn->GetHand()) {
				if (Pawn != this)
					RemoveInteractableCard(HandCard);
				else
					AddInteractableCard(HandCard);
			}
		}

		return true;
	}
	if (KingActivated > 0) {
		if (Hand.Contains(Card))
			KingHandCard = Card;
		else
			KingStolenCard = Card;

		if (KingHandCard && KingStolenCard) {
			KingActivated--;

			ATPawn* KingStolenCardPawn = nullptr;

			// Enabling discard pile collisions
			TGameState->DiscardPile->EnableBoxCollision(true);
			// Removing other players collisions
			for (ATPawn* Pawn : TGameState->GetPawns()) {
				for (ATCard* HandCard : Pawn->GetHand()) {
					if (Pawn != this)
						RemoveInteractableCard(HandCard);
					else
						AddInteractableCard(HandCard);

					if (HandCard == KingStolenCard)
						KingStolenCardPawn = Pawn;
				}
			}

			ServerChangeHandCardCollisions(KingStolenCardPawn, this, KingStolenCard, KingHandCard);

			FVector HandCardLocation = KingHandCard->GetActorLocation();
			FVector StolenCardLocation = KingStolenCard->GetActorLocation();
			FRotator HandCardRotation = KingHandCard->GetActorRotation();
			FRotator StolenCardRotation = KingStolenCard->GetActorRotation();

			ServerMoveCards(KingHandCard, StolenCardLocation, StolenCardRotation);
			ServerMoveCards(KingStolenCard, HandCardLocation, HandCardRotation);

			KingHandCard = nullptr;
			KingStolenCard = nullptr;
		}

		return true;
	}

	return false;
}

void ATPawn::FlipCard(ATCard* Card)
{
	FVector TargetLocation = Card->GetActorLocation();
	FRotator TargetRotation = Card->GetActorRotation();
	TargetRotation.Roll = 0.f;
	Card->MoveCard(TargetLocation, TargetRotation);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this, Card, TargetLocation, TargetRotation]() {
		FRotator ModifiedRotation = TargetRotation;
		ModifiedRotation.Roll = 180.f;
		Card->MoveCard(TargetLocation, ModifiedRotation);
	}, 3.0f, false);
}

void ATPawn::ClientStartQueenActivated_Implementation()
{
	TGameState->DiscardPile->EnableBoxCollision(false);
	for (ATPawn* Pawn : TGameState->GetPawns()) {
		for (ATCard* HandCard : Pawn->GetHand()) {
			if (Pawn != this)
				AddInteractableCard(HandCard);
			else
				RemoveInteractableCard(HandCard);
		}
	}
}

void ATPawn::ClientStartKingActivated_Implementation()
{
	TGameState->DiscardPile->EnableBoxCollision(false);
	for (ATPawn* GSPawn : TGameState->GetPawns()) {
		for (ATCard* HandCard : GSPawn->GetHand())
			AddInteractableCard(HandCard);
	}
}

void ATPawn::ServerMoveCards_Implementation(class ATCard* CardToMove, const FVector& TargetLocation, const FRotator& TargetRotation)
{
	CardToMove->ServerMoveCard(TargetLocation, TargetRotation);
}

void ATPawn::ServerCallTalmut_Implementation()
{
	if (TGameState)
		TGameState->ServerCallTalmut(this);
}

void ATPawn::ServerChangeHandCard_Implementation(ATPawn* PawnToModify, ATCard* CardToAdd, ATCard* CardToRemove)
{
	AActor* CardToAddOwner = CardToAdd->GetOwner();
	AActor* CardToRemoveOwner = CardToRemove->GetOwner();

	if (PawnToModify->GetHand().Contains(CardToRemove))
		PawnToModify->RemoveCardFromHand(CardToRemove);

	PawnToModify->AddCardToHand(CardToAdd);

	CardToRemove->SetOwner(CardToAddOwner);
	CardToAdd->SetOwner(CardToRemoveOwner);

	ForceNetUpdate();
}

void ATPawn::AddInteractableCard(ATCard* CardToAdd)
{
	if (!InteractableCards.Contains(CardToAdd)) {
		TArray<ATCard*> OldInteractableCards = InteractableCards;
		InteractableCards.Add(CardToAdd);

		UpdateInteractableCardsCollisions(OldInteractableCards);
	}
}

void ATPawn::RemoveInteractableCard(ATCard* CardToRemove)
{
	if (InteractableCards.Contains(CardToRemove)) {
		TArray<ATCard*> OldInteractableCards = InteractableCards;
		InteractableCards.Remove(CardToRemove);

		UpdateInteractableCardsCollisions(OldInteractableCards);
	}
}

void ATPawn::MulticastAddInteractableCard_Implementation(ATPawn* Pawn, ATCard* CardToAdd)
{
	if (Pawn == UGameplayStatics::GetPlayerPawn(this, 0))
		AddInteractableCard(CardToAdd);
	else
		RemoveInteractableCard(CardToAdd);
}

void ATPawn::UpdateInteractableCardsCollisions(TArray<class ATCard*> OldInteractableCards)
{
	for (ATCard* Card : OldInteractableCards)
		Card->EnableCardCollision(false);
	
	for (ATCard* Card : InteractableCards)
		Card->EnableCardCollision(true);
}

void ATPawn::ServerChangeHandCardCollisions_Implementation(class ATPawn* StolenCardPawn, class ATPawn* HandCardPawn, class ATCard* StolenCard, class ATCard* HandCard)
{
	ServerChangeHandCard(StolenCardPawn, HandCard, StolenCard);
	ServerChangeHandCard(HandCardPawn, StolenCard, HandCard);

	MulticastChangeHandCardCollisions(StolenCardPawn, HandCardPawn, StolenCard, HandCard);
}

void ATPawn::MulticastChangeHandCardCollisions_Implementation(class ATPawn* StolenCardPawn, class ATPawn* HandCardPawn, class ATCard* StolenCard, class ATCard* HandCard)
{
	if (StolenCardPawn == UGameplayStatics::GetPlayerPawn(this, 0))
		StolenCardPawn->RemoveInteractableCard(StolenCard);
	else if (HandCardPawn == UGameplayStatics::GetPlayerPawn(this, 0))
		HandCardPawn->AddInteractableCard(StolenCard);

	if (HandCardPawn == UGameplayStatics::GetPlayerPawn(this, 0))
		HandCardPawn->RemoveInteractableCard(HandCard);
	else if (StolenCardPawn == UGameplayStatics::GetPlayerPawn(this, 0))
		StolenCardPawn->AddInteractableCard(HandCard);
}

FVector ATPawn::CalculateHandTargetLocation()
{
	uint8 HandNum = Hand.Num() - 1;
	FVector PlayerLocation = GetActorLocation();

	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();

	float ForwardOffset = (HandNum % 2 == 0) ? 65.0f : 85.0f;
	float RightOffset = -7.5f + (HandNum / 2) * 15.0f;

	FVector Offset = ForwardVector * ForwardOffset + RightVector * RightOffset + FVector(0, 0, 38.f + HandNum * 0.2f);

	return PlayerLocation + Offset;
}

FVector ATPawn::CalculateDrawnTargetLocation()
{
	FVector PlayerLocation = GetActorLocation();

	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector Offset = ForwardVector * 110 + RightVector + FVector(0, 0, 38.f);

	return PlayerLocation + Offset;
}

void ATPawn::ClientShowEndWidget_Implementation()
{
	if (UUserWidget* EndWidget = CreateWidget<UUserWidget>(GetWorld(), EndWidgetClass))
		EndWidget->AddToViewport();
}

uint8 ATPawn::GetHandPoints() const
{
	uint8 Points = 0;

	for (ATCard* HandCard : Hand)
		Points += static_cast<uint8>(HandCard->GetCardValue());

	return Points;
}

void ATPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPawn, Hand);
	DOREPLIFETIME(ATPawn, DrawedCard);
	DOREPLIFETIME(ATPawn, JackActivated);
	DOREPLIFETIME(ATPawn, QueenActivated);
	DOREPLIFETIME(ATPawn, KingActivated);
}

void ATPawn::PrintInteractableCards()
{
	UE_LOG(LogTemp, Warning, TEXT("------------------------"));
	for (ATCard* Card : InteractableCards)
		UE_LOG(LogTemp, Warning, TEXT("%s: Interactable card %s"), *GetDebugStringForWorld(GetWorld()), *Card->GetName());
	UE_LOG(LogTemp, Warning, TEXT("------------------------"));
}

void ATPawn::PrintHandCards()
{
	UE_LOG(LogTemp, Warning, TEXT("------------------------"));
	for (ATCard* Card : Hand)
		UE_LOG(LogTemp, Warning, TEXT("%s: Hand card %s"), *GetDebugStringForWorld(GetWorld()), *Card->GetName());
	UE_LOG(LogTemp, Warning, TEXT("------------------------"));
}