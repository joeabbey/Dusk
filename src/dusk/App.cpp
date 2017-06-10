#include "dusk/App.hpp"

#include <dusk/Log.hpp>
#include <dusk/Benchmark.hpp>

#include <fstream>

namespace dusk {

App * App::_Inst = nullptr;

App::App(int argc, char** argv)
{
    App::InitScripting();

    DuskLogInfo("Starting Application");

    _Inst = this;
}

App::~App()
{
    DuskLogInfo("Stopping Application");

    for (auto& it : _scenes)
    {
        delete it.second;
    }
    _scenes.clear();
}

void App::CreateWindow()
{
    DuskBenchStart();

    if (!glfwInit())
    {
        DuskLogError("Failed to initialize GLFW");
        return;
    }

    glfwSetErrorCallback(&App::GLFW_ErrorCallback);

#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 16);
    _glfwWindow = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), NULL, NULL);
    if (!_glfwWindow)
    {
        DuskLogError("Failed to create GLFW window");
        return;
    }

    glfwMakeContextCurrent(_glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        DuskLogError("Failed to initialize OpenGL context");
        return;
    }

    int samples;
    glGetIntegerv(GL_SAMPLES, &samples);
    DuskLogInfo("Running %dx AA", samples);

    ImGui_ImplGlfwGL3_Init(_glfwWindow, false);

    glfwSetMouseButtonCallback(_glfwWindow, &App::GLFW_MouseButtonCallback);
    glfwSetScrollCallback(_glfwWindow, &App::GLFW_ScrollCallback);
    glfwSetKeyCallback(_glfwWindow, &App::GLFW_KeyCallback);
    glfwSetCharCallback(_glfwWindow, &App::GLFW_CharCallback);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    DuskBenchEnd("App::CreateWindow()");
}

void App::DestroyWindow()
{
    ImGui_ImplGlfwGL3_Shutdown();

    glfwDestroyWindow(_glfwWindow);
    _glfwWindow = nullptr;

    glfwTerminate();
}

bool App::ParseWindow(nlohmann::json& data)
{
	if (data.find("Width") != data.end())
	{
		WindowWidth = data["Width"];
	}

	if (data.find("Height") != data.end())
	{
		WindowHeight = data["Height"];
	}

	if (data.find("Title") != data.end())
	{
		WindowTitle = data["Title"].get<std::string>();
	}

	if (_glfwWindow)
	{
		glfwSetWindowSize(_glfwWindow, WindowWidth, WindowHeight);
		glfwSetWindowTitle(_glfwWindow, WindowTitle.c_str());
	}

	return true;
}

bool App::ParseShader(nlohmann::json& data)
{
	std::vector<Shader::FileInfo> files;

	for (auto& file : data["Files"])
	{
		GLenum shaderType = GL_INVALID_ENUM;

		const std::string& type = file["Type"];
		if ("Vertex" == type)
		{
			shaderType = GL_VERTEX_SHADER;
		}
		else if ("Fragment" == type)
		{
			shaderType = GL_FRAGMENT_SHADER;
		}
		else if ("Geometry" == type)
		{
			shaderType = GL_GEOMETRY_SHADER;
		}
		// Compute Shader, GL 4.3+
		/*
		else if ("Compute" == type)
		{
		shaderType = GL_COMPUTE_SHADER;
		}
		*/
		// Tessellation Shaders, GL 4.0+
		/*
		else if ("TessControl" == type)
		{
		shaderType = GL_TESS_CONTROL_SHADER;
		}
		else if ("TessEvaluation" == type)
		{
		shaderType = GL_TESS_EVALUATION_SHADER;
		}
		*/

		files.push_back({
			shaderType,
			file["File"],
		});
	}

	Shader * shader = new Shader(data["Name"], data["BindData"], files);
	_shaders.emplace(data["Name"], shader);
	return true;
}

bool App::ParseScene(nlohmann::json& data)
{
	Scene * scene = new Scene(data["Name"].get<std::string>());

	for (auto& script : data["Scripts"])
	{
        scene->AddScript(script.get<std::string>());
	}

	for (auto& camera : data["Cameras"])
	{
		Camera * tmp = ParseCamera(camera);
		if (!tmp)
		{
			DuskLogError("Failed to parse camera");
			delete scene;
			return false;
		}
		scene->AddCamera(tmp);
	}

	for (auto& actor : data["Actors"])
	{
		Actor * tmp = ParseActor(actor, scene);
		if (!tmp)
		{
			DuskLogError("Failed to parse actor");
			delete scene;
			return false;
		}
		scene->AddActor(tmp);
	}

	_scenes.emplace(data["Name"], scene);
	return true;
}

Camera * App::ParseCamera(nlohmann::json& data)
{
	Camera * camera = new Camera();

	if (data.find("Position") != data.end())
	{
		camera->SetPosition({
			data["Position"][0], data["Position"][1], data["Position"][2]
		});
	}

	if (data.find("Forward") != data.end())
	{
		camera->SetForward({
			data["Forward"][0], data["Forward"][1], data["Forward"][2]
		});
	}

	if (data.find("Up") != data.end())
	{
		camera->SetUp({
			data["Up"][0], data["Up"][1], data["Up"][2]
		});
	}

	if (data.find("FOV") != data.end())
	{
		camera->SetFOV(data["FOV"]);
	}

	if (data.find("Aspect") != data.end())
	{
		camera->SetAspect(data["Aspect"]);
	}

	if (data.find("Clip") != data.end())
	{
		camera->SetClip(data["Clip"][0], data["Clip"][1]);
	}

	return camera;
}

Mesh * App::ParseMesh(nlohmann::json& data)
{
	Mesh * mesh = nullptr;

	Shader * shader = _shaders[data["Shader"]];

	if (data.find("File") != data.end())
	{
		mesh = new FileMesh(shader, data["File"].get<std::string>());
	}
	else if (data.find("Shape") != data.end())
	{
		// TODO
	}

	return mesh;
}

Actor * App::ParseActor(nlohmann::json& data, Scene * scene)
{
	Actor * actor = new Actor(scene, data["Name"].get<std::string>());

	DuskLogInfo("Read Actor %s", data["Name"].get<std::string>().c_str());

	if (data.find("Position") != data.end())
	{
		actor->SetPosition({
			data["Position"][0], data["Position"][1], data["Position"][2]
		});
	}

	if (data.find("Rotation") != data.end())
	{
		actor->SetRotation({
			data["Rotation"][0], data["Rotation"][1], data["Rotation"][2]
		});
	}

	if (data.find("Scale") != data.end())
	{
		actor->SetScale({
			data["Scale"][0], data["Scale"][1], data["Scale"][2]
		});
	}

	for (auto& component : data["Components"])
	{
		Component * tmp = ParseComponent(component, actor);
		if (!tmp)
		{
			DuskLogError("Failed to parse componenet");
			delete actor;
			return nullptr;
		}
		actor->AddComponent(tmp);
	}

	return actor;
}

Component * App::ParseComponent(nlohmann::json& data, Actor * actor)
{
	Component * component = nullptr;

	const std::string& type = data["Type"];
	if ("Mesh" == type)
	{
		Mesh * mesh = ParseMesh(data);
		if (!mesh)
		{
			DuskLogError("Failed to parse mesh");
			delete mesh;
			return nullptr;
		}
		component = new MeshComponent(actor, mesh);
	}
	else if ("Script" == type)
	{
		component = new ScriptComponent(actor, data["File"].get<std::string>());
	}
	else if ("Camera" == type)
	{
		Camera * camera = ParseCamera(data);
		component = new CameraComponent(actor, camera);
	}

	return component;
}

bool App::LoadConfig(const std::string& filename)
{
    DuskBenchStart();

    std::ifstream file(filename);
    nlohmann::json data;

    DuskLogInfo("Loading config file '%s'", filename.c_str());

    if (!file.is_open())
    {
        DuskLogError("Failed to open config file '%s'", filename.c_str());
        return false;
    }

	data << file;

	bool retval = true;

	if (data.find("Window") != data.end())
	{
		retval &= ParseWindow(data["Window"]);
	}

    for (auto& shader : data["Shaders"])
    {
		retval &= ParseShader(shader);
    }

    for (auto& scene : data["Scenes"])
    {
		retval &= ParseScene(scene);
    }

    const auto& startScene = _scenes.find(data["StartScene"]);
    if (startScene != _scenes.end())
    {
        _startScene = startScene->second;
    }

    file.close();

    DuskBenchEnd("App::LoadConfig()");
    return retval;
}

Scene * App::GetSceneByName(const std::string& name) const
{
    if (_scenes.find(name) != _scenes.end())
    {
        return _scenes.at(name);
    }
    return nullptr;
}

void App::PushScene(Scene * scene)
{
    if (!scene) return;

    if (!_sceneStack.empty())
    {
        _sceneStack.top()->Stop();
    }

    _sceneStack.push(scene);

    DuskLogInfo("Starting Scene %s", _sceneStack.top()->GetName().c_str());
    _sceneStack.top()->Start();
}

void App::PopScene()
{
    if (_sceneStack.empty()) return;

    _sceneStack.top()->Stop();
    _sceneStack.pop();

    if (_sceneStack.empty()) return;

    DuskLogInfo("Starting Scene %s", _sceneStack.top()->GetName().c_str());
    _sceneStack.top()->Start();
}

void App::Run()
{
    CreateWindow();

    // Load the shaders, now that we have a GL Context
    for (const auto& it : _shaders)
    {
        it.second->Load();
    }

    // Load the starting scene
    PushScene(_startScene);

    double frame_delay = 1.0;   // MS until next frame
    double frame_elap  = 0.0;   // MS since last frame
    double fps_delay   = 250.0; // MS until next FPS update
    double fps_elap    = 0.0;   // MS since last FPS update
    double elapsed;

    unsigned long frames = 0;

    UpdateEventData updateEventData;

    updateEventData.SetTargetFPS(TARGET_FPS);
    frame_delay = (1000.0 / TARGET_FPS) / 1000.0;

    double timeOffset = glfwGetTime();
    while (!glfwWindowShouldClose(_glfwWindow))
    {
        // TODO: Cleanup
        elapsed = glfwGetTime() - timeOffset;
        timeOffset = glfwGetTime();

        glfwPollEvents();

        updateEventData.Update(elapsed);
        DispatchEvent(Event((EventID)Events::UPDATE, updateEventData));

        frame_elap += elapsed;
        if (frame_delay <= frame_elap)
        {
            frame_elap = 0.0;
            ++frames;

            ImGui_ImplGlfwGL3_NewFrame();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DispatchEvent(Event((EventID)Events::RENDER));

            UI::Render();

            glfwSwapBuffers(_glfwWindow);
        }

        fps_elap += elapsed;
        if (fps_delay <= fps_elap)
        {
            float fps = (float)((frames / fps_elap) * 1000.0f);
            updateEventData.SetCurrentFPS(fps);

            frames = 0;
            fps_elap = 0.0;
        }
    }

    if (!_sceneStack.empty())
    {
        _sceneStack.top()->Stop();
    }

    DestroyWindow();
}

void App::InitScripting()
{
    ScriptHost::AddFunction("dusk_App_GetInst", &App::Script_GetInst);
    ScriptHost::AddFunction("dusk_App_LoadConfig", &App::Script_LoadConfig);
    ScriptHost::AddFunction("dusk_App_GetScene", &App::Script_GetScene);
    ScriptHost::AddFunction("dusk_App_GetSceneByName", &App::Script_GetSceneByName);
    ScriptHost::AddFunction("dusk_App_PushScene", &App::Script_PushScene);
    ScriptHost::AddFunction("dusk_App_PopScene", &App::Script_PopScene);

    IEventDispatcher::InitScripting();

    Scene::InitScripting();
    Actor::InitScripting();
    Component::InitScripting();
}

int App::Script_GetInst(lua_State * L)
{
    lua_pushinteger(L, (ptrdiff_t)App::GetInst());

    return 1;
}

int App::Script_LoadConfig(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);

    bool result = app->LoadConfig(std::string(lua_tostring(L, 2)));
    lua_pushboolean(L, result);

    return 1;
}

int App::Script_GetScene(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);

    lua_pushinteger(L, (ptrdiff_t)app->GetScene());

    return 1;
}

int App::Script_GetSceneByName(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);
    std::string name = lua_tostring(L, 2);

    lua_pushinteger(L, (ptrdiff_t)app->GetSceneByName(name));

    return 1;
}

int App::Script_PushScene(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);
    Scene * scene = (Scene *)lua_tointeger(L, 2);

    app->PushScene(scene);

    return 0;
}

int App::Script_PopScene(lua_State * L)
{
    App * app = (App *)lua_tointeger(L, 1);

    app->PopScene();

    return 0;
}

void App::GLFW_ErrorCallback(int code, const char * message)
{
    DuskLogError("GLFW: %d, %s", code, message);
}

void App::GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
}

void App::GLFW_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
}

void App::GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
        UI::ConsoleShown ^= 1;

    ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
}

void App::GLFW_CharCallback(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfwGL3_CharCallback(window, c);
}

int UpdateEventData::PushToLua(lua_State * L) const
{
    lua_newtable(L);

    lua_pushstring(L, "Delta");
    lua_pushnumber(L, _delta);
    lua_settable(L, -3);

    lua_pushstring(L, "ElapsedTime");
    lua_pushnumber(L, _elapsed_time);
    lua_settable(L, -3);

    lua_pushstring(L, "TotalTime");
    lua_pushnumber(L, _total_time);
    lua_settable(L, -3);

    lua_pushstring(L, "CurrentFPS");
    lua_pushnumber(L, _current_fps);
    lua_settable(L, -3);

    return 1;
}

void UpdateEventData::Update(double elapsed)
{
    _elapsed_time = elapsed;
    _total_time += elapsed;
    _delta = (float)(elapsed / (1000.0f / _target_fps));
}

} // namespace dusk
