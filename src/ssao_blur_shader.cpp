
#include "towerengine.h"
#include "tresources.h"


void tSSAOBlurShader::Init(void)
{
	InitShader(ssao_blur_shader_vert, ssao_blur_shader_frag, "SSAO Blur Shader");
	LinkProgram();

	tex_uniform = GetUniformLocation("tex_uni");

	blur_vertical_uniform = GetUniformLocation("blur_vertical_uni");

	Bind();
	glUniform1i(tex_uniform, 0);
}

void tSSAOBlurShader::SetTexture(GLuint tex)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void tSSAOBlurShader::SetBlurDirection(bool vertical)
{
	glUniform1i(blur_vertical_uniform, vertical ? 1 : 0);
}
