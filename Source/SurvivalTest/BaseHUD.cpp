#include "BaseHUD.h"
#include "BaseCharacter.h"

void ABaseHUD::DrawHUD()
{
	Super::DrawHUD();
	DrawStats();
}

void ABaseHUD::DrawStats()
{
	ABaseCharacter* player = Cast<ABaseCharacter>(GetOwningPawn());
	APlayerController* pc = Cast<APlayerController>(player->GetController());

	int32 vpX;
	int32 vpY;
	pc->GetViewportSize(vpX, vpY);

	float startX = vpX * 0.05;
	float startY = vpY * 0.9;

	DrawStatBar(startX, startY, 60, "Health", player->GetCurrentStats().health / player->GetMaxStats().health, FLinearColor::Red, 100, 15);
	DrawStatBar(startX, startY + 20, 60, "Water", player->GetCurrentStats().water / player->GetMaxStats().water, FLinearColor::Blue, 100, 15);
	DrawStatBar(startX, startY + 40, 60, "Hunger", player->GetCurrentStats().hunger / player->GetMaxStats().hunger, FLinearColor::Yellow, 100, 15);
	DrawStatBar(startX, startY + 60, 60, "Rest", player->GetCurrentStats().rest / player->GetMaxStats().rest, FLinearColor::White, 100, 15);
}

void ABaseHUD::DrawStatBar(float startX, float startY, float lineOffset, FString text, float value, FLinearColor lineColour, float lineLength, float lineThickness)
{
	TArray< FStringFormatArg > args;
	args.Add(FStringFormatArg(text));
	float lineX = startX;
	float lineY = startY;

	DrawText(FString::Format(TEXT("{0}"), args), FLinearColor::Black, startX, startY);

	// Offset to the right
	lineX += lineOffset;
	lineY += lineThickness / 2;

	DrawLine(lineX, lineY, lineX + lineLength, lineY, FLinearColor::Gray, lineThickness);

	if (value > 0)
		DrawLine(lineX, lineY, lineX + (lineLength * value), lineY, lineColour, lineThickness);
}