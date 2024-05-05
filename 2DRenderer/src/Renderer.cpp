#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct RendererData
{
	struct Vertex
	{
		float position[3] = { 0.0f, 0.0f, 0.0f };
		float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float textureCoords[2] = {0.0f, 0.0f};
		int textureID = 0;
	};

	struct Quad
	{
		Vertex vertices[4];
	};

	GLFWwindow* window;
	uint32_t va;
	uint32_t vb;
	uint32_t ib;
	uint32_t prg;
	Quad* vertices;
	uint32_t* indices;
	uint32_t* textures;
	uint32_t currentQuad;
	uint32_t currentTexture;

	struct VertexAttribute
	{
		uint32_t count, type, size;
	};

	std::vector<VertexAttribute> vertexAttributes;

	static const uint32_t MaxQuads = 1000;
	static const uint32_t MaxTextures = 16;
};

RendererData data;

void Renderer::Initialize(int wWidth, int wHeight)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	data.window = glfwCreateWindow(wWidth, wHeight, "LearnOpenGL", NULL, NULL);
	if (!data.window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(data.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glViewport(0, 0, wWidth, wHeight);

	data.vertices = new RendererData::Quad[RendererData::MaxQuads];
	data.indices = new uint32_t[RendererData::MaxQuads*6];

	for (int i = 0; i < data.MaxQuads; i++)
	{
		data.indices[i * 6 + 0] = i * 4 + 0;
		data.indices[i * 6 + 1] = i * 4 + 1;
		data.indices[i * 6 + 2] = i * 4 + 2;
		data.indices[i * 6 + 3] = i * 4 + 2;
		data.indices[i * 6 + 4] = i * 4 + 3;
		data.indices[i * 6 + 5] = i * 4 + 0;
	}

	glCreateVertexArrays(1, &data.va);
	glBindVertexArray(data.va);

	glCreateBuffers(1, &data.vb);
	glCreateBuffers(1, &data.ib);

	glBindBuffer(GL_ARRAY_BUFFER, data.vb);
	glBufferData(GL_ARRAY_BUFFER, data.MaxQuads * sizeof(RendererData::Quad), data.vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * data.MaxQuads * 6, data.indices, GL_STATIC_DRAW);

	data.vertexAttributes.push_back({ 3, GL_FLOAT, 3 * sizeof(float) });
	data.vertexAttributes.push_back({ 4, GL_FLOAT, 4 * sizeof(float) });
	data.vertexAttributes.push_back({ 2, GL_FLOAT, 2 * sizeof(float) });
	data.vertexAttributes.push_back({ 1, GL_INT, sizeof(int) });

	SetVertexAttributes();

	if (!InitializeShader("res/vertex.shader", "res/fragment.shader"))
	{
		std::cout << "Shader Error" << std::endl;
	}

	int* samplers = new int[data.MaxTextures];
	for (int i = 0; i < data.MaxTextures; i++)
		samplers[i] = i;

	int loc = glGetUniformLocation(data.prg, "u_Textures");
	glUniform1iv(loc, data.MaxTextures, samplers);

	delete[] samplers;

	data.textures = new uint32_t[data.MaxTextures];
	memset(data.textures, 0, data.MaxTextures * sizeof(uint32_t));

	stbi_set_flip_vertically_on_load(true);

	data.currentQuad = 0;
	data.currentTexture = 0;

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::CleanUp()
{
	glDeleteVertexArrays(1, &data.va);
	glDeleteBuffers(1, &data.vb);
	glDeleteBuffers(1, &data.ib);
	glDeleteProgram(data.prg);
	delete[] data.vertices;
	delete[] data.indices;
	delete[] data.textures;

	glfwTerminate();
}

void Renderer::WindowHint(int hint, int value)
{
	glfwWindowHint(hint, value);
}

const int signs[] = { -1, -1, 1, -1, 1, 1, -1, 1 };

void Renderer::DrawTexture(float position[3], float dimensions[2], float color[4], uint32_t textureID)
{
	int texID = -1;
	for (int i = 0; i < data.currentTexture; i++)
	{
		if (data.textures[i] == textureID)
			texID = i;
	}
	if (texID == -1)
	{
		if (data.currentTexture == data.MaxTextures)
		{
			Render();
		}
		data.textures[data.currentTexture] = textureID;
		texID = data.currentTexture;
		data.currentTexture++;
	}

	for (int i = 0; i < 4; i++)
	{
		data.vertices[data.currentQuad].vertices[i].position[0] = position[0] + dimensions[0] * signs[i * 2] / 2;
		data.vertices[data.currentQuad].vertices[i].position[1] = position[1] + dimensions[1] * signs[i * 2 + 1] / 2;
		data.vertices[data.currentQuad].vertices[i].position[2] = position[2];
		for (int j = 0; j < 4; j++)
		{
			data.vertices[data.currentQuad].vertices[i].color[j] = color[j];
		}
		data.vertices[data.currentQuad].vertices[i].textureCoords[0] = (signs[i * 2] + 1) / 2;
		data.vertices[data.currentQuad].vertices[i].textureCoords[1] = (signs[i * 2 + 1] + 1) / 2;
		data.vertices[data.currentQuad].vertices[i].textureID = texID;
	}

	data.currentQuad++;

	if (data.currentQuad == data.MaxQuads)
	{
		Render();
	}
}

void Renderer::Render()
{
	for (int i = 0; i < data.currentTexture; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, data.textures[i]);
	}

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(RendererData::Quad) * data.currentQuad, data.vertices);

	glDrawElements(GL_TRIANGLES, data.currentQuad*6, GL_UNSIGNED_INT, 0);

	data.currentQuad = 0;
	data.currentTexture = 0;
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame()
{
	glfwSwapBuffers(data.window);
	glfwPollEvents();
}

bool Renderer::WindowIsOpen()
{
	return !glfwWindowShouldClose(data.window);
}

uint32_t Renderer::GenerateTexture(const std::string& filepath)
{
	uint32_t id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	int w, h, nrChannels;
	unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &nrChannels, 0);
	
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Error Loading Texture: " << filepath << std::endl;
	}

	stbi_image_free(data);

	return id;
}

uint32_t Renderer::GetKey(uint32_t key)
{
	return glfwGetKey(data.window, key);
}

uint32_t Renderer::GetMouseButton(uint32_t button)
{
	return glfwGetMouseButton(data.window, button);
}

std::pair<double, double> Renderer::GetCursorPos()
{
	double xPos, yPos;
	glfwGetCursorPos(data.window, &xPos, &yPos);
	return std::pair<double, double>(xPos, yPos);
}

bool Renderer::InitializeShader(const std::string& vertFilePath, const std::string& fragFilePath)
{
	uint32_t vs = 0;
	vs = glCreateShader(GL_VERTEX_SHADER);

	std::ifstream vsFile(vertFilePath);
	if (!vsFile.is_open()) std::cout << "ERROR:VERTEX::FILE::OPEN::FAIL" << std::endl;
	std::stringstream vsss;
	vsss << vsFile.rdbuf();
	std::string vss = vsss.str();
	const char* vertexShaderSource = vss.c_str();
	vsFile.close();

	glShaderSource(vs, 1, &vertexShaderSource, NULL);
	glCompileShader(vs);

	char infoLog[512];
	int success;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	};

	uint32_t fs = 0;
	fs = glCreateShader(GL_FRAGMENT_SHADER);

	std::ifstream fsFile(fragFilePath);
	if (!fsFile.is_open()) std::cout << "ERROR:FRAGMENT::FILE::OPEN::FAIL" << std::endl;
	std::stringstream fsss;
	fsss << fsFile.rdbuf();
	std::string fss = fsss.str();
	const char* fragmentShaderSource = fss.c_str();
	fsFile.close();

	glShaderSource(fs, 1, &fragmentShaderSource, NULL);
	glCompileShader(fs);


	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	};

	data.prg = glCreateProgram();
	glAttachShader(data.prg, vs);
	glAttachShader(data.prg, fs);
	glLinkProgram(data.prg);

	glDeleteShader(vs);
	glDeleteShader(fs);

	glGetProgramiv(data.prg, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(data.prg, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}

	glUseProgram(data.prg);
	return true;
}

void Renderer::SetVertexAttributes()
{
	uint32_t stride = 0;
	for (auto& va : data.vertexAttributes)
	{
		stride += va.size;
	}

	uint32_t offset = 0;
	for (int i = 0; i < data.vertexAttributes.size(); i++)
	{
		glVertexAttribPointer(i, data.vertexAttributes[i].count, data.vertexAttributes[i].type, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(i);
		offset += data.vertexAttributes[i].size;
	}
}
