#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class Renderer
{
public:
	static void Initialize(int wWidth, int wHeight);
	static void CleanUp();

	static void WindowHint(int hint, int value);

	static void DrawTexture(float position[3], float dimensions[2], float color[4], uint32_t textureID);
	static void Render();

	static void Clear();
	static void EndFrame();

	static bool WindowIsOpen();

	static uint32_t GenerateTexture(const std::string& filepath);

	static uint32_t GetKey(uint32_t key);
	static uint32_t GetMouseButton(uint32_t button);
	static std::pair<double, double> GetCursorPos();
private:
	static bool InitializeShader(const std::string& vertFilePath, const std::string& fragFilePath);
	static void SetVertexAttributes();
};