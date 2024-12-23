#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TPawn.generated.h"

UCLASS()
class TALMUT_API ATPawn : public APawn
{
	GENERATED_BODY()

public:
	ATPawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Client, Reliable)
	void ClientStartTurn();
	UFUNCTION(Client, Reliable)
	void ClientEndTurn();
	UFUNCTION(Server, Reliable)
	void ServerEndTurn();

	FVector CalculateHandTargetLocation();
	FVector CalculateDrawnTargetLocation();

	TArray<TObjectPtr<class ATCard>> GetHand() const { return Hand; }
	void AddCardToHand(class ATCard* Card) { if (!Hand.Contains(Card)) Hand.Add(Card); }
	void RemoveCardFromHand(class ATCard* Card) { if (Hand.Contains(Card)) Hand.Remove(Card); }

	void IncreaseJackActivated() { JackActivated++; }
	void IncreaseQueenActivated() { QueenActivated++; }
	UFUNCTION(Client, Reliable)
	void ClientStartQueenActivated();
	void IncreaseKingActivated() { KingActivated++; }
	UFUNCTION(Client, Reliable)
	void ClientStartKingActivated();

	void AddInteractableCard(ATCard* CardToAdd);
	void RemoveInteractableCard(ATCard* CardToRemove);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddInteractableCard(ATPawn* Pawn, ATCard* CardToAdd);

	UFUNCTION(Client, Reliable)
	void ClientShowEndWidget();

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	uint8 GetHandPoints() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerStartGame();
	UFUNCTION(Server, Reliable)
	void ServerShowInitialCards();

	void SelectCard(class ATCard* CardToSelect, bool bSelectIt);

	UFUNCTION(Exec)
	void PrintInteractableCards();
	UFUNCTION(Exec)
	void PrintHandCards();

private:
	void OnMouseClick();
	void OnPlayerPressSpace();

	UFUNCTION(Server, Reliable)
	void ServerGetDeckCard(class ATPawn* Pawn);
	UFUNCTION(Server, Reliable)
	void ServerGetDiscardCard(class ATPawn* Pawn);
	UFUNCTION(Server, Reliable)
	void ServerGetPenaltyCard(class ATPawn* Pawn);

	bool ExecuteCardAbility(class ATCard* Card);
	void FlipCard(class ATCard* Card);

	UFUNCTION(Server, Reliable)
	void ServerMoveCards(class ATCard* CardToMove, const FVector& TargetLocation, const FRotator& TargetRotation);

	UFUNCTION(Server, Reliable)
	void ServerChangeHandCard(class ATPawn* PawnToModify, class ATCard* CardToAdd, class ATCard* CardToRemove);

	UFUNCTION(Server, Reliable)
	void ServerCallTalmut();

	void UpdateInteractableCardsCollisions(TArray<class ATCard*> OldInteractableCards);

	UFUNCTION(Server, Reliable)
	void ServerChangeHandCardCollisions(class ATPawn* StolenCardPawn, class ATPawn* HandCardPawn, class ATCard* StolenCard, class ATCard* HandCard);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastChangeHandCardCollisions(class ATPawn* StolenCardPawn, class ATPawn* HandCardPawn, class ATCard* StolenCard, class ATCard* HandCard);

private:
	UPROPERTY(EditAnywhere, Category = "Pawn|Input")
	TObjectPtr<class UInputMappingContext> DefaultMapping;

	UPROPERTY(EditAnywhere, Category = "Pawn|Input")
	TObjectPtr<class UInputAction> ActionMouseClick;

	UPROPERTY(EditAnywhere, Category = "Pawn|Input")
	TObjectPtr<class UInputAction> ActionSpace;

	UPROPERTY()
	TObjectPtr<class ATGameState> TGameState;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<class ATCard>> Hand;
	TArray<TObjectPtr<class ATCard>> InteractableCards;

	UPROPERTY(Replicated)
	TObjectPtr<class ATCard> DrawedCard;
	TObjectPtr<class ATCard> SelectedCard;

	bool bIsDiscardedCard;

	UPROPERTY(Replicated)
	uint8 JackActivated;
	UPROPERTY(Replicated)
	uint8 QueenActivated;
	UPROPERTY(Replicated)
	uint8 KingActivated;
	TObjectPtr<class ATCard> KingHandCard;
	TObjectPtr<class ATCard> KingStolenCard;

	UPROPERTY(EditAnywhere, Category = "Pawn|Widgets")
	TSubclassOf<class UUserWidget> EndWidgetClass;
};