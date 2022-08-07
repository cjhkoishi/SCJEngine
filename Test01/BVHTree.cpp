#include "BVHTree.h"


bool cmpx(const BVHTree::BoundingBox& obj1, const BVHTree::BoundingBox& obj2) {
	return obj1.center.x < obj2.center.x;
}
bool cmpy(const BVHTree::BoundingBox& obj1, const BVHTree::BoundingBox& obj2) {
	return obj1.center.y < obj2.center.y;
}





BVHTree::BVHNode* ComputeNode(vector<BVHTree::BoundingBox>& objects, int l, int r, int n) {
	BVHTree::BVHNode* node = new BVHTree::BVHNode();
	node->AA = dvec2(INFINITY);
	node->BB = dvec2(-INFINITY);


	// ���� AABB
	for (int i = l; i <= r; i++) {
		// ��С�� AA
		node->AA.x = std::min(node->AA.x, objects[i].AA.x);
		node->AA.y = std::min(node->AA.y, objects[i].AA.y);
		// ���� BB
		node->BB.x = std::max(node->BB.x, objects[i].BB.x);
		node->BB.y = std::max(node->BB.y, objects[i].BB.y);
	}

	// ������ n �������� ����Ҷ�ӽڵ�
	if ((r - l + 1) <= n) {
		node->n = r - l + 1;
		node->index = l;
		return node;
	}
	//// ����ݹ齨��
	//float lenx = node->BB.x - node->AA.x;
	//float leny = node->BB.y - node->AA.y;
	//// �� x ����
	//if (lenx >= leny)
	//	sort(objects.begin() + l, objects.begin() + r + 1, cmpx);
	//// �� y ����
	//else
	//	sort(objects.begin() + l, objects.begin() + r + 1, cmpy);

	//// �ݹ�
	//int mid = (l + r) / 2;
	//node->left = ComputeNode(objects, l, mid, n);
	//node->right = ComputeNode(objects, mid + 1, r, n);
	//return node;


	float Cost = INFINITY;
	int Axis = 0;
	int Split = (l + r) / 2;
	for (int axis = 0; axis < 2; axis++) {
		// �ֱ� x��y��z ������
		if (axis == 0) std::sort(&objects[0] + l, &objects[0] + r + 1, cmpx);
		if (axis == 1) std::sort(&objects[0] + l, &objects[0] + r + 1, cmpy);

		// leftMax[i]: [l, i] ������ xyz ֵ
		// leftMin[i]: [l, i] ����С�� xyz ֵ
		std::vector<dvec2> leftMax(r - l + 1, dvec2(-INFINITY, -INFINITY));
		std::vector<dvec2> leftMin(r - l + 1, dvec2(INFINITY, INFINITY));
		// ����ǰ׺ ע�� i-l �Զ��뵽�±� 0
		for (int i = l; i <= r; i++) {
			auto& t = objects[i];
			int bias = (i == l) ? 0 : 1;  // ��һ��Ԫ�����⴦��

			leftMax[i - l].x = std::max(leftMax[i - l - bias].x, t.BB.x);
			leftMax[i - l].y = std::max(leftMax[i - l - bias].y, t.BB.y);

			leftMin[i - l].x = std::min(leftMin[i - l - bias].x, t.AA.x);
			leftMin[i - l].y = std::min(leftMin[i - l - bias].y, t.AA.y);
		}

		// rightMax[i]: [i, r] ������ xyz ֵ
		// rightMin[i]: [i, r] ����С�� xyz ֵ
		std::vector<dvec2> rightMax(r - l + 1, dvec2(-INFINITY, -INFINITY));
		std::vector<dvec2> rightMin(r - l + 1, dvec2(INFINITY, INFINITY));
		// �����׺ ע�� i-l �Զ��뵽�±� 0
		for (int i = r; i >= l; i--) {
			auto& t = objects[i];
			int bias = (i == r) ? 0 : 1;  // ��һ��Ԫ�����⴦��

			rightMax[i - l].x = std::max(rightMax[i - l + bias].x, t.BB.x);
			rightMax[i - l].y = std::max(rightMax[i - l + bias].y, t.BB.y);

			rightMin[i - l].x = std::min(rightMin[i - l + bias].x, t.AA.x);
			rightMin[i - l].y = std::min(rightMin[i - l + bias].y, t.AA.y);
		}

		// ����Ѱ�ҷָ�
		float cost = INFINITY;
		int split = l;
		for (int i = l; i <= r - 1; i++) {
			float lenx, leny;
			// ��� [l, i]
			dvec2 leftAA = leftMin[i - l];
			dvec2 leftBB = leftMax[i - l];
			lenx = leftBB.x - leftAA.x;
			leny = leftBB.y - leftAA.y;
			float leftS = 2.0 * (lenx + leny);
			float leftCost = leftS * (i - l + 1);

			// �Ҳ� [i+1, r]
			dvec2 rightAA = rightMin[i + 1 - l];
			dvec2 rightBB = rightMax[i + 1 - l];
			lenx = rightBB.x - rightAA.x;
			leny = rightBB.y - rightAA.y;
			float rightS = 2.0 * (lenx + leny);
			float rightCost = rightS * (r - i);

			// ��¼ÿ���ָ����С��
			float totalCost = leftCost + rightCost;
			if (totalCost < cost) {
				cost = totalCost;
				split = i;
			}
		}
		// ��¼ÿ�������Ѵ�
		if (cost < Cost) {
			Cost = cost;
			Axis = axis;
			Split = split;
		}
	}

	// �������ָ�
	if (Axis == 0) std::sort(&objects[0] + l, &objects[0] + r + 1, cmpx);
	if (Axis == 1) std::sort(&objects[0] + l, &objects[0] + r + 1, cmpy);

	// �ݹ�
	node->left = ComputeNode(objects, l, Split, n);
	node->right = ComputeNode(objects, Split + 1, r, n);

	return node;
}

void BVHTree::BuildTree(vector<BoundingBox>& objects, int n)
{
	root = ComputeNode(objects, 0, objects.size() - 1, n);
}

void BVHTree::RayHitBalls(dvec2 pos, dvec2 dir, vector<int>& indices)
{
	indices.clear();
	function<void(BVHNode*)> search;
	int count = 0;
	search = [&](BVHNode* node) {
		bool res = ray->RayHitAABB(node->AA, node->BB); count++;
		if (res) {
			if (node->left)
				search(node->left);
			if (node->right)
				search(node->right);
			if (!node->left && !node->right) {
				for (int i = 0; i < node->n; i++) {
					int ball_index = AABBs[i + node->index].index;
					auto bpos = container->balls[ball_index].pos;
					auto r = container->balls[ball_index].r;
					bool res = ray->RayHitBall(bpos, r); count++;
					if (res)
						indices.push_back(ball_index);
				}
			}
		}
	};
	search(root);
	cout << count << endl;
}

void BVHTree::start()
{
	unsigned indices[]{
	0,1,
	2,3,
	0,2,
	1,3
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	shader = shader_asset.asset["default"];
}

void BVHTree::update()
{
	if (!is_updated) {
		is_updated = true;
		if (container) {

			auto& data = container->balls;
			AABBs.resize(data.size());
			for (int i = 0; i < data.size(); i++) {
				AABBs[i].AA = data[i].pos - dvec2(data[i].r);
				AABBs[i].BB = data[i].pos + dvec2(data[i].r);
				AABBs[i].center = (AABBs[i].AA + AABBs[i].BB) * 0.5;
				AABBs[i].index = i;
			}
			BuildTree(AABBs, 4);
		}
	}

	vector<int> result;
	if (ray) {
		RayHitBalls(ray->pos, ray->dir, result);
	}
	for (auto& item : result) {
		container->balls[item].activated = true;
	}

	Renderer::update();
}

void BVHTree::render(const mat4& view, const mat4& proj)
{

	shader.use();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);
	shader.setMat4("model", getObject()->getWorldTransform());
	shader.setVec4("color", vec4(0, 1, 0, 1));
	auto drawnode = [](auto&& fun, BVHNode* node, int depth) {
		if (depth > 8 || node->left == NULL && node->right == NULL) {
			vec2 verts[4];
			verts[0].x = verts[2].x = node->AA.x;
			verts[0].y = verts[1].y = node->AA.y;
			verts[1].x = verts[3].x = node->BB.x;
			verts[2].y = verts[3].y = node->BB.y;
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
			glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);
			return;
		}
		fun(fun, node->left, depth + 1);
		fun(fun, node->right, depth + 1);
	};
	drawnode(drawnode, root, 0);
}


