#include "triangle_mesh_model.hpp"
#include <iostream>

namespace M3D_ISICG
{
	void TriangleMeshModel::load( const std::string & p_name, const std::string & p_filePath )
	{
		_name = p_name;
		std::cout << "Loading model " << _name << " from: " << p_filePath << std::endl;

		Assimp::Importer importer;

		// Read scene and triangulate meshes
		const aiScene * const scene
			= importer.ReadFile( p_filePath,
								 aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals
									 | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace );

		if ( scene == nullptr )
		{
			throw std::runtime_error( "Fail to load file \" " + p_filePath + "\": " + importer.GetErrorString() );
		}

		_meshes.reserve( scene->mNumMeshes );
		for ( unsigned int i = 0; i < scene->mNumMeshes; ++i )
		{
			_loadMesh( scene->mMeshes[ i ], scene );
		}
		_meshes.shrink_to_fit();
		std::cout << "Done! " << _meshes.size() << " meshes, " << _nbTriangles << " triangles, " << _nbVertices
				  << " vertices" << std::endl;
	}

	void TriangleMeshModel::render( const GLuint p_glProgram ) const
	{
		for ( size_t i = 0; i < _meshes.size(); i++ )
		{
			_meshes[ i ].render( p_glProgram );
		}
	}

	void TriangleMeshModel::cleanGL()
	{
		for ( size_t i = 0; i < _meshes.size(); i++ )
		{
			_meshes[ i ].cleanGL();
		}
	}

	void TriangleMeshModel::_loadMesh( const aiMesh * const p_mesh, const aiScene * const p_scene )
	{
		const std::string meshName = _name + "_" + std::string( p_mesh->mName.C_Str() );
		std::cout << "-- Loading mesh: " << meshName << std::endl;

		// Load vertex attributes.
		std::vector<Vertex> vertices;
		vertices.resize( p_mesh->mNumVertices );
		for ( unsigned int v = 0; v < p_mesh->mNumVertices; ++v )
		{
			Vertex & vertex = vertices[ v ];
			// Position.
			vertex._position.x = p_mesh->mVertices[ v ].x;
			vertex._position.y = p_mesh->mVertices[ v ].y;
			vertex._position.z = p_mesh->mVertices[ v ].z;
			// Normal.
			vertex._normal.x = p_mesh->mNormals[ v ].x;
			vertex._normal.y = p_mesh->mNormals[ v ].y;
			vertex._normal.z = p_mesh->mNormals[ v ].z;
			// Texture coordinates.
			if ( p_mesh->HasTextureCoords( 0 ) )
			{
				vertex._texCoords.x = p_mesh->mTextureCoords[ 0 ][ v ].x;
				vertex._texCoords.y = p_mesh->mTextureCoords[ 0 ][ v ].y;
				// Tangent.
				vertex._tangent.x = p_mesh->mTangents[ v ].x;
				vertex._tangent.y = p_mesh->mTangents[ v ].y;
				vertex._tangent.z = p_mesh->mTangents[ v ].z;
				// Bitangent.
				vertex._bitangent.x = p_mesh->mBitangents[ v ].x;
				vertex._bitangent.y = p_mesh->mBitangents[ v ].y;
				vertex._bitangent.z = p_mesh->mBitangents[ v ].z;
			}
			else
			{
				vertex._texCoords.x = 0.f;
				vertex._texCoords.y = 0.f;
			}
		}

		// Load indices.
		std::vector<unsigned int> indices;
		indices.resize( p_mesh->mNumFaces * 3 ); // Triangulated.
		for ( unsigned int f = 0; f < p_mesh->mNumFaces; ++f )
		{
			const aiFace &	   face = p_mesh->mFaces[ f ];
			const unsigned int f3	= f * 3;
			indices[ f3 ]			= face.mIndices[ 0 ];
			indices[ f3 + 1 ]		= face.mIndices[ 1 ];
			indices[ f3 + 2 ]		= face.mIndices[ 2 ];
		}

		// Load materials.
		const aiMaterial * const mtl = p_scene->mMaterials[ p_mesh->mMaterialIndex ];
		Material				 material;
		if ( mtl == nullptr )
		{
			std::cerr << "Material undefined," << meshName << " assigned to default material" << std::endl;
		}
		else
		{
			aiColor3D color;
			if ( mtl->Get( AI_MATKEY_COLOR_AMBIENT, color ) == AI_SUCCESS )
				material._ambient = Vec3f( color.r, color.g, color.b );
			if ( mtl->Get( AI_MATKEY_COLOR_DIFFUSE, color ) == AI_SUCCESS )
				material._diffuse = Vec3f( color.r, color.g, color.b );
			if ( mtl->Get( AI_MATKEY_COLOR_SPECULAR, color ) == AI_SUCCESS )
				material._specular = Vec3f( color.r, color.g, color.b );
			float shininess;
			if ( mtl->Get( AI_MATKEY_SHININESS, shininess ) == AI_SUCCESS )
				material._shininess = shininess;
		}

		_nbTriangles += p_mesh->mNumFaces;
		_nbVertices += p_mesh->mNumVertices;

		_meshes.push_back( TriangleMesh( meshName, vertices, indices, material ) );

		std::cout << "-- Done! " << indices.size() / 3 << " triangles, " << vertices.size() << " vertices."
				  << std::endl;
	}

} // namespace M3D_ISICG
