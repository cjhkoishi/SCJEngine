#include "Canvas.h"
#define N  50000
#define STRINGIZE(a) #a
void Cloud::start()
{
	shader = shader_asset["default"];
	kernel = compute_shader_asset["test"];
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	float test[N * 4];
	auto random = []() {
		return (float)rand() / RAND_MAX;
	};
	for (int i = 0; i < N; i++) {
		test[4 * i] = random() * 100 - 50;
		test[4 * i + 1] = random() * 100 - 50;
		test[4 * i + 2] = random() * 100 - 50;
		test[4 * i + 3] = 1;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(test), test, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenTextures(1, &TBO);
	glBindTexture(GL_TEXTURE_BUFFER, TBO);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, VBO);

}

void Cloud::render(const mat4& view, const mat4& proj)
{
	kernel.use();
	kernel.setFloat("a", a);
	kernel.setFloat("b", b);
	kernel.setFloat("c", c);
	glBindImageTexture(0, TBO, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	if (is_running)
		glDispatchCompute(N, 1, 1);


	shader.use();
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);
	glPointSize(1);
	shader.setMat4("model", getObject()->getWorldTransform());
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);
	shader.setVec4("color", vec4(1, 1, 1, 1));
	glDrawArrays(GL_POINTS, 0, N);
}

void Cloud::onGui()
{
	ImGui::SliderFloat("a", &a, 0, 20);
	ImGui::SliderFloat("b", &b, 0, 40);
	ImGui::SliderFloat("c", &c, 0, 10);
	ImGui::Checkbox("OK", &is_running);
}

void Canvas::start()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	float verts[] = {
		-1,-1,0,
		1,-1,0,
		1,1,0,
		-1,-1,0,
		1,1,0,
		-1,1,0,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	shader = shader_asset["default"];
}

void Canvas::render(const mat4& view, const mat4& proj)
{
	shader.use();
	shader.setMat4("model", getObject()->getWorldTransform());
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);
	shader.setVec4("color", vec4(1,0, 1, 1));
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void Canvas::onGui()
{
}
