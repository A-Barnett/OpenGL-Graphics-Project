# pragma once
#include "..\nclgl\scenenode.h"

class CubeRobot : public SceneNode {
public:
	CubeRobot(Mesh* cube,int x,int y,int z);
	~CubeRobot(void) {};
	void Update(float dt) override;

protected:
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;
	SceneNode* leftLeg;
	SceneNode* rightLeg;
};
