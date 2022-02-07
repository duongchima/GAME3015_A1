#pragma once
#include "Entity.hpp"
#include <string>

class Aircraft :
    public Entity
{
public:
	enum Type
	{
		Eagle,
		Raptor,
	};
	enum AircraftType
	{
		Player,
		Enemy,
		Total_Num_Types
	};


public:
	Aircraft(Type type, AircraftType aircraftType, Game* game);


private:
	virtual void		drawCurrent() const;
	virtual void		updateCurrent(); 
	virtual void		buildCurrent();


private:
	Type				mType;
	AircraftType		mAircraftType;
	std::string			mSprite;
};
