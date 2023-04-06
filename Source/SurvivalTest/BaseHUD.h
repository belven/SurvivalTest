// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALTEST_API ABaseHUD : public AHUD
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
		virtual void DrawHUD() override;
	void DrawStats();
	void DrawStatBar(int32 startX, int32 startY, int32 lineOffset, FString text, FLinearColor lineColour,
	                 int32 lineLength,
	                 int32 lineThickness);
	void DrawStatBar(float startX, float startY, float lineOffset, FString text, float value,
	                 FLinearColor lineColour, float lineLength, float lineThickness);
};
