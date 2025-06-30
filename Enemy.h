#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Enemy.generated.h"


class USphereComponent;
class Aspaceship;
class AShipGameMode;
class AEnemySpawner1;

UCLASS()
class SPACESHIP03_API AEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemy();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Component")
	UStaticMeshComponent * ShipSM;//用来显示样子

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveTowardPlayer(float DeltaTime);//向主角移动

	Aspaceship* SpaceShip;

	float Speed = 300.0f;

	AShipGameMode* MyGameMode;

	AEnemySpawner1* EnemySpawner1;

	UFUNCTION(BlueprintImplementableEvent)//蓝图可拓展
	void SetColor();

	UPROPERTY(EditAnywhere, Category = "Particle");
	UParticleSystem* ExplosionParticle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnDeath1();

};
