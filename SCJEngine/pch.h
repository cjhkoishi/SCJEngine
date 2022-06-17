#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;

#include<Eigen/Dense>
using namespace Eigen;
#define ETL(S,T) S<T,Eigen::aligned_allocator<T>>

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <stack>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <regex>
using namespace std;