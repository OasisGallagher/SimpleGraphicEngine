#include "graphics.h"

class Cube : public Object {
public:
	Cube(float scale = 1.f) : yRotation(12 * deg2Rad) {
		Vertex v[24];
		// fill in the front face vertex data
		v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0, 0.999f);
		v[1] = Vertex(-1.0f, 1.0f, -1.0f, 0, 0);
		v[2] = Vertex(1.0f, 1.0f, -1.0f, 0.999f, 0);
		v[3] = Vertex(1.0f, -1.0f, -1.0f, 0.999f, 0.999f);

		// fill in the back face vertex data
		v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0, 0.999f);
		v[5] = Vertex(1.0f, -1.0f, 1.0f, 0.999f, 0.999f);
		v[6] = Vertex(1.0f, 1.0f, 1.0f, 0.999f, 0);
		v[7] = Vertex(-1.0f, 1.0f, 1.0f, 0, 0);

		// fill in the top face vertex data
		v[8] = Vertex(-1.0f, 1.0f, -1.0f, 0, 0.999f);
		v[9] = Vertex(-1.0f, 1.0f, 1.0f, 0.999f, 0.999f);
		v[10] = Vertex(1.0f, 1.0f, 1.0f, 0.999f, 0);
		v[11] = Vertex(1.0f, 1.0f, -1.0f, 0, 0);

		// fill in the bottom face vertex data
		v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0, 0.999f);
		v[13] = Vertex(1.0f, -1.0f, -1.0f, 0.999f, 0.999f);
		v[14] = Vertex(1.0f, -1.0f, 1.0f, 0.999f, 0);
		v[15] = Vertex(-1.0f, -1.0f, 1.0f, 0, 0);

		// fill in the left face vertex data
		v[16] = Vertex(-1.0f, -1.0f, 1.0f, 0, 0.999f);
		v[17] = Vertex(-1.0f, 1.0f, 1.0f, 0, 0);
		v[18] = Vertex(-1.0f, 1.0f, -1.0f, 0.999f, 0);
		v[19] = Vertex(-1.0f, -1.0f, -1.0f, 0.999f, 0.999f);

		// fill in the right face vertex data
		v[20] = Vertex(1.0f, -1.0f, -1.0f, 0, 0.999f);
		v[21] = Vertex(1.0f, 1.0f, -1.0f, 0, 0);
		v[22] = Vertex(1.0f, 1.0f, 1.0f, 0.999f, 0);
		v[23] = Vertex(1.0f, -1.0f, 1.0f, 0.999f, 0.999f);

		Texture crateTexture("bmp/crate.bmp");
		Triangle cube[] = {
			Triangle(0, 1, 2, crateTexture),
			Triangle(0, 2, 3, crateTexture),
			Triangle(4, 5, 6, crateTexture),
			Triangle(4, 6, 7, crateTexture),
			Triangle(8, 9, 10, crateTexture),
			Triangle(8, 10, 11, crateTexture),
			Triangle(12, 13, 14, crateTexture),
			Triangle(12, 14, 15, crateTexture),
			Triangle(16, 17, 18, crateTexture),
			Triangle(16, 18, 19, crateTexture),
			Triangle(20, 21, 22, crateTexture),
			Triangle(20, 22, 23, crateTexture),
		};

		Initialize(VertexContainer(v, v + _countof(v)),
				   IndexedTriangleContainer(cube, cube + _countof(cube)),
				   5.f, scale, Vector3(0, 12.5, -47), Vector3(0, 0));// 3.141592f / 4));
	}

	virtual bool Update(float deltaTime) {
		if (!Object::Update(deltaTime))
			return false;
		eulerAngles.y += yRotation * deltaTime;
		
		if (eulerAngles.y > Pi * 2)
			eulerAngles.y -= 2 * Pi;

		return true;
	}

private:
	float yRotation;
};
