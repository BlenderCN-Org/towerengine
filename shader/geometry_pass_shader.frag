#version 330

#define M_PI 3.1415926535897932384626433832795

uniform vec3 diffuse_color_uni;
uniform vec4 diffuse_color2_uni;
uniform vec3 specular_color_uni;
uniform float specular_size_uni;
uniform float bump_depth_uni;

uniform bool diffuse_tex_enabled_uni;
uniform bool specular_tex_enabled_uni;
uniform bool normal_tex_enabled_uni;
uniform bool bump_tex_enabled_uni;

uniform sampler2D diffuse_tex_uni;
uniform sampler2D normal_tex_uni;
uniform sampler2D specular_tex_uni;
uniform sampler2D bump_tex_uni;


uniform vec3 clip_vec_uni;
uniform float clip_dist_uni;


in vec3 pos_var;
in vec3 normal_var;
in vec3 tang_var;
in vec3 bitang_var;
in vec2 uv_var;

in vec3 cam_dir_var;


layout (location = 0) out vec4 position_out;
layout (location = 1) out vec4 diffuse_out;
layout (location = 2) out vec4 normal_out;
layout (location = 3) out vec4 specular_out; 
layout (location = 4) out vec4 specular_exponent_out;

vec2 ParallaxUV(void);
vec2 ParallaxOcclusionUV(mat3 tang_mat);

void main(void)
{
	if(!gl_FrontFacing) // backface culling
		discard;
		
	if(clip_vec_uni != vec3(0.0, 0.0, 0.0)) // face clipping for water
	{
		vec3 clip = pos_var - clip_vec_uni * clip_dist_uni;
		if(dot(clip, clip_vec_uni) >= 0.0)
			discard;
	}
	
	mat3 tang_mat = mat3(normalize(tang_var), normalize(bitang_var), normalize(normal_var));
	
	vec2 uv;
	
	if(bump_tex_enabled_uni)
		uv = ParallaxOcclusionUV(tang_mat);
	else
		uv = uv_var;
				
	vec4 diffuse_color = vec4(1.0, 1.0, 1.0, 1.0);
	
	if(diffuse_tex_enabled_uni)
		diffuse_color = texture2D(diffuse_tex_uni, uv).rgba;
		
	diffuse_color *= vec4(diffuse_color_uni.rgb, 1.0) * diffuse_color2_uni;
	
	vec3 normal;
	if(normal_tex_enabled_uni)
	{
		vec3 normal_tex_color = texture2D(normal_tex_uni, uv).rgb;
		normal = tang_mat * ((normal_tex_color - vec3(0.5, 0.5, 0.5)) * 2.0);
	}
	else
		normal = normal_var;
		
	normal_out = vec4(normal * 0.5 + vec3(0.5, 0.5, 0.5), 1.0);
			
	vec3 specular_color = specular_color_uni;
	if(specular_tex_enabled_uni)
		specular_color *= texture2D(specular_tex_uni, uv).rgb;
	
	position_out = vec4(pos_var, 1.0);
	diffuse_out = diffuse_color;
	specular_out = vec4(specular_color, 1.0);
	specular_exponent_out = vec4(specular_size_uni, 0.0, 0.0, 1.0);
}


vec2 ParallaxUV(void)
{
	float height = texture2D(bump_tex_uni, uv_var).r * bump_depth_uni;
	vec3 cam_offset = height * normalize(cam_dir_var);
	vec2 uv_offset;
	uv_offset.x = dot(cam_offset, tang_var);
	uv_offset.y = dot(cam_offset, bitang_var);
	return uv_var + uv_offset;
}

#define PARALLAX_OCCLUSION_MIN_SAMPLES 4
#define PARALLAX_OCCLUSION_MAX_SAMPLES 20

vec2 ParallaxOcclusionUV(mat3 tang_mat)
{
	vec3 cam_dir_tang = cam_dir_var * tang_mat;
	
	float parallax_limit = -length(cam_dir_tang.xy) / cam_dir_tang.z;
	parallax_limit *= bump_depth_uni;
	
	vec2 offset_dir = normalize(cam_dir_tang.xy);
		
	vec2 max_offset = offset_dir * parallax_limit;
	
	int samples_count = int(mix(PARALLAX_OCCLUSION_MAX_SAMPLES, PARALLAX_OCCLUSION_MIN_SAMPLES, dot(-normalize(cam_dir_var), normal_var)));
	float step_size = 1.0 / float(samples_count);
	
	vec2 dx = dFdx(uv_var);
	vec2 dy = dFdy(uv_var);
	
	float current_ray_height = 1.0;
	vec2 current_offset = vec2(0.0, 0.0);
	vec2 last_offset = vec2(0.0, 0.0);
	
	float last_sampled_height = 1.0;
	float current_sampled_height = 1.0;
	
	int current_sample = 0;
	
	while(current_sample < samples_count)
	{
		current_sampled_height = textureGrad(bump_tex_uni, uv_var + current_offset, dx, dy).r;
		
		if(current_sampled_height > current_ray_height)
		{
			float delta1 = current_sampled_height - current_ray_height;
			float delta2 = (current_ray_height + step_size) - last_sampled_height;
			float ratio = delta1 / (delta1 + delta2);
			
			current_offset = ratio * last_offset + (1.0 - ratio) * current_offset;
			
			break;
		}		
		else
		{
			current_sample++;
			
			current_ray_height -= step_size;
			last_offset = current_offset;
			current_offset += step_size * max_offset;
			
			last_sampled_height = current_sampled_height;
		}
	}
	
	return uv_var + current_offset;
}











