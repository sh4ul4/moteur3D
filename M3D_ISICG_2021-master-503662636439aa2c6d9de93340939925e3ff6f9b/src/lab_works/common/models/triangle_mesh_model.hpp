#ifndef __TRIANGLE_MESH_MODEL_HPP__
#define __TRIANGLE_MESH_MODEL_HPP__

#include "base_model.hpp"
#include "triangle_mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace M3D_ISICG
{
	class TriangleMeshModel : public BaseModel
	{
	  public:
		TriangleMeshModel()			 = default;
		virtual ~TriangleMeshModel() = default;

		// Load a 3D model with Assimp.
		void load( const std::string & p_name, const std::string & p_filePath );

		void render( const GLuint p_glProgram ) const override;

		void cleanGL() override;

	  private:
		void _loadMesh( const aiMesh * const p_mesh, const aiScene * const p_scene );

	  public:
		std::vector<TriangleMesh> _meshes; // A model can contain several meshes.
		// Some stats.
		int _nbTriangles = 0;
		int _nbVertices	 = 0;
	};
} // namespace M3D_ISICG

#endif // __TRIANGLE_MESH_MODEL_HPP__
