#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TEnums.h"
#include "TPawn.h"
#include "TCard.generated.h"

UCLASS()
class TALMUT_API ATCard : public AActor
{
	GENERATED_BODY()
	
public:
	ATCard();

	virtual void PostInitializeComponents() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInitialize(UTexture* CardTexture, uint8 CardOffset);

	void MoveCard(const FVector& TargetLocation, const FRotator& TargetRotation);
	UFUNCTION(Server, Reliable)
	void ServerMoveCard(const FVector& TargetLocation, const FRotator& TargetRotation);
	UFUNCTION(Server, Reliable)
	void ServerMoveDrawedCard(const FVector& TargetLocation, FRotator TargetRotation, APawn* OwningPawn);
	UFUNCTION(Server, Reliable)
	void ServerMoveDiscardedCard(class ATPawn* Pawn);

	void EnableCardCollision(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Card|Mesh")
	FORCEINLINE class UStaticMeshComponent* GetCardMesh() const { return CardMesh; }

	void SetSelected(bool bSelected) { bIsSelected = bSelected; }

	UFUNCTION(BlueprintCallable, Category = "Card")
	FORCEINLINE EPokerCardValue GetCardValue() const { return CardValue; }

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMoveCard(const FVector& TargetLocation, const FRotator& TargetRotation);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMoveDeckCard(const FVector& TargetLocation, FRotator TargetRotation, APawn* OwningPawn);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMoveDiscardedCard(class ATPawn* Pawn);

protected:
	UPROPERTY(EditAnywhere, Category = "Card")
	float CardMovingVelocity;
	UPROPERTY(EditAnywhere, Category = "Card")
	float CardRotationVelocity;

private:
	UFUNCTION()
	void OnBeginCursorOver(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void OnEndCursorOver(UPrimitiveComponent* TouchedComponent);

	void SetCardTexture(UTexture* CardTexture);
	void SetCardValue(UTexture* CardTexture);

private:
	UPROPERTY(EditAnywhere, Category = "Card|Mesh")
	TObjectPtr<class UStaticMeshComponent> CardMesh;

	EPokerCardValue CardValue;

	FTimerHandle MoveCardTimerHandle;

	bool bIsSelected;
};