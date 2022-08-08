#include "Canvas.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define N  50000

float vert_data[N * 4];

void Cloud::start()
{
	shader = shader_asset["default"];
	kernel = compute_shader_asset["test"];
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	auto random = []() {
		return (float)rand() / RAND_MAX;
	};
	for (int i = 0; i < N; i++) {
		vert_data[4 * i] = random() * 100 - 50;
		vert_data[4 * i + 1] = random() * 100 - 50;
		vert_data[4 * i + 2] = random() * 100 - 50;
		vert_data[4 * i + 3] = 1;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_data), vert_data, GL_STATIC_DRAW);

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
		-1,-1,0,0,0,
		1,-1,0,1,0,
		1,1,0,1,1,
		-1,-1,0,0,0,
		1,1,0,1,1,
		-1,1,0,0,1
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	int s[2];
	int comp;
	unsigned char* data = stbi_load("D:\\data\\fbx\\box.jpg", s, s + 1, &comp, 4);
	glGenTextures(1, &TEX0);
	glBindTexture(GL_TEXTURE_2D, TEX0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s[0], s[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &TEX1);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s[0], s[1], 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenTextures(1, &TEX2);
	glBindTexture(GL_TEXTURE_2D, TEX2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, s[0], s[1], 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glGenerateMipmap(GL_TEXTURE_2D);

	shader = shader_asset["texture"];
	kernel = compute_shader_asset["fluid"];
}

void Canvas::render(const mat4& view, const mat4& proj)
{
	kernel.use();

	glBindImageTexture(0, TEX1, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindTexture(GL_TEXTURE_2D, TEX0);
	kernel.setInt("pass", 0);
	glDispatchCompute(360, 360, 1);

	glBindImageTexture(0, TEX2, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	kernel.setInt("pass", 1);
	glDispatchCompute(360, 360, 1);

	glBindImageTexture(0, TEX1, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TEX0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TEX2);
	glActiveTexture(GL_TEXTURE0);
	kernel.setInt("pass", 2);
	glDispatchCompute(360, 360, 1);
	swap(TEX0, TEX1);

	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TEX0);
	shader.setMat4("model", getObject()->getWorldTransform());
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);
	shader.setInt("texture0", 0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);



}

void Canvas::onGui()
{
}
