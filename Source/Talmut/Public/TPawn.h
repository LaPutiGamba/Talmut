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
	void AddCardToHand(class ATCard* Card) { Hand.Add(Card); }

	TObjectPtr<class ATCard> GetSelectedCard() const { return SelectedCard; }
	TObjectPtr<class ATCard> GetDrawedCard() const { return DrawedCard; }

	void IncreaseJackActivated() { JackActivated++; }
	void IncreaseQueenActivated() { QueenActivated++; }
	UFUNCTION(Client, Reliable)
	void ClientStartQueenActivated();
	void IncreaseKingActivated() { KingActivated++; }
	UFUNCTION(Client, Reliable)
	void ClientStartKingActivated();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerStartGame();
	UFUNCTION(Server, Reliable)
	void ServerShowInitialCards();

	void SelectCard(class ATCard* CardToSelect, bool bSelectIt);

private:
	void OnMouseClick();
	void OnPlayerPressSpace();

	UFUNCTION(Server, Reliable)
	void ServerGetDeckCard();
	UFUNCTION(Server, Reliable)
	void ServerGetDiscardCard();
	UFUNCTION(Server, Reliable)
	void ServerGetPenaltyCard();

	bool ExecuteCardAbility(class ATCard* Card);
	void FlipCard(class ATCard* Card);
	UFUNCTION(Client, Reliable)
	void ClientEnableCardCollision(class ATCard* Card, bool bEnable);
	UFUNCTION(Server, Reliable)
	void ServerMoveCards(class ATCard* CardToMove, const FVector& TargetLocation, const FRotator& TargetRotation);

	UFUNCTION(Server, Reliable)
	void ServerAddDrawedCard(class ATCard* CardToAdd, class ATCard* CardToRemove);

	UFUNCTION(Server, Reliable)
	void ServerCallTalmut();

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
};