#pragma once

#include "CoreMinimal.h"
#include "TDeck.h"
#include "TDiscardPile.generated.h"

UCLASS()
class TALMUT_API ATDiscardPile : public ATDeck
{
	GENERATED_BODY()

public:
	void AddCardToDiscardPile(class ATCard* Card);

protected:
	virtual void BeginPlay() override;

	virtual void OnBeginCursorOver(UPrimitiveComponent* TouchedComponent) override;
	virtual void OnEndCursorOver(UPrimitiveComponent* TouchedComponent) override;
};