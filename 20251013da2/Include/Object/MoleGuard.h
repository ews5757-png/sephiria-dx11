#pragma once
#include "MonsterBase.h"

class CMoleGuard : public CMonsterBase
{
	friend class CScene;

protected:
	CMoleGuard();
	CMoleGuard(const CMoleGuard& Obj);
	CMoleGuard(CMoleGuard&& Obj);
	virtual ~CMoleGuard();



public:
	virtual bool Init() override;
	virtual void Update(float Deltatime) override;


	virtual void OnDamage(const FDamageInfo& Info);

	virtual void Death();

protected:
	void InitLine();
	void TriggerAttack(); 
	virtual void SetTarget(const FVector3D& pos, class CColliderBase* obj) override;

	virtual void AI(float DeltaTime) override;
	virtual void RandomAction(float DeltaTime) override;


	virtual void StartIdle() override;
	virtual void UpdateIdle(float DeltaTime) override;
	virtual void EndIdle() override;

	virtual void StartWander() override;
	virtual void UpdateWander(float DeltaTime) override;
	virtual void EndWander() override;

	virtual void StartChase() override;
	virtual void UpdateChase(float DeltaTime) override;
	virtual void EndChase() override;

	virtual void StartConfront() override;
	virtual void UpdateConfront(float DeltaTime) override;
	virtual void EndConfront() override;

	virtual void StartAttackReady() override;
	virtual void UpdateAttackReady(float DeltaTime) override;
	virtual void EndAttackReady() override;

	virtual void StartAttack() override;
	virtual void UpdateAttack(float DeltaTime) override;
	virtual void EndAttack() override;

	virtual void StartRecovery() override;
	virtual void UpdateRecovery(float DeltaTime) override;
	virtual void EndRecovery() override;

	virtual void StartDelay() override;
	virtual void UpdateDelay(float DeltaTime) override;
	virtual void EndDelay() override;

	virtual void StartHit() override;
	virtual void UpdateHit(float DeltaTime) override;
	virtual void EndHit() override;

	virtual void StartDeath() override;
	virtual void UpdateDeath(float DeltaTime) override;
	virtual void EndDeath() override;

	virtual void StartHitEffect() override;
	virtual void UpdateHitEffect(float DeltaTime) override;
	virtual void EndHitEffect() override;

	virtual void StartDeathEffect() override;
	virtual void UpdateDeathEffect(float DeltaTime) override;
	virtual void EndDeathEffect() override;

};