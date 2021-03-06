
#include <towerengine.h>

using namespace std;




bool CompareFloatComparable(tComparable<float> *a, tComparable<float> *b)
{
	return a->GetCompareValue() < b->GetCompareValue();
}

void tRenderer::InitRenderer(tWorld *world)
{
	this->world = world;

	shadow_pass = false;

	depth_prepass_enabled = true;

	InitShaders();

	screen_quad_vao = new tVAO();
	screen_quad_vao->Bind();

	screen_quad_vbo = new tVBO<float>(2, 4);
	float *screen_quad_data = screen_quad_vbo->GetData();
	screen_quad_data[0] = -1.0f; screen_quad_data[1] = 1.0f;
	screen_quad_data[2] = -1.0f; screen_quad_data[3] = -1.0f;
	screen_quad_data[4] = 1.0f; screen_quad_data[5] = 1.0f;
	screen_quad_data[6] = 1.0f; screen_quad_data[7] = -1.0f;
	screen_quad_vbo->AssignData();
	screen_quad_vbo->SetAttribute(tShader::vertex_attribute, GL_FLOAT);

	screen_quad_uv_vbo = new tVBO<float>(2, 4);
	screen_quad_data = screen_quad_uv_vbo->GetData();
	screen_quad_data[0] = 0.0f; screen_quad_data[1] = 1.0f;
	screen_quad_data[2] = 0.0f; screen_quad_data[3] = 0.0f;
	screen_quad_data[4] = 1.0f; screen_quad_data[5] = 1.0f;
	screen_quad_data[6] = 1.0f; screen_quad_data[7] = 0.0f;
	screen_quad_uv_vbo->AssignData();
	screen_quad_uv_vbo->SetAttribute(tScreenShader::uv_coord_attribute, GL_FLOAT);

	point_light_shadow_limit = -1;

	matrix_buffer = new tMatrixBuffer();
}

void tRenderer::InitShaders()
{
	color_shader = new tColorShader();
	simple_forward_shader = new tSimpleForwardShader();
	depth_pass_shader = new tDepthPassShader();
	point_shadow_shader = new tPointShadowShader();
	point_shadow_blur_shader = new tPointShadowBlurShader();
	directional_shadow_shader = new tDirectionalShadowShader();
	directional_shadow_blur_shader = new tDirectionalShadowBlurShader();
	skybox_shader = new tSkyBoxShader();
}

tRenderer::~tRenderer()
{
	delete point_shadow_shader;
	delete point_shadow_blur_shader;
	delete directional_shadow_shader;
	delete directional_shadow_blur_shader;
	delete skybox_shader;
	delete color_shader;
	delete simple_forward_shader;

	delete screen_quad_vbo;
	delete screen_quad_vao;

	delete matrix_buffer;
}


void tRenderer::PrepareRender(tCamera *camera, tRenderObjectSpace *render_space)
{
	current_rendering_camera = camera;
	current_rendering_render_space = render_space;

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	RenderShadowMaps();
}


void tRenderer::RenderShadowMaps(void)
{
	shadow_pass = true;

	// fill render spaces

	render_point_light_shadows.clear();

	set<tPointLight *>::iterator pi;
	set<tDirectionalLight *>::iterator di;
	list<tPointLight *>::iterator pli;

	for(pi=current_rendering_render_space->point_lights.begin(); pi!=current_rendering_render_space->point_lights.end(); pi++)
	{
		if(!(*pi)->GetShadowEnabled())
			continue;

		render_point_light_shadows.push_back(*pi);
		(*pi)->SetCompareValue(((*pi)->GetPosition() - current_rendering_camera->GetPosition()).SquaredLen());
	}

	if(point_light_shadow_limit == 0)
		render_point_light_shadows.clear();
	else if(point_light_shadow_limit > 0)
	{
		render_point_light_shadows.sort(CompareFloatComparable);

		while((int)render_point_light_shadows.size() > point_light_shadow_limit)
			render_point_light_shadows.pop_back();
	} // < 0 renders all point lights

	// add all point light shadows which have not been rendered at all yet
	for(pi=current_rendering_render_space->point_lights.begin(); pi!=current_rendering_render_space->point_lights.end(); pi++)
	{
		if(!(*pi)->GetShadowInvalid())
			continue;
		render_point_light_shadows.push_back(*pi);
	}
	render_point_light_shadows.unique();



	for(pli=render_point_light_shadows.begin(); pli!=render_point_light_shadows.end(); pli++)
	{
		tObjectSpace *object_space = (*pli)->GetShadow()->GetRenderObjectSpace();
		world->FillObjectSpace(object_space, *pli);
	}



	// finally render shadows

	for(pli=render_point_light_shadows.begin(); pli!=render_point_light_shadows.end(); pli++)
		(*pli)->RenderShadow(this);


	for(di=current_rendering_render_space->dir_lights.begin(); di!=current_rendering_render_space->dir_lights.end(); di++)
		(*di)->RenderShadow(current_rendering_camera, this);
}


void tRenderer::RenderScreenQuad(void)
{
	screen_quad_vao->Draw(GL_TRIANGLE_STRIP, 0, 4);
}