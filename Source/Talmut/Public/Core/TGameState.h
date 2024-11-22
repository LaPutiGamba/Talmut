#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TEnums.h"
#include "TGameState.generated.h"

UCLASS()
class TALMUT_API ATGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATGameState();

	void SpawnDeckAndDiscardPile();
	void InitializeDeck();

	void StartGame();
	void DealInitialCards();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowInitialCards();

	void StartPlayerTurn();
	void EndPlayerTurn();

	UFUNCTION(BlueprintCallable, Category = "GameState|Pawns")
	FORCEINLINE TArray<class ATPawn*>& GetPawns() { return Pawns; }

	UFUNCTION(BlueprintCallable, Category = "GameState|DeckCards")
	FORCEINLINE TArray<class ATCard*>& GetDeckCards() { return DeckCards; }
	UFUNCTION(BlueprintCallable, Category = "GameState|DeckCards")
	FORCEINLINE TArray<class ATCard*>& GetDiscardCards() { return DiscardCards; }

	UFUNCTION(Server, Reliable)
	void ServerSetStateOfTheGame(EStateOfTheGame NewState);
	FORCEINLINE EStateOfTheGame GetStateOfTheGame() const { return StateOfTheGame; }

	bool IsPlayerTurn(class ATPawn* Pawn);

	UFUNCTION(Server, Reliable)
	void ServerCallTalmut(ATPawn* ATPawn);
	UFUNCTION(Server, Reliable)
	void ServerShowAllPlayerHands();

public:
	UPROPERTY(Replicated)
	TArray<class ATPawn*> Pawns;

	UPROPERTY(Replicated)
	TObjectPtr<class ATDeck> Deck;
	UPROPERTY(Replicated)
	TObjectPtr<class ATDiscardPile> DiscardPile;

protected:
	UPROPERTY(Replicated)
	TArray<class ATCard*> DeckCards;
	UPROPERTY(Replicated)
	TArray<class ATCard*> DiscardCards;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetStateOfTheGame(EStateOfTheGame NewState);

private:
	EStateOfTheGame StateOfTheGame;

	UPROPERTY(EditAnywhere, Category = "DeckCards")
	TSubclassOf<class ATCard> CardClass;
	UPROPERTY(EditAnywhere, Category = "DeckCards")
	TSubclassOf<class ATDeck> DeckClass;
	UPROPERTY(EditAnywhere, Category = "DeckCards")
	TSubclassOf<class ATDiscardPile> DiscardPileClass;
	UPROPERTY(EditAnywhere, Category = "DeckCards")
	TArray<TObjectPtr<UTexture>> CardTextures;

	UPROPERTY(Replicated)
	uint8 CurrentPlayerIndex;

	UPROPERTY(Replicated)
	int8 LastPlayerIndex;
};