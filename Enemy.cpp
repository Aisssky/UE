
#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "spaceship.h"
#include "Kismet/KismetMathLibrary.h"
#include "ShipGameMode.h"
#include "EnemySpawner1.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	ShipSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipSM"));
	ShipSM->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	SpaceShip=Cast<Aspaceship>(UGameplayStatics::GetPlayerPawn(this,0));//强转
	SetColor();

	TArray<AActor*> EnemySpawnerArray;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemySpawner1::StaticClass(), EnemySpawnerArray);
	EnemySpawner1 = Cast<AEnemySpawner1>(EnemySpawnerArray[0]);

	MyGameMode=Cast<AShipGameMode>(UGameplayStatics::GetGameMode(this));
}

void AEnemy::MoveTowardPlayer(float DeltaTime)
{
	FVector Direction = (SpaceShip->GetActorLocation() - GetActorLocation()).GetSafeNormal();//获得方向
	AddActorWorldOffset(Direction * Speed * DeltaTime, true);//阻挡移动
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), SpaceShip->GetActorLocation()));//控制旋转
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(SpaceShip->GetBDead()==false)
	MoveTowardPlayer(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::OnDeath1()
{
	MyGameMode->IncreaseScore();
	EnemySpawner1->DecreaseEnemyCount();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(), FRotator::ZeroRotator, true);
	Destroy();
}
