#pragma once
#include "pch.h"

class Shader
{
public:
	unsigned ID;

	// constructor reads and builds the shader
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);

	// use/activate the shader
	void compileRaw(const char* vertexCode, const char* fragmentCode);
	void compile(const char* vertexPath, const char* fragmentPath);
	void use();
	void disable();

	// utility uniform functions
	void setBool(const string& name, bool value);
	void setInt(const string& name, int value);
	void setFloat(const string& name, float value);
	void setMat4(const string& name, const mat4& value);
	void setVec4(const string& name, const vec4& value);
};

class ShaderAsset {
public:
	map<string, Shader> asset;
	Shader operator[](string name) {
		auto it = asset.find(name);
		if (it != asset.end())
			return it->second;
		else {
			auto& shader = asset[name];
			shader.compile(string("../SCJEngine/shader/").append(name).append(".vert").c_str(), string("../SCJEngine/shader/").append(name).append(".frag").c_str());
			return shader;
		}
	};
};


class ComputeShader {
public:
	unsigned ID;

	void compileRaw(const char* computeCode);
	void compile(const char* computePath);

	void use();
};

class ComputeShaderAsset {
public:
	map<string, ComputeShader> asset;
	ComputeShader operator[](string name) {
		auto it = asset.find(name);
		if (it != asset.end())
			return it->second;
		else {
			auto& shader = asset[name];
			shader.compile(("../SCJEngine/shader/" + name + ".comp").c_str());
			return shader;
		}
	};
};

extern ShaderAsset shader_asset;
extern ComputeShaderAsset compute_shader_asset;