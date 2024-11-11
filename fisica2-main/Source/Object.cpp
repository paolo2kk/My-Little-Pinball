#include "Object.h"


Object::Object(ObjectType t)
{
	
}
Object::~Object()
{
}

int Object::Points() const
{
	if (type == ObjectType::FRUIT_1)		return POINTS_OBJECT;
	else if (type == ObjectType::FRUIT_2)	return POINTS_OBJECT;
	else if (type == ObjectType::FRUIT_3)	return POINTS_OBJECT;
	else if (type == ObjectType::OBJECT_1)	return POINTS_OBJECT;
	else if (type == ObjectType::OBJECT_2)	return POINTS_OBJECT;
	else
	{
		
		return 0;
	}
}

ObjectType Object::GetID() const
{
	if (type == ObjectType::FRUIT_1)		return ObjectType::FRUIT_1;
	else if (type == ObjectType::FRUIT_2)	return ObjectType::FRUIT_2;
	else if (type == ObjectType::FRUIT_3)	return ObjectType::FRUIT_3;
	else if (type == ObjectType::OBJECT_1)	return ObjectType::OBJECT_1;
	else if (type == ObjectType::OBJECT_2)	return ObjectType::OBJECT_2;

}
