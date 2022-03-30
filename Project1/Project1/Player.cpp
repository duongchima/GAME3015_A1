#include "Player.h"
#include "CommandQueue.h"
#include "Aircraft.hpp"
#include "../../Common/MathHelper.h"
#include "../../Common/d3dApp.h"
#include <map>
#include <string>
#include <algorithm>
#include <stdio.h>

using namespace DirectX;

struct AircraftMover
{
	AircraftMover(float vx, float vy, float vz)
		: velocity(vx, vy, vz)
	{
	}

	void operator() (Aircraft& aircraft, const GameTimer&) const
	{
		aircraft.accelerate(velocity);
	}

	XMFLOAT3 velocity;
};

Player::Player()
{
	mKeyBinding[VK_LBUTTON] = MoveLeft;
	mKeyBinding[VK_RBUTTON] = MoveRight;
	mKeyBinding[VK_UP] = MoveUp;
	mKeyBinding[VK_DOWN] = MoveDown;

	initializeActions();

	for (auto pair : mKeyBinding)
		mKeyFlag[pair.first] = false;

	for (auto& pair : mActionBinding)
		pair.second.category = Category::PlayerAircraft;
}

void Player::handleEvent(CommandQueue& commands)
{
	for (auto pair : mKeyBinding)
	{
		if (!isRealtimeAction(pair.second))
		{
			if (mKeyFlag[pair.first])
			{
				if (!GetAsyncKeyState(pair.first))
				{
					mKeyFlag[pair.first] = false; // RELEASED
				}
			}
			else
			{
				if (GetAsyncKeyState(pair.first) & 0x8000)
				{
					mKeyFlag[pair.first] = true; // PRESSED
					commands.push(mActionBinding[pair.second]);
				}
			}
		}
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	for (auto pair : mKeyBinding) {
		if (GetAsyncKeyState(pair.first) & 0x8000 && isRealtimeAction(pair.second))
			commands.push(mActionBinding[pair.second]);
	}
}

void Player::assignKey(Action action, char key)
{
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); ) {
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}
	mKeyBinding[key] = action;
}

char Player::getAssignedKey(Action action) const
{
	for (auto pair : mKeyBinding) {
		if (pair.second == action)
			return pair.first;
	}
	return 0x00;
}

void Player::initializeActions()
{
	// initialize actions here
	const float playerSpeed = 10.f;

	mActionBinding[MoveLeft].action = derivedAction<Aircraft>(AircraftMover(-playerSpeed, 0.f, 0.0f));
	mActionBinding[MoveRight].action = derivedAction<Aircraft>(AircraftMover(+playerSpeed, 0.f, 0.0f));
	mActionBinding[MoveUp].action = derivedAction<Aircraft>(AircraftMover(0.f, 0.0f, +playerSpeed));
	mActionBinding[MoveDown].action = derivedAction<Aircraft>(AircraftMover(0.f, 0.0f, -playerSpeed));
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveLeft:
	case MoveRight:
	case MoveDown:
	case MoveUp:
		return true;

	default:
		return false;
	}
}
