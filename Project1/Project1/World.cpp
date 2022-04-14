#define NOMINMAX
#include "World.hpp"

World::World(State* state)
	: mSceneGraph(new SceneNode(state))
	, mState(state)
	, mPlayerAircraft(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-4.25f, 4.25f, -3.0f, 3.0f) //Left, Right, Down, Up
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}

void World::update(const GameTimer& gt)
{
	mPlayerAircraft->setVelocity(0, 0, 0);
	while (!mCommandQueue.isEmpty())
		mSceneGraph->onCommand(mCommandQueue.pop(), gt);

	mSceneGraph->update(gt);

	if (mPlayerAircraft->getVelocity().y > 0) mPlayerAircraft->setWorldRotation(-1, 0, 0);

	else if (mPlayerAircraft->getVelocity().y < 0) mPlayerAircraft->setWorldRotation(1, 0, 0);

	else if (mPlayerAircraft->getVelocity().x > 0) mPlayerAircraft->setWorldRotation(0, 0, -1);

	else if (mPlayerAircraft->getVelocity().x < 0) mPlayerAircraft->setWorldRotation(0, 0, 1);

	else if (mPlayerAircraft->getVelocity().y == 0 || mPlayerAircraft->getVelocity().x == 0)
		mPlayerAircraft->setWorldRotation(0, 0, 0);

	for (int i = 0; i < totalEnemies; i++)
	{
		if (mEnemy[i]->getVelocity().y > 0) mEnemy[i]->setWorldRotation(-1, 135, 0);

		else if (mEnemy[i]->getVelocity().y < 0) mEnemy[i]->setWorldRotation(1, 135, 0);

		else if (mEnemy[i]->getVelocity().y == 0)
			mEnemy[i]->setWorldRotation(0, 0, 0);
	}


	if (mPlayerAircraft->getWorldPosition().x < -maxWidth)
		mPlayerAircraft->setPosition(-maxWidth, mPlayerAircraft->getWorldPosition().y, mPlayerAircraft->getWorldPosition().z);

	else if (mPlayerAircraft->getWorldPosition().x > maxWidth)
		mPlayerAircraft->setPosition(maxWidth, mPlayerAircraft->getWorldPosition().y, mPlayerAircraft->getWorldPosition().z);

	if (mPlayerAircraft->getWorldPosition().y < minHeight)
		mPlayerAircraft->setPosition(mPlayerAircraft->getWorldPosition().x, minHeight, mPlayerAircraft->getWorldPosition().z);

	else if (mPlayerAircraft->getWorldPosition().y > maxHeight)
		mPlayerAircraft->setPosition(mPlayerAircraft->getWorldPosition().x, maxHeight, mPlayerAircraft->getWorldPosition().z);

	for (int i = 0; i < totalEnemies; i++)
	{
		if (mEnemy[i]->getWorldPosition().x < -maxWidth || mEnemy[i]->getWorldPosition().x > maxWidth)
		{
			mEnemy[i]->setVelocity(-mEnemy[i]->getVelocity().x, mEnemy[i]->getVelocity().y, mEnemy[i]->getVelocity().z);
		}

		if (mEnemy[i]->getWorldPosition().y < minHeight || mEnemy[i]->getWorldPosition().y > maxHeight)
		{
			mEnemy[i]->setVelocity(mEnemy[i]->getVelocity().x, -mEnemy[i]->getVelocity().y, mEnemy[i]->getVelocity().z);
		}
	}
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

void World::draw()
{
	mSceneGraph->draw();
}

void World::buildScene()
{
	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Type::Eagle, mState));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(0, 0, -10);
	mPlayerAircraft->setScale(3.0, 3.0, 3.0);
	mPlayerAircraft->setVelocity(2.5f, 2.0f, 0.0f);
	mSceneGraph->attachChild(std::move(player));

	for (int i = 0; i < totalEnemies; i++)
	{
		std::unique_ptr<Aircraft> enemy1(new Aircraft(Aircraft::Type::Raptor, mState));
		mEnemy[i] = enemy1.get();
		mEnemy[i]->setPosition(mPlayerAircraft->getWorldPosition().x * i, (rand() % 5) + 5, mPlayerAircraft->getWorldPosition().z + 10.0f);
		mEnemy[i]->setScale(3.0, 3.0, 3.0);
		mEnemy[i]->setVelocity(2.f * i + i, 2.0f, 0.0f);
		mSceneGraph->attachChild(std::move(enemy1));
	}

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(mState));
	backgroundSprite->SetMatGeoDrawName("Desert", "boxGeo", "box");
	mBackground = backgroundSprite.get();
	//mBackground->setPosition(mWorldBounds.left, mWorldBounds.top);
	mBackground->setPosition(0, -30.0, 0.0);
	mBackground->setScale(200.0, 1.0, 200.0);
	mBackground->setVelocity(0, 0, -mScrollSpeed);
	mBackground->setWorldRotation(20.0, 0.0, 0.0);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	mSceneGraph->build();
}
