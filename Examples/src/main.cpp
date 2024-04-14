#include "Renderer.h"

int main()
{
	Renderer::Initialize(800, 800);

	float p[3] = { 0.2f, 0.2f, 0.2f };
	float d[2] = { 0.5f, 0.8f };
	float c[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	uint32_t texID = Renderer::GenerateTexture("res/Among_Us.png");

	float** positions = new float*[4000];
	float** dimensions = new float*[4000];
	float** colors = new float*[4000];

	srand(time(NULL));

	for (int i = 0; i < 4000; i++)
	{
		positions[i] = new float[3];
		dimensions[i] = new float[2];
		colors[i] = new float[4];

		for (int j = 0; j < 3; j++)
			positions[i][j] = (rand() % 4000)/1000.0f - 2;
		for (int j = 0; j < 2; j++)
			dimensions[i][j] = (rand() % 4000) / 1000.0f;
		for (int j = 0; j < 4; j++)
			colors[i][j] = (rand () % 500) / 1000.0f;
	}

	while (Renderer::WindowIsOpen())
	{
		Renderer::Clear();

		Renderer::DrawTexture(p, d, c, texID);

		for (int i = 0; i < 4000; i++)
			Renderer::DrawTexture(positions[i], dimensions[i], colors[i], texID);

		Renderer::Render();

		Renderer::EndFrame();
	}

	for (int i = 0; i < 4000; i++)
	{
		delete[] positions[i];
		delete[] dimensions[i];
		delete[] colors[i];
	}
	delete[] positions;
	delete[] dimensions;
	delete[] colors;

	Renderer::CleanUp();

	return 0;
}