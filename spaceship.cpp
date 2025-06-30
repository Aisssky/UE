
#include "spaceship.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Misc/App.h"
#include "Engine/World.h"
#include "Bullet.h"
#include "TimerManager.h"
#include "Enemy.h"
#include "Engine/BlockingVolume.h"
#include "Kismet/GamePlayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

 // Sets default values
Aspaceship::Aspaceship()//仅在游戏开始时调用一次
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

	ShipSM = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipSM"));
	ShipSM->SetupAttachment(RootComponent);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(ShipSM);

	ThrusterParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrusterParticleComp"));
	ThrusterParticleComp->SetupAttachment(RootComponent);

	Speed = 2500.0f;
	TimeBetweenShot = 0.2f;
	bDead = false;
}

// Called when the game starts or when spawned
void Aspaceship::BeginPlay()//仅在游戏开始时调用一次
{
	Super::BeginPlay();
	PC = Cast<APlayerController>(GetController());
	PC->bShowMouseCursor=true;//把鼠标显示出来
}


void Aspaceship::LookAtCursor()
{
	FVector MouseLocation, MouseDirection;
	PC->DeprojectMousePositionToWorld(MouseLocation,MouseDirection);
	FVector TargetLocation = FVector(MouseLocation.X, MouseLocation.Y, GetActorLocation().Z);//保证飞机在平面上旋转
	FRotator Rotator=UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	SetActorRotation(Rotator);
}

void Aspaceship::MoveUp(float Value)
{
	if (Value != 0)
	{
		bMoveUp = true;
	}
	else
	{
		bMoveUp = false;
	}
	AddMovementInput(FVector::ForwardVector,Value);
}

void Aspaceship::MoveRight(float Value)
{
	if (Value != 0)
	{
		bMoveRight = true;
	}
	else
	{
		bMoveRight = false;
	}
	AddMovementInput(FVector::RightVector,Value);
}
void Aspaceship::Move()
{
	AddActorWorldOffset(ConsumeMovementInputVector()*Speed* FApp::GetDeltaTime(), true);
}

void Aspaceship::Fire()
{
	if (Bullet&&!bDead)
	{
		FActorSpawnParameters SpawnParams;
		GetWorld()->SpawnActor<ABullet>(Bullet, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), SpawnParams);
		if(ShootCue)
		UGameplayStatics::PlaySoundAtLocation(this,ShootCue, GetActorLocation());
	}
}

void Aspaceship::StartFire()
{
	GetWorldTimerManager().SetTimer(TimerHandle_BetweenShot,this,&Aspaceship::Fire,TimeBetweenShot,true,0.0f);
}

void Aspaceship::EndFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BetweenShot);
}

void Aspaceship::RestartLevel()
{

	UGameplayStatics::OpenLevel(this,"MainMap");
}

void Aspaceship::OnDeath()
{
	//让主角消失，不用Destory
	bDead = true;
	CollisionComp->SetVisibility(false,true);//玩家无法看到主角
	if(GameOverCue)
	UGameplayStatics::PlaySoundAtLocation(this, GameOverCue, GetActorLocation());
	if(ExplosionParticle)
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, GetActorLocation(),FRotator::ZeroRotator,true);
	GetWorldTimerManager().SetTimer(TimerHandle_Reatart, this, &Aspaceship::RestartLevel, 2.0f,false);//每隔两秒调用一次参数，不需要循环false
}


// Called every frame
void Aspaceship::Tick(float DeltaTime)//游戏开始的每一帧都会调用
{
	Super::Tick(DeltaTime);
	if (!bDead)
	{
		if (bMoveUp || bMoveRight)
		{
			ThrusterParticleComp->Activate();
		}
		else
		{
			ThrusterParticleComp->Deactivate();
		}
		LookAtCursor();
		Move();
	}
	else
	{
		ThrusterParticleComp->Deactivate();
	}
}

// Called to bind functionality to input
void Aspaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveUp", this, &Aspaceship::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &Aspaceship::MoveRight);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &Aspaceship::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &Aspaceship::EndFire);
}

void Aspaceship::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	//两个物体里必须有一个物体的碰撞设为overlap
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if (Enemy)
	{
		Enemy->Destroy();
		OnDeath();
	}
}
