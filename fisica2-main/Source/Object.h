#define OBJECT_PHYSICAL_SIZE	8
#define OBJECT_FRAME_SIZE		8


#define POINTS_OBJECT	20

enum class ObjectType { FRUIT_1, FRUIT_2, FRUIT_3, OBJECT_1, OBJECT_2 };

class Object
{
public:
	Object(ObjectType t);
	~Object();

	int Points() const;
	ObjectType GetID() const;

public:

	ObjectType type;

};