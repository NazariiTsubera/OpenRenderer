#pragma once
#include "Events/Events.h"
#include <functional>



struct FrameStats {
    float DeltaTime;
    float Timer;

	void Begin();
	void End();
};

struct GLFWwindow;

struct WindowProps
{
	unsigned int Width;
	unsigned int Height;
	std::string Title;

	WindowProps(int width = 1200, int height = 840, std::string title = "Window")
		: Width(width), Height(height), Title(title)
	{
	}
};

class Window
{
	using EventCallbackFn = std::function<void(Events::Event&)>;

public:
	Window(WindowProps props);
	~Window() {}
public:
	inline unsigned int GetWidth() const { return m_Data.Width; };
	inline unsigned int GetHeight() const { return m_Data.Height; };

	void Update();
	void Shutdown();
	void SetEventCallback(EventCallbackFn fn) { m_Data.EventCallback = fn; }
	inline GLFWwindow* GetHandle() const { return m_Handle; }
private:
	static void MouseMovedCallback(GLFWwindow* window, double xPos, double yPos);
	static void DefaultEventCallback(Events::Event&);
public:
	bool Active;
	static Window* currentWindow;
private:
	struct WindowData
	{
		unsigned int Width;
		unsigned int Height;
		std::string Title;
		EventCallbackFn EventCallback;
		Window* WindowPtr;
	};

	static void SetupGL();

	static bool s_GlfwInitialized;
	WindowData m_Data;
	GLFWwindow* m_Handle;
};
