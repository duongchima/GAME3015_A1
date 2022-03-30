#include "Entity.hpp"

Entity::Entity(Game* game) : SceneNode(game), mVelocity(0, 0, 0)
{
}

void Entity::setVelocity(XMFLOAT3 velocity)
{
	mVelocity = velocity;
}

void Entity::setVelocity(float vx, float vy, float vz)
{
	mVelocity.x = vx;
	mVelocity.y = vy;
	mVelocity.z = vz;
}

XMFLOAT3 Entity::getVelocity() const
{
	return mVelocity;
}

void Entity::accelerate(XMFLOAT3 velocity)
{
	mVelocity.x = mVelocity.x + velocity.x;
	mVelocity.y = mVelocity.y + velocity.y;
	mVelocity.z = mVelocity.z + velocity.z;
}

void Entity::updateCurrent(const GameTimer& gt) 
{
	mPosition.x = mVelocity.x * gt.DeltaTime();
	mPosition.y = mVelocity.y * gt.DeltaTime();
	mPosition.z = mVelocity.z * gt.DeltaTime();

	move(mPosition.x, mPosition.y, mPosition.z);
}
