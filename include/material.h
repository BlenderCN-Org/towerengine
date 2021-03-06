#ifndef _MATERIAL_H
#define _MATERIAL_H

class tMaterial
{
	protected:
		bool own_textures;

	public:
		virtual ~tMaterial(void) {}

		virtual bool InitDepthPrePass(tRenderer *renderer)								{ return false; }
		virtual bool InitShadowPass(tRenderer *renderer)								{ return false; }
		virtual bool InitGeometryPass(tDeferredRenderer *renderer)						{ return false; }
		virtual bool InitStandardForwardPass(tForwardRenderer *renderer)				{ return false; }
		virtual bool InitForwardPass(tRenderer *renderer, float *transform)				{ return false; }
		virtual bool InitRefractionPass(tDeferredRenderer *renderer, float *transform)	{ return false; }

		virtual bool GetCubeMapReflectionEnabled(void)	{ return false; }
		virtual tVector GetCubeMapReflectionColor(void)	{ return tVec(0.0, 0.0, 0.0); }

		void SetOwnTextures(bool own)		{ this->own_textures = own; }
		bool GetOwnTextures()				{ return own_textures; }
};

class tStandardMaterial : public tMaterial
{
	public:
		enum TextureType
		{
			BASE_COLOR,
			METAL_ROUGH_REFLECT,
			NORMAL,
			EMISSION,
			BUMP
		};

		static const int tex_count = 5;

	protected:
		tVector base_color;
		float metallic;
		float roughness;
		float reflectance;

		tVector emission;

		float bump_depth;


		bool shadow_cast;


		bool transparent;

		GLuint tex[tex_count];


		tUniformBuffer *uniform_buffer;

	public:
		tStandardMaterial(void);
		~tStandardMaterial(void);

		void SetBaseColor(tVector color)	{ this->base_color = color; }
		void SetMetallic(float metallic)	{ this->metallic = metallic; }
		void SetRoughness(float roughness)	{ this->roughness = roughness; }
		void SetReflectance(float reflect)	{ this->reflectance = reflect; }
		void SetEmission(tVector color)		{ this->emission = color; }
		void SetBumpDepth(float depth)		{ this->bump_depth = depth; }
		void SetShadowCast(bool enabled)	{ this->shadow_cast = enabled; }

		void LoadTexture(TextureType type, std::string file);
		void LoadTexture(TextureType type, const char *extension, const void *data, size_t size);
		void SetTexture(TextureType type, GLuint gl_tex);

		void UpdateUniformBuffer(void);

		bool InitDepthPrePass(tRenderer *renderer);
		bool InitShadowPass(tRenderer *renderer);
		bool InitGeometryPass(tDeferredRenderer *renderer);
		bool InitStandardForwardPass(tForwardRenderer *renderer);
};

#endif
