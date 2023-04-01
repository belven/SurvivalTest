#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS()
class SURVIVALTEST_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void SetupInputComponent() override;
	void OnPrimaryAction();
	void MoveForward(float Val);
	void MoveRight(float Val);

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
		FOnUseItem OnUseItem;
};