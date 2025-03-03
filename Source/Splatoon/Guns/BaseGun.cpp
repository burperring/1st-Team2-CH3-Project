#include "BaseGun.h"

#include "Kismet/GameplayStatics.h"

ABaseGun::ABaseGun()
{
	PrimaryActorTick.bCanEverTick = false;

	/* 컴포넌트 초기화 */
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(GetRootComponent());
	
	FrontOfGun = CreateDefaultSubobject<USceneComponent>(TEXT("FrontOfGun"));
	FrontOfGun->SetupAttachment(GetRootComponent());

	/* Fire 초기화 */
	FireMode = EFireMode::FullAuto;
	FireBulletInterval = 0.1f;

	/* Reload 초기화 */
	ReloadBulletInterval = 0.1f;

	/* Bullets 초기화 */
	MaxRemainingBullets = 20;
	RemainingBullets = MaxRemainingBullets;
}

void ABaseGun::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseGun::FirePressed()
{
	Fire();
}

void ABaseGun::FireReleased()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void ABaseGun::ReloadStart()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			ReloadTimerHandle,
			this,
			&ABaseGun::Reload,
			ReloadBulletInterval,
			true
		);
	}
}

void ABaseGun::ReloadStop()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
}

void ABaseGun::Fire()
{
	// 1. 남은 탄환 확인
	if (RemainingBullets <= 0) return;

	// 2. 탄환 감소
	RemainingBullets -= 1;

	// 3. 탄환 생성
	if (UWorld* World = GetWorld())
	{
		World->SpawnActor<AActor>(
			BulletClass,
			FrontOfGun->GetComponentLocation(),
			FrontOfGun->GetComponentRotation()
		);
	}

	// 4. 연사 모드일 경우 타이머 설정
	if (FireMode == EFireMode::FullAuto)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer
			(
				FireTimerHandle,
				this,
				&ABaseGun::Fire,
				FireBulletInterval
			);
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Fire / RemainingBullets = %d"), RemainingBullets));
}

void ABaseGun::Reload()
{
	if (RemainingBullets >= MaxRemainingBullets) return;
	
	RemainingBullets += 1;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Reload / RemainingBullets = %d"), RemainingBullets));
}
