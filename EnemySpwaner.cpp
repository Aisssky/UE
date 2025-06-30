
#include "EnemySpawner1.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "spaceship.h"
#include "Engine/World.h"
#include "Enemy.h"
#include "TimerManager.h"


// Sets default values
AEnemySpawner1::AEnemySpawner1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));

	RootComponent = SpawnArea;

	SpawnInterval = 2.0f;
	
	MaxEnemyNum = 30;

	CurrentEnemyCount = 0;
}


// Called when the game starts or when spawned
void AEnemySpawner1::BeginPlay()
{
	Super::BeginPlay();
	
	SpaceShip=Cast<Aspaceship>( UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorldTimerManager().SetTimer(TimerHandle_Spawn,this,&AEnemySpawner1::SpawnEnemy,SpawnInterval,true,0.0f);


}

FVector AEnemySpawner1::GetGenerateLocation()
{
	float Distance=0;//要设置初始值
	FVector Location;
	while (Distance<MinimumDistanceToPlayer)
	{
		Location = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea->Bounds.Origin, SpawnArea->Bounds.BoxExtent);//获得随机生成的点
		Distance = (Location - SpaceShip->GetActorLocation()).Size();
	}
	return Location;
}

void AEnemySpawner1::SpawnEnemy()//生成敌人
{
	if (SpaceShip->GetBDead() == false&&CurrentEnemyCount<MaxEnemyNum)
	{
		FActorSpawnParameters SpawnParameters;
		GetWorld()->SpawnActor<AEnemy>(Enemy, GetGenerateLocation(), FRotator::ZeroRotator, SpawnParameters);
		CurrentEnemyCount++;
	}
}

// Called every frame
void AEnemySpawner1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner1::DecreaseEnemyCount()
{
	if (CurrentEnemyCount > 0) {
		CurrentEnemyCount--;
	}
}

