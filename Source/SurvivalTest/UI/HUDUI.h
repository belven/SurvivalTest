#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDUI.generated.h"

UCLASS()
class SURVIVALTEST_API UHUDUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	float GetHealthProgress();

	UFUNCTION(BlueprintCallable)
	float GetWaterProgress();

	UFUNCTION(BlueprintCallable)
	float GetFoodProgress();

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponMaxAmmo();

	UFUNCTION(BlueprintCallable)
	FString GetWeaponText();

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponCurrentAmmo();

	UFUNCTION(BlueprintCallable)
	float GetRestProgress();

	UFUNCTION(BlueprintCallable)
	float GetStaminaProgress();
	
	UFUNCTION(BlueprintCallable)
	ABaseCharacter* GetPlayer() const { return player; }

	UFUNCTION(BlueprintCallable)
	void SetPlayer(ABaseCharacter* inPlayer) { player = inPlayer; }

private:
	UPROPERTY()
	ABaseCharacter* player;
};
