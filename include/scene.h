
#ifndef _SCENE_H
#define _SCENE_H


// rapidxml simple declarations
namespace rapidxml
{
    template<class Ch> class xml_node;
    template<class Ch> class xml_attribute;
    template<class Ch> class xml_document;
}


class tScene
{
	private:
		tWorld *world;

		tMaterialManager *material_manager;
		bool own_material_manager;

		std::string load_path;

		std::map<std::string, tAsset *> assets;
		std::map<std::string, tSceneObject *> objects;

		std::map<std::string, tReflectionProbeSceneObject *> reflection_probe_scene_objects;

		tCubeMapAsset *sky_cubemap;
		tSkyBox *skybox;

		void ParseSceneNode(rapidxml::xml_node<char> *cur);

		void ParseAssetsNode(rapidxml::xml_node<char> *cur);
		void ParseMeshAssetNode(rapidxml::xml_node<char> *cur);
		void ParseMaterialAssetNode(rapidxml::xml_node<char> *cur);
		void ParseCubeMapAssetNode(rapidxml::xml_node<char> *cur);

		void ParseObjectsNode(rapidxml::xml_node<char> *cur);
		tSceneObject *ParseMeshObjectNode(rapidxml::xml_node<char> *cur);
		tSceneObject *ParseDirectionalLightObjectNode(rapidxml::xml_node<char> *cur);
		tSceneObject *ParsePointLightObjectNode(rapidxml::xml_node<char> *cur);
		tSceneObject *ParseEmptyObjectNode(rapidxml::xml_node<char> *cur);

		static float ParseFloatNode(rapidxml::xml_node<char> *cur, const char *p = "v");
		static tTransform ParseTransformNode(rapidxml::xml_node<char> *cur);
		static tVector ParseVectorNode(rapidxml::xml_node<char> *cur,	const char *x_p = "x",
																		const char *y_p = "y",
																		const char *z_p = "z");

		void AddObject(std::string name, tSceneObject *object);

	public:
		tScene(tWorld *world, tMaterialManager *material_manager = 0);
		~tScene(void);

		bool LoadFromFile(std::string file);

		void AddToWorld(void);
		void RemoveFromWorld(void);

		tSceneObject *GetObjectByTag(std::string tag);
		void GetObjectsByTag(std::string tag, std::list<tSceneObject *> &result);
		tSceneObject *GetObjectWhereTagStartsWith(std::string start);
		void GetObjectsWhereTagStartsWith(std::string start, std::list<tSceneObject *> &result);

		void GetObjectsMapByTag(std::string tag, std::string attribute, std::multimap<std::string, tSceneObject *> &result);
};



#endif
