#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// vertex shader source code
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// fragment shader source code
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

// MODULE STATE BOOLS to keep track of shown/unshown modules
static bool showInspectorModule = true;
static bool showViewportModule = true;
static bool showHierarchyModule = true;
static bool showFileDirectoryModule = true;
static bool showConsoleModule = true;
static bool showControlsModule = true;

#define CHECKED_MENU_ITEM(menuItemName, checkedState) if (ImGui::MenuItem(menuItemName, NULL, checkedState)) { checkedState = !checkedState;}

void ShowMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "CTRL + S")) {}
			if (ImGui::MenuItem("Save as")) {}
			if (ImGui::MenuItem("Import")) {}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View Module"))
		{
			// show all module options to close/open specific modules
			CHECKED_MENU_ITEM("Inspector", showInspectorModule);
			CHECKED_MENU_ITEM("Viewport", showViewportModule);
			CHECKED_MENU_ITEM("Hierarchy", showHierarchyModule);
			CHECKED_MENU_ITEM("File Directory", showFileDirectoryModule);
			CHECKED_MENU_ITEM("Console", showConsoleModule);
			CHECKED_MENU_ITEM("Controls", showControlsModule);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

static float pos[3];
static float rot[3];
static float scale[3];
static bool scaleUniform;

void ShowTransformComponent()
{
	//TODO: transform component
	if (ImGui::CollapsingHeader("Transform"))
	{
		float oldScaleValues[3] = { scale[0], scale[1], scale[2] };

		// transform sliders
		ImGui::DragFloat3("Position", pos, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		ImGui::DragFloat3("Rotation", rot, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");

		if (ImGui::DragFloat3("Scale", scale, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f"))
		{
			float deltaX = scale[0] - oldScaleValues[0];
			float deltaY = scale[1] - oldScaleValues[1];
			float deltaZ = scale[2] - oldScaleValues[2];

			// check if scale uniform is on
			if (scaleUniform)
			{
				// change all sliders the same amount as changed slider
				if (deltaX != 0)
				{
					scale[1] += deltaX;
					scale[2] += deltaX;
				}
				else if (deltaY != 0)
				{
					scale[0] += deltaY;
					scale[2] += deltaY;
				}
				else if (deltaZ != 0)
				{
					scale[0] += deltaZ;
					scale[1] += deltaZ;
				}
			}
		}
		ImGui::Checkbox("Scale Uniform", &scaleUniform);

	}
}

static bool isTrigger;

void ShowMeshColliderComponent()
{
	if (ImGui::CollapsingHeader("Mesh Collider"))
	{
		ImGui::Checkbox("Is Trigger", &isTrigger);
	}
}

void ShowInspectorModule()
{

	if (showInspectorModule) 
	{
		if (ImGui::Begin("Inspector", &showInspectorModule)) 
		{
			ShowTransformComponent();
			ShowMeshColliderComponent();
		}
		ImGui::End();
	}
}

void ShowViewportModule()
{
	if (showViewportModule)
	{
		if (ImGui::Begin("Viewport", &showViewportModule))
		{
			//TODO: myca
			// luke requests this holds a 2d image so it can be updated by the renderer later for now
		}
		ImGui::End();
	}
}

void ShowHierarchyModule()
{
	if (showHierarchyModule)
	{
		if (ImGui::Begin("Hierarchy", &showHierarchyModule))
		{
			//TODO: myca
		}
		ImGui::End();
	}
}

void ShowFileDirectoryModule()
{
	if (showFileDirectoryModule)
	{
		if (ImGui::Begin("File Directory", &showFileDirectoryModule))
		{
			//TODO: myca
		}
		ImGui::End();
	}
}

void ShowConsoleTraceOutput(const char* source, const char* message)
{
	ImU32 consoleTraceGray = ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 0.65f));
	ImGui::Text("[%s] : %s", source, message);
	ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, consoleTraceGray);
}

void ShowConsoleWarningOutput(const char* source, const char* message)
{
	ImU32 consoleWarningYellow = ImGui::GetColorU32(ImVec4(0.6f, 0.4f, 0.04f, 0.85f));
	ImGui::Text("[%s] : %s", source, message);
	ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, consoleWarningYellow);
}

void ShowConsoleErrorOutput(const char* source, const char* message)
{
	ImU32 consoleErrorRed = ImGui::GetColorU32(ImVec4(0.8f, 0.1f, 0.04f, 0.65f));
	ImGui::Text("[%s] : %s", source, message);
	ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, consoleErrorRed);
}

void ShowConsoleModule()
{
	if (showConsoleModule)
	{
		if (ImGui::Begin("Console", &showConsoleModule))
		{
			// TODO: kelly
			// Helper class to easy setup a text filter.
			// You may want to implement a more feature-full filtering scheme in your own application.
			HelpMarker("Not a widget per-se, but ImGuiTextFilter is a helper to perform simple filtering on text strings.");
			static ImGuiTextFilter filter;
			filter.Draw("Search");
			//const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
			//for (int i = 0; i < IM_COUNTOF(lines); i++)
			//	if (filter.PassFilter(lines[i]))
			//		ImGui::BulletText("%s", lines[i]);
			//ImGui::TreePop();

			if (ImGui::BeginTable("ConsoleOutputTable", 1))
			{
				ImGui::TableSetupColumn("Output", ImGuiTableColumnFlags_WidthFixed);
				
				ImGui::TableNextRow();
				if (ImGui::TableSetColumnIndex(0))
				{
					ShowConsoleTraceOutput("TEST", "This is an example trace output.");
				}
				
				ImGui::TableNextRow();
				if (ImGui::TableNextColumn())
				{
					ShowConsoleWarningOutput("TEST", "This is an example warning output.");
				}

				ImGui::TableNextRow();
				if (ImGui::TableNextColumn())
				{
					ShowConsoleErrorOutput("TEST", "This is an example error output!");
				}

				for (int i = 0; i < 50; i++) {
					ImGui::TableNextRow();
					if (ImGui::TableNextColumn())
					{
						ShowConsoleTraceOutput("TEST", std::format("Output {}", i).c_str());
					}
				}

			}
			ImGui::EndTable();
		}
		ImGui::End();
	}
}

void ShowControlsModule() 
{
	// control keybinds - hardcoded because not planning on being customizable
	std::vector<std::pair<const char*, const char*>> ControlTextMap = 
	{
		{"Undo", "CTRL + Z"},
		{"Redo", "CTRL + Y"},
		{"Rotate Viewport Angle", "ALT + MMB"},
		{"Zoom In","Mouse Scroll Down"},
		{"Zoom Out","Mouse Scroll Up"},
		{"Create Camera From View","CTRL + SHIFT + C"},
		{"Move","W"},
		{"Rotate","E"},
		{"Scale","R"},
		{"Duplicate","CTRL + D"},
		{"Delete","DELETE"}
	};

	if (showControlsModule) 
	{
		if (ImGui::Begin("Controls", &showControlsModule)) 
		{
			// [Method 1] Using TableNextRow() to create a new row, and TableSetColumnIndex() to select the column.
			// In many situations, this is the most flexible and easy to use pattern.
			HelpMarker("Control keybinds in Orion are not currently editable!");
			static ImGuiTableFlags flags =
				ImGuiTableFlags_SizingFixedFit |
				ImGuiTableFlags_Hideable;

			if (ImGui::BeginTable("ControlsTable", 3, flags))
			{
				// make control name column fixed width, keybind column stretch
				ImGui::TableSetupColumn("Control", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Keybind", ImGuiTableColumnFlags_WidthStretch);
				auto it = ControlTextMap.begin();

				// set up all row entries
				for (int row = 0; row < ControlTextMap.size(); row++)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(it->first);
					ImGui::TableSetColumnIndex(1);
					ImGui::Text(it->second);

					// iterate to next control keybind to print
					it++;
				}
				ImGui::EndTable();
			}
			ImGui::End();
		}
	}
	
}

int main()
{
	const int window_width = 1920;
	const int window_height = 1080;

	glfwInit();

	// window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat vertices[] =
	{
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f
	};

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Orion Modules", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	// make this window part of the current context (we want to use it)
	glfwMakeContextCurrent(window);

	// tell glad to load openGL configurations
	gladLoadGL();
	
	//// specify the area OpenGL needs to render in by rect. begin coord and end coords
	// coordinate system puts origin in bottom left corner
	glViewport(0, 0, window_width, window_height);

	//// create reference values for OpenGL to use
	//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glCompileShader(vertexShader);

	//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//glCompileShader(fragmentShader);

	//GLuint shaderProgram = glCreateProgram();

	//glAttachShader(shaderProgram, vertexShader);
	//glAttachShader(shaderProgram, fragmentShader);
	//glLinkProgram(shaderProgram);

	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);

	//GLuint VAO, VBO; // vertex array object and vertex buffer object

	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	// swap the two buffers so the back buffer is the front buffer now.
	glfwSwapBuffers(window);

	// -------------------IMGUI INITIALIZATION-------------------
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	// I want a blue color (on the back buffer).
	glClearColor(0.01f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// -------------------APPLICATION WINDOW LOOP-------------------
	while (!glfwWindowShouldClose(window))
	{	

		// I want a blue color (on the back buffer).
		glClearColor(0.01f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//glUseProgram(shaderProgram);
		//glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();
		
		ShowMainMenuBar();
		ShowInspectorModule();
		ShowViewportModule();
		ShowHierarchyModule();
		ShowFileDirectoryModule();
		ShowConsoleModule();
		ShowControlsModule();

		ImGui::ShowDemoWindow();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			// for OpenGL: restore current GL context.
			glfwMakeContextCurrent(backup_current_context);
		}

		// swap back buffer with front buffer
		glfwSwapBuffers(window);
		// poll events (allows window to update context)
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// delete GL arrays and objects
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteProgram(shaderProgram);

	// destroy the given glfw window
	glfwDestroyWindow(window);
	// stop glfw
	glfwTerminate();
	return 0;
}