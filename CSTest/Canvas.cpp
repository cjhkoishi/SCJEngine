#include "Canvas.h"

void Canvas::start()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

//	glBufferData(GL_ARRAY_BUFFER, s);
}
