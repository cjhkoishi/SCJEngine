#pragma once
#include <Widget.h>
#include "Container.h"


class BVHTree :public Renderer
{
	COMPONENT
public:
	struct BVHNode {
		BVHNode* left = NULL;
		BVHNode* right = NULL;
		int n, index;
		dvec2 AA, BB;
	};
	BVHNode* root = NULL;

	struct BoundingBox {
		dvec2 AA, BB;
		dvec2 center;
		int index;
	};
	vector<BoundingBox> AABBs;
	bool is_updated = false;
	Container* container = NULL;
	Ray* ray = NULL;

	GLuint VAO, VBO, EBO;
	Shader shader;

	void BuildTree(vector<BoundingBox>& objects, int n);
	void RayHitBalls(dvec2 pos, dvec2 dir, vector<int>& indices);

	void start();
	void update();
	void render(const mat4& view, const mat4& proj);
};



