#include "pch.h"
#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"

Object* Window::getRoot()
{
	return scene.getRoot();
}

void Window::objectViewerGui()
{
	ImGui::Begin("Object Viewer");

	ImGui::BeginChild("Tree",ImVec2(150, 0), true);
	Object* root = getRoot();
	function<void(Object*)> drawNode;

	drawNode = [&drawNode,this](Object* node) {
		auto num_c = node->numChildren();
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_FramePadding;
		if (num_c == 0) {
			base_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
		}		
		
		if (focused_obj == node) {
			base_flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool is_open = ImGui::TreeNodeEx(node->name.c_str(), base_flags);
		if (ImGui::IsItemFocused()) {
			focused_obj = node;
		}

		if (is_open) {
			for (int i = 0; i < num_c; i++) {
				drawNode(node->children(i));
			}
			ImGui::TreePop();
		}
	};
	for (int i = 0; i < root->numChildren(); i++) {
		drawNode(root->children(i));
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Components", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
	if (focused_obj) {
		focused_obj->onGui();
	}
	ImGui::EndChild();
	ImGui::End();
}

bool Window::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 1);
	window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);


	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	return true;
}
void Window::run()
{
	Object* camera_obj = scene.createObject("Camera");
	Camera* camera = camera_obj->addComponent<Camera>();
	render_system.setCamera(camera_obj);

	if (setScene)
		setScene(scene);

	scene.start();
	while (!glfwWindowShouldClose(window)) {

		glClearColor(1, 1, 0.9, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene.update();

		render_system.run(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		objectViewerGui();

		if (drawUI)
			drawUI(this);

		ImGui::Render();

		auto io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
}

void Window::destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
