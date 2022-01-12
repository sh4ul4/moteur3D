#ifndef __LAB_WORK_5_HPP__
#define __LAB_WORK_5_HPP__

#define WAVES
#define POINTLIGHTS
#define BLOOM

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "common/FreeflyCamera.hpp"
#include "common/TrackballCamera.hpp"
#include "common/models/triangle_mesh_model.hpp"
#include "define.hpp"
#include <map>
#include <vector>
#include "GL/ShaderManager.hpp"

namespace M3D_ISICG
{
	class LabWork5 : public BaseLabWork
	{
	  public:
		LabWork5() :
			BaseLabWork(), basic( "src/lab_works/lab_work_5/shaders/" )
#ifdef BLOOM
			,
			blurr( "src/lab_works/lab_work_5/shaders/" ), bloom( "src/lab_works/lab_work_5/shaders/" )
#endif
		{
		}
		~LabWork5();

		ShaderManager basic;
		float		  scale = 0.01;
		bool		  cameratype = false;
#ifdef BLOOM
		ShaderManager blurr;
		ShaderManager bloom;
		GLuint		  hdrFBO;
		GLuint		  tmpTexture[ 2 ];
		GLuint		  pingpongFBO[ 2 ];
		GLuint		  pingpongtmpTexture[ 2 ];
		unsigned int  attachments[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		unsigned int  rboDepth;
#endif

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;
		void renderModel( TriangleMeshModel & model );

		void updateMatrices( const Mat4f & model, const Mat4f & view, const Mat4f & projection );

		void updateuMVPmatrix();
		void updateuNormalMatrix();
		void updateuMVMatrix();
		void updateuMMatrix( const TriangleMeshModel & model );
		void setLightPosition( const Vec3f & pos );
		void updateLightPosition();

#ifdef POINTLIGHTS
		void initPointLights();
		float constant = 20, linear = 500, quadratic = 1000;
#endif

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void resize( const int p_width, const int p_height ) override;

	  private:
		bool _initProgram();
		void _initCamera();

	  private:
		// ================ Scene data.
		TriangleMeshModel _bunny_2;
		TriangleMeshModel _conference;
		TriangleMeshModel _sponza;
		TriangleMeshModel _splash;

		std::vector<BaseCamera *> _cameras;
		BaseCamera *			  _camera		 = nullptr;
		Vec3f					  _lightPosition = Vec3f( 0.f );
		// ================

		// ================ GL data.
		GLuint _program			 = GL_INVALID_INDEX;
		GLint  _uMVPMatrixLoc	 = GL_INVALID_INDEX;
		GLint  _uNormalMatrixLoc = GL_INVALID_INDEX;
		GLint  _uMVMatrixLoc	 = GL_INVALID_INDEX;
		GLint  _uMMatrixLoc		 = GL_INVALID_INDEX;

		GLint _uViewLightPositionLoc = GL_INVALID_INDEX;

		GLint _uModelMatrixLoc		= GL_INVALID_INDEX;
		GLint _uViewMatrixLoc		= GL_INVALID_INDEX;
		GLint _uProjectionMatrixLoc = GL_INVALID_INDEX;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// Camera
		float _cameraSpeed		 = 0.1f;
		float _cameraSensitivity = 0.1f;
		float _fovy				 = 60.f;
		// ================

#ifdef POINTLIGHTS
		class PointLight
		{
		  public:
			GLint posLoc   = GL_INVALID_INDEX;
			GLint constLoc = GL_INVALID_INDEX;
			GLint linLoc   = GL_INVALID_INDEX;
			GLint quadLoc  = GL_INVALID_INDEX;
			GLint ambLoc   = GL_INVALID_INDEX;
			GLint difLoc   = GL_INVALID_INDEX;
			GLint specLoc  = GL_INVALID_INDEX;
			Vec3f position;
			float constant;
			float linear;
			float quadratic;
			Vec3f ambient;
			Vec3f diffuse;
			Vec3f specular;

		  public:
			void locate( GLuint _program, std::string number );
			void set( GLuint _program ) const;
			void updatePos( GLuint _program, BaseCamera * _camera );
		};
		std::vector<PointLight> pointLights;
#endif
		float timer = 0;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_4_HPP__
