#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

UCLASS()
class SURVIVALTEST_API ABaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	virtual void DrawHUD() override;
	void DrawStats();
	void DrawStatBar(float startX, float startY, float lineOffset, FString text, float value, FLinearColor lineColour, float lineLength, float lineThickness);
};
