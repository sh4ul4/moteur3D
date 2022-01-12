#include "lab_work_5.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
#ifdef BLOOM
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	void		 renderQuad()
	{
		if ( quadVAO == 0 ) // first iteration
		{
			float quadVertices[] = {
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
			};
			glGenVertexArrays( 1, &quadVAO );
			glGenBuffers( 1, &quadVBO );
			glBindVertexArray( quadVAO );
			glBindBuffer( GL_ARRAY_BUFFER, quadVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), &quadVertices, GL_STATIC_DRAW );
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), NULL );
			glEnableVertexAttribArray( 1 );
			glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( float ), (void *)( 3 * sizeof( float ) ) );
		}
		glBindVertexArray( quadVAO );
		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		glBindVertexArray( 0 );
	}
#endif

	LabWork5::~LabWork5()
	{
		for ( BaseCamera * it : _cameras )
			delete it;
		_cameras.clear();
		_camera = nullptr;
		_sponza.cleanGL();
	}

	bool LabWork5::init()
	{
		std::cout << "Initializing lab work 5..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		_bgColor = Vec4f( 11.f / 255.f, 12.f / 255.f, 30.f / 255.f, 1.f );
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		glEnable( GL_DEPTH_TEST );
#ifdef BLOOM
		unsigned int attachments[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers( 2, attachments );
#endif
		if ( _camera == nullptr )
		{
			_cameras.emplace_back( new FreeflyCamera() );
			_cameras.emplace_back( new TrackballCamera() );
			_camera = _cameras[ cameratype ];
		}
		if ( !_initProgram() )
			return false;
		_initCamera();
		basic.use( _program );
		_sponza.load( "sponza", "./sponza/sponza.obj" );
		for ( size_t i = 320; i < 331; i++ )
		_sponza._meshes[ i ]._material._liquid = true;

		_sponza._transformation = glm::scale( _sponza._transformation, glm::vec3( scale,scale,scale ) );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork5::animate( const float p_deltaTime ) {}

	void LabWork5::renderModel(TriangleMeshModel& model) {
		// Clear the color buffer.
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		_camera->_computeUMVPMatrix( model._transformation );
		updateuMVPmatrix();
		_camera->_computeUMVMatrix( model._transformation );
		updateuMVMatrix();
		updateuMMatrix( model );
		updateuNormalMatrix();
		updateMatrices( model._transformation, _camera->getViewMatrix(), _camera->getProjectionMatrix() );
#ifdef POINTLIGHTS
		for ( int i = 0; i < 4; i++ )
			pointLights[ i ].updatePos( _program, _camera );
#endif
		model.update();
		model.render( _program );
	}

	void LabWork5::render()
	{
#ifdef BLOOM
		glBindFramebuffer( GL_FRAMEBUFFER, hdrFBO );
#endif
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		basic.use( _program );
#ifdef BLOOM
		glActiveTexture( GL_TEXTURE0 );
#endif
		timer += 0.00001;
		timer = fmod( timer, 1000.f );
		glProgramUniform1f( _program, glGetUniformLocation( _program, "uTimer" ), timer );
		renderModel( _sponza );
#ifdef BLOOM
		// actual blurring
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		bool		 hori	 = true;
		bool		 initial = true;
		blurr.use( _program );
		for ( unsigned int i = 0; i < 10; i++ )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, pingpongFBO[ hori ] );
			blurr.setInt( "horizontal", hori );
			glBindTexture( GL_TEXTURE_2D, initial ? tmpTexture[ 1 ] : pingpongtmpTexture[ !hori ] );
			renderQuad();
			hori = !hori;
			if ( initial )
				initial = false;
		}
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		bloom.use( _program );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, tmpTexture[ 0 ] );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, pingpongtmpTexture[ !hori ] );
		bloom.setInt( "bloom", true );
		bloom.setFloat( "exposure", 1.f );
		renderQuad();
		basic.use( _program );
#endif
	}

	void LabWork5::setLightPosition( const Vec3f & pos ) { _lightPosition = pos; }

	void LabWork5::updateLightPosition()
	{
		const Vec3f viewLightPosition = _camera->getViewMatrix() * Vec4f( _lightPosition, 1.0f );
		glProgramUniform3fv( _program, _uViewLightPositionLoc, 1, glm::value_ptr( viewLightPosition ) );
	}

	void LabWork5::updateMatrices( const Mat4f & model, const Mat4f & view, const Mat4f & projection )
	{
		glProgramUniformMatrix4fv( _program, _uModelMatrixLoc, 1, false, glm::value_ptr( model ) );
		glProgramUniformMatrix4fv( _program, _uViewMatrixLoc, 1, false, glm::value_ptr( view ) );
		glProgramUniformMatrix4fv( _program, _uProjectionMatrixLoc, 1, false, glm::value_ptr( projection ) );
	}

	void LabWork5::updateuMVPmatrix()
	{
		glProgramUniformMatrix4fv( _program, _uMVPMatrixLoc, 1, false, glm::value_ptr( _camera->getUMVPMatrix() ) );
	}

	void LabWork5::updateuMVMatrix()
	{
		glProgramUniformMatrix4fv( _program, _uMVMatrixLoc, 1, false, glm::value_ptr( _camera->getUMVMatrix() ) );
	}

	void LabWork5::updateuMMatrix( const TriangleMeshModel & model )
	{
		glProgramUniformMatrix4fv( _program, _uMMatrixLoc, 1, false, glm::value_ptr( model._transformation ) );
	}

	void LabWork5::updateuNormalMatrix()
	{
		glProgramUniformMatrix4fv( _program,
								   _uNormalMatrixLoc,
								   1,
								   false,
								   glm::value_ptr( glm::transpose( glm::inverse( _camera->getUMVMatrix() ) ) ) );
	}

	void LabWork5::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera->moveFront( _cameraSpeed );
				break;
			case SDL_SCANCODE_S: // Back
				_camera->moveFront( -_cameraSpeed );
				break;
			case SDL_SCANCODE_A: // Left
				_camera->moveRight( -_cameraSpeed );
				break;
			case SDL_SCANCODE_D: // Right
				_camera->moveRight( _cameraSpeed );
				break;
			case SDL_SCANCODE_R: // Up
				_camera->moveUp( _cameraSpeed );
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera->moveUp( -_cameraSpeed );
				break;
			case SDL_SCANCODE_SPACE: // Print camera info
				_camera->print();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera->rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
		}
	}

	void LabWork5::displayUI()
	{
		ImGui::Begin( "Settings lab work 5" );

		// Background.
		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		// Camera.
		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) )
		{
			_camera->setFovy( _fovy );
		}
		if ( ImGui::SliderFloat( "Speed", &_cameraSpeed, 0.1f, 10.f, "%01.1f" ) ) {}
		if ( ImGui::Button( "cameratype",ImVec2(100,20) ) )
		{
			cameratype = !cameratype;
			_camera	   = _cameras[ cameratype ];
			_cameras[ cameratype ]->setPosition( _cameras[ !cameratype ]->getPosition() );
			_cameras[ cameratype ]->setFovy( _cameras[ !cameratype ]->getFovy() );
		}
		if ( ImGui::SliderFloat( "TrackballCamera::distance_to_subject", &dynamic_cast<TrackballCamera*>(_cameras[1])->_subjectDistance, 0.1f, 100.f, "%0.1f" ) ) {
			_camera->update();
		}
#ifdef POINTLIGHTS
		if ( ImGui::SliderFloat( "constant", &constant, 0.01f, 2000.f, "%01.01f" ) )
		{
			for ( int i = 0; i < 4; i++ )
			{
				pointLights[ i ].constant = constant;
			}
		}
		if ( ImGui::SliderFloat( "linear", &linear, 0.01f, 2000.f, "%01.01f" ) )
		{
			for ( int i = 0; i < 4; i++ )
			{
				pointLights[ i ].linear = linear;
			}
		}
		if ( ImGui::SliderFloat( "quadratic", &quadratic, 0.01f, 2000.f, "%01.01f" ) )
		{
			for ( int i = 0; i < 4; i++ )
			{
				pointLights[ i ].quadratic = quadratic;
			}
		}
#endif
		ImGui::End();
	}

	void LabWork5::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		if ( _camera == nullptr )
		{
			_cameras.clear();
			_cameras.emplace_back( new FreeflyCamera() );
			_cameras.emplace_back( new TrackballCamera() );
			_camera = _cameras[ cameratype ];
		}
		for (auto cam : _cameras)
			cam->setScreenSize( p_width, p_height );
	}

	bool LabWork5::_initProgram()
	{
		// ====================================================================
		// Shaders.
		// ====================================================================
		// Create shaders.
		basic.init();
		basic.addShader( "vertex", GL_VERTEX_SHADER, "mesh.vert" );
		basic.addShader( "fragment", GL_FRAGMENT_SHADER, "mesh.frag" );
		basic.getShader( "vertex" )->compile();
		basic.getShader( "fragment" )->compile();
		basic.link();
		basic.use( _program );
		basic.shaders.clear();
#ifdef BLOOM
		blurr.init();
		blurr.addShader( "fragment", GL_FRAGMENT_SHADER, "blurr.frag" );
		blurr.addShader( "vertex", GL_VERTEX_SHADER, "blurr.vert" );
		blurr.getShader( "fragment" )->compile();
		blurr.getShader( "vertex" )->compile();
		blurr.link();
		blurr.shaders.clear();

		bloom.init();
		bloom.addShader( "fragment", GL_FRAGMENT_SHADER, "bloom.frag" );
		bloom.addShader( "vertex", GL_VERTEX_SHADER, "bloom.vert" );
		bloom.getShader( "fragment" )->compile();
		bloom.getShader( "vertex" )->compile();
		bloom.link();
		bloom.shaders.clear();
#endif
		// ====================================================================

		// ====================================================================
		// Get uniform locations.
		// ====================================================================
		_uNormalMatrixLoc = glGetUniformLocation( _program, "uNormalMatrix" );
		_uMVPMatrixLoc	  = glGetUniformLocation( _program, "uMVPMatrix" );
		_uMVMatrixLoc	  = glGetUniformLocation( _program, "uMVMatrix" );

		_uViewLightPositionLoc = glGetUniformLocation( _program, "uViewLightPos" );

		_uModelMatrixLoc	  = glGetUniformLocation( _program, "uModelMatrix" );
		_uViewMatrixLoc		  = glGetUniformLocation( _program, "uViewMatrix" );
		_uProjectionMatrixLoc = glGetUniformLocation( _program, "uProjectionMatrix" );
		// ====================================================================
		// Init lighting environment.
		// ====================================================================
#ifdef POINTLIGHTS
		initPointLights();
#endif
		// ====================================================================
		// Init bloom.
		// ====================================================================
#ifdef BLOOM
		glGenFramebuffers( 1, &hdrFBO );
		glBindFramebuffer( GL_FRAMEBUFFER, hdrFBO );
		glGenTextures( 2, tmpTexture );
		for ( unsigned int i = 0; i < 2; i++ )
		{
			glBindTexture( GL_TEXTURE_2D, tmpTexture[ i ] );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, _windowWidth, _windowHeight, 0, GL_RGBA, GL_FLOAT, NULL );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tmpTexture[ i ], 0 );
		}
		
		glGenRenderbuffers( 1, &rboDepth );
		glBindRenderbuffer( GL_RENDERBUFFER, rboDepth );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _windowWidth, _windowHeight );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth );
		
		glDrawBuffers( 2, attachments );
		if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
			std::cout << "Problem with Bloom : Framebuffer!" << std::endl;
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		glGenFramebuffers( 2, pingpongFBO );
		glGenTextures( 2, pingpongtmpTexture );
		for ( unsigned int i = 0; i < 2; i++ )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, pingpongFBO[ i ] );
			glBindTexture( GL_TEXTURE_2D, pingpongtmpTexture[ i ] );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, _windowWidth, _windowHeight, 0, GL_RGBA, GL_FLOAT, NULL );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongtmpTexture[ i ], 0 );
			if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
				std::cout << "Problem with Bloom : Framebuffer!" << std::endl;
		}

		blurr.use( _program );
		blurr.setInt( "image", 0 );
		bloom.use( _program );
		bloom.setInt( "scene", 0 );
		bloom.setInt( "bloomBlur", 1 );
#endif
		return true;
	}

#ifdef POINTLIGHTS
	void LabWork5::PointLight::updatePos( GLuint _program, BaseCamera * _camera )
	{
		for ( int i = 0; i < 4; i++ )
		{
			Vec4f res  = _camera->getUMVMatrix() * Vec4f( position, 1.f );
			Vec3f tmp  = position;
			position.x = res.x;
			position.y = res.y;
			position.z = res.z;
			set( _program );
			position = tmp;
		}
	}
	void LabWork5::PointLight::locate( GLuint _program, std::string number )
	{
		posLoc	 = glGetUniformLocation( _program, std::string( "uPointLights[" + number + "].position" ).c_str() );
		constLoc = glGetUniformLocation( _program, std::string( "uPointLights[" + number + "].constant" ).c_str() );
		linLoc	 = glGetUniformLocation( _program, std::string( "uPointLights[" + number + "].linear" ).c_str() );
		quadLoc	 = glGetUniformLocation( _program, std::string( "uPointLights[" + number + "].quadratic" ).c_str() );
		ambLoc	 = glGetUniformLocation( _program, std::string( "uPointLights[" + number + "].ambient" ).c_str() );
		difLoc	 = glGetUniformLocation( _program, std::string( "uPointLights[" + number + "].diffuse" ).c_str() );
		specLoc	 = glGetUniformLocation( _program, std::string( "uPointLights[" + number + "].specular" ).c_str() );
	}
	void LabWork5::PointLight::set( GLuint _program ) const
	{
		glProgramUniform3fv( _program, posLoc, 1, glm::value_ptr( position ) );
		glProgramUniform1f( _program, constLoc, constant );
		glProgramUniform1f( _program, linLoc, linear );
		glProgramUniform1f( _program, quadLoc, quadratic );
		glProgramUniform3fv( _program, ambLoc, 1, glm::value_ptr( ambient ) );
		glProgramUniform3fv( _program, difLoc, 1, glm::value_ptr( diffuse ) );
		glProgramUniform3fv( _program, specLoc, 1, glm::value_ptr( specular ) );
	}
	void LabWork5::initPointLights()
	{
		for ( int i = 0; i < 4; i++ )
			pointLights.emplace_back();
		pointLights[ 0 ].position = glm::vec3( 490.f, 128.f, 142.f );
		pointLights[ 1 ].position = glm::vec3( 490.f, 128.f, -220.f );
		pointLights[ 2 ].position = glm::vec3( -620.0f, 128.0f, -220.0f );
		pointLights[ 3 ].position = glm::vec3( -620.0f, 128.0f, 142.0f );
		// point light 1
		pointLights[ 0 ].ambient   = glm::vec3( 255.f, 120.f, 18.f );
		pointLights[ 0 ].diffuse   = glm::vec3( 255.f, 160.f, 18.f );
		pointLights[ 0 ].specular  = glm::vec3( 255.0f, 240.0f, 100.0f );
		pointLights[ 0 ].constant  = constant;
		pointLights[ 0 ].linear	   = linear;
		pointLights[ 0 ].quadratic = quadratic;
		// point light 2
		pointLights[ 1 ].ambient   = glm::vec3( 255.f, 120.f, 18.f );
		pointLights[ 1 ].diffuse   = glm::vec3( 255.f, 160.f, 18.f );
		pointLights[ 1 ].specular  = glm::vec3( 255.0f, 240.0f, 100.0f );
		pointLights[ 1 ].constant  = constant;
		pointLights[ 1 ].linear	   = linear;
		pointLights[ 1 ].quadratic = quadratic;
		// point light 3
		pointLights[ 2 ].ambient   = glm::vec3( 255.f, 120.f, 18.f );
		pointLights[ 2 ].diffuse   = glm::vec3( 255.f, 160.f, 18.f );
		pointLights[ 2 ].specular  = glm::vec3( 255.0f, 240.0f, 100.0f );
		pointLights[ 2 ].constant  = constant;
		pointLights[ 2 ].linear	   = linear;
		pointLights[ 2 ].quadratic = quadratic;
		// point light 4
		pointLights[ 3 ].ambient   = glm::vec3( 255.f, 120.f, 18.f );
		pointLights[ 3 ].diffuse   = glm::vec3( 255.f, 160.f, 18.f );
		pointLights[ 3 ].specular  = glm::vec3( 255.0f, 240.0f, 100.0f );
		pointLights[ 3 ].constant  = constant;
		pointLights[ 3 ].linear	   = linear;
		pointLights[ 3 ].quadratic = quadratic;
		for ( int i = 0; i < 4; i++ )
		{
			pointLights[ i ].locate( _program, std::to_string( i ) );
			pointLights[ i ].set( _program );
		}
	}
#endif

	void LabWork5::_initCamera()
	{
		_camera->setScreenSize( _windowWidth, _windowHeight );
		_camera->setPosition( Vec3f( 0.f, 1.f, 0.f ) );
	}
} // namespace M3D_ISICG
