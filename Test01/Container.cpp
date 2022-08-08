#include "Container.h"

double random() {
	return (double)rand() / RAND_MAX;
}

void Container::start()
{
	shader = shader_asset["default"];

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	double vert[50 * 2];

	for (int i = 0; i < 50; i++) {
		double theta = i * 2 * pi<double>() / 50;
		vert[2 * i] = cos(theta) * 5;
		vert[2 * i + 1] = sin(theta) * 5;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);
	glEnableVertexAttribArray(0);

	balls.resize(500);
	for (auto& item : balls) {
		item.pos[0] = random() * 1000;
		item.pos[1] = random() * 1000;
	}
}

void Container::render(const mat4& view, const mat4& proj)
{
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);

	mat4 model = getObject()->getWorldTransform();
	for (auto& item : balls) {
		mat4 trans(1);
		trans[3] = vec4(item.pos, 0, 1);
		shader.setMat4("model", model * trans);
		if (item.activated) {
			shader.setVec4("color", vec4(1, 0.5, 0.2, 1));
			glDrawArrays(GL_TRIANGLE_FAN, 0, 50);
			item.activated = false;

		}
		else {
			shader.setVec4("color", vec4(1, 1, 1, 1));
			glDrawArrays(GL_LINE_LOOP, 0, 50);
		}
	}

}

bool Ray::RayHitAABB(dvec2 AA, dvec2 BB, double* t)
{
	dvec2 DAA = AA - pos;
	dvec2 DBB = BB - pos;
	dvec2 tA = DAA / dir;
	dvec2 tB = DBB / dir;
	dvec2 tMin = min(tA, tB);
	dvec2 tMax = max(tA, tB);
	float tA_max = std::max(tMin.x, tMin.y);
	float tB_min = std::min(tMax.x, tMax.y);
	bool res = tA_max <= tB_min ? (tB_min >= 0) : false;
	if (res && t) {
		*t = tA_max >= 0 ? tA_max : tB_min;
	}
	return res;
}

bool Ray::RayHitBall(dvec2 bpos, double r)
{
	dvec2 delta = bpos - pos;
	double dist = abs(determinant(mat2(delta, dir)) / length(dir));
	return dist <= r;
}

void Ray::start()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	shader = shader_asset["default"];
}

void Ray::onGui()
{
	float buf = atan2(dir.y, dir.x);
	ImGui::SliderAngle("angle", &buf,-90,90);
	dir = vec2(cos(buf), sin(buf));
}

void Ray::render(const mat4& view, const mat4& proj)
{
	shader.use();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);

	vec2 verts[2] = { pos,pos + dir * 10000.0 };
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);
	shader.setMat4("model", getObject()->getWorldTransform());
	shader.setVec4("color", vec4(1, 1, 0.5, 1));
	glDrawArrays(GL_LINES, 0, 2);
}
