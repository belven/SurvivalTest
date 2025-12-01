#pragma once
#include "CoreMinimal.h"
#include "CharacterTask.h"
#include "ReloadTask.generated.h"

class UAnimationAction;
class UReloadAction;

UCLASS()
class SURVIVALTEST_API UReloadTask : public UCharacterTask
{
	GENERATED_BODY()

public:
	virtual void PerformTask(AController* inController) override;
	UReloadTask();

private:
	bool CheckForReload();

	UPROPERTY()
	UReloadAction* reloadAction;

	UPROPERTY()
	UAnimationAction* animationAction;

};
