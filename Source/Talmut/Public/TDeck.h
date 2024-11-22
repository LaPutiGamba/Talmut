#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDeck.generated.h"

UCLASS()
class TALMUT_API ATDeck : public AActor
{
	GENERATED_BODY()
	
public:	
	ATDeck();

	virtual void PostInitializeComponents() override;

	void EnableBoxCollision(bool bEnable);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginCursorOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	virtual void OnEndCursorOver(UPrimitiveComponent* TouchedComponent);

protected:
	TObjectPtr<class ATGameState> TGameState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> DeckCollision;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USceneComponent> SceneRootComponent;
};
