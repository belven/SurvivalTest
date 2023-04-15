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
	ABaseCharacter* GetPlayer() const { return player; }

	UFUNCTION(BlueprintCallable)
	void SetPlayer(ABaseCharacter* inPlayer) { player = inPlayer; }

private:
	UPROPERTY()
	ABaseCharacter* player;
};
