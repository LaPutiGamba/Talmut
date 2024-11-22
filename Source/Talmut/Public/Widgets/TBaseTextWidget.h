#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TBaseTextWidget.generated.h"

UCLASS()
class TALMUT_API UTBaseTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Text", meta=(BindWidget))
	TObjectPtr<class URichTextBlock> BaseText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	FText InputText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	bool bAutoWrapText;

	UPROPERTY(EditAnywhere, Category = "Text")
	TObjectPtr<UDataTable> DefaultStyleSet;

	UPROPERTY(EditAnywhere, Category = "Text")
	TObjectPtr<UDataTable> DyslexicStyleSet;

	UFUNCTION(BlueprintCallable, Category = "Text")
	void UpdateBaseText(FText Text, bool AutoWrapText = false, ETextJustify::Type Justification = ETextJustify::Left) const;
};
