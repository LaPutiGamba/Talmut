#include "Widgets/TBaseTextWidget.h"
#include "TGameUserSettings.h"
#include "Components/RichTextBlock.h"

void UTBaseTextWidget::UpdateBaseText(FText Text, bool AutoWrapText, ETextJustify::Type Justification) const
{
	if (BaseText) {
		BaseText->SetText(Text);
		BaseText->SetAutoWrapText(AutoWrapText);
		BaseText->SetJustification(Justification);

		if (auto UserSettings = UTGameUserSettings::GetTGameUserSettings()) {
			if (UserSettings->IsDyslexicModeOn())
				BaseText->SetTextStyleSet(DyslexicStyleSet);
			else
				BaseText->SetTextStyleSet(DefaultStyleSet);
		}
	}
}
