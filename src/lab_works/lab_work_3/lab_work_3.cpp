#include "lab_work_3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "utils/random.hpp"
#include "utils/read_file.hpp"
#include <iostream>

namespace M3D_ISICG
{
	const std::string LabWork3::_shaderFolder = "src/lab_works/lab_work_3/shaders/";

	LabWork3::~LabWork3() { glDeleteProgram( _program ); }

	bool LabWork3::init()
	{
		std::cout << "Initializing lab work 3..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		if ( !_initProgram() )
			return false;
		
		glUseProgram( _program );

		_initCamera();

		_cubes.emplace_back( _createCube( Vec3f( 0, 0, 1 ) ) );
		_cubes.emplace_back( _createCube( Vec3f( 0, 1, 0 ) ) );
		_cubes.emplace_back( _createCube( Vec3f( 1, 1, 0 ) ) );
		
		
		for ( Mesh& cube : _cubes )
			_initBuffers( cube );
		
		_updateViewMatrix();
		_updateProjectionMatrix();
		
		// init scene
		_cubes[ 1 ]._transformation = glm::translate( _cubes[ 1 ]._transformation, glm::vec3( 10.f, 0.f, 0.f ) );

		_cubes[ 2 ]._transformation = glm::translate( _cubes[ 2 ]._transformation, glm::vec3( 10.f, 10.f, 0.f ) );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork3::animate( const float p_deltaTime )
	{
		_cubes[ 1 ]._transformation = glm::translate( _cubes[ 1 ]._transformation, glm::vec3( -10.f, 0.f, 0.f ) );
		_cubes[ 1 ]._transformation
			= glm::rotate( _cubes[ 1 ]._transformation, glm::radians( p_deltaTime * 10 ), glm::vec3( 0.f, 1.f, 0.f ) );
		_cubes[ 1 ]._transformation = glm::translate( _cubes[ 1 ]._transformation, glm::vec3( 10.f, 0.f, 0.f ) );
		glProgramUniformMatrix4fv(
			_program, _uModelMatrixLoc, 1, false, glm::value_ptr( _cubes[ 1 ]._transformation ) );
	}

	void LabWork3::render()
	{
		// Clear the color buffer.
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		for (const Mesh& cube : _cubes) {
			glBindVertexArray( cube._vao );
			glProgramUniformMatrix4fv( _program, _uModelMatrixLoc, 1, false, glm::value_ptr( cube._transformation ) );
			glDrawElements( GL_TRIANGLES, cube._indices.size(), GL_UNSIGNED_INT, 0 );
			glBindVertexArray( 0 );
		}
		
	}

	void LabWork3::handleEvents( const SDL_Event & p_event )
	{
		if ( p_event.type == SDL_KEYDOWN )
		{
			switch ( p_event.key.keysym.scancode )
			{
			case SDL_SCANCODE_W: // Front
				_camera.moveFront( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_S: // Back
				_camera.moveFront( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_A: // Left
				_camera.moveRight( -_cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_D: // Right
				_camera.moveRight( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_R: // Up
				_camera.moveUp( _cameraSpeed );
				_updateViewMatrix();
				break;
			case SDL_SCANCODE_F: // Bottom
				_camera.moveUp( -_cameraSpeed );
				_updateViewMatrix();
				break;
			default: break;
			}
		}

		// Rotate when left click + motion (if not on Imgui widget).
		if ( p_event.type == SDL_MOUSEMOTION && p_event.motion.state & SDL_BUTTON_LMASK
			 && !ImGui::GetIO().WantCaptureMouse )
		{
			_camera.rotate( p_event.motion.xrel * _cameraSensitivity, p_event.motion.yrel * _cameraSensitivity );
			_updateViewMatrix();
		}
	}

	void LabWork3::displayUI()
	{
		ImGui::Begin( "Settings lab work 3" );

		// Background.
		if ( ImGui::ColorEdit3( "Background", glm::value_ptr( _bgColor ) ) )
		{
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		}

		// Camera.
		if ( ImGui::SliderFloat( "fovy", &_fovy, 10.f, 160.f, "%01.f" ) )
		{
			_camera.setFovy( _fovy );
			_updateProjectionMatrix();
		}
		if ( ImGui::SliderFloat( "Speed", &_cameraSpeed, 0.1f, 10.f, "%01.1f" ) )
		{
			_camera.setFovy( _fovy );
			_updateProjectionMatrix();
		}
		if ( ImGui::Button( "add cube", { 200.f, 20.f } ) )
		{
			_cubes.emplace_back( _createCube( Vec3f( 1, 0, 0 ) ) );
			_cubes[ _cubes.size() - 1 ]._transformation
				= glm::translate( _cubes[ _cubes.size() - 1 ]._transformation, glm::vec3( -10.f, 0.f, 0.f ) );
			for ( Mesh & cube : _cubes )
				_initBuffers( cube );
		}

		ImGui::End();
	}

	void LabWork3::resize( const int p_width, const int p_height )
	{
		BaseLabWork::resize( p_width, p_height );
		_camera.setScreenSize( p_width, p_height );
	}

	bool LabWork3::_initProgram()
	{
		// ====================================================================
		// Shaders.
		// ====================================================================
		// Create shaders.
		const GLuint vertexShader	= glCreateShader( GL_VERTEX_SHADER );
		const GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

		// Get sources from files.
		const std::string vertexShaderSrc	= readFile( _shaderFolder + "triangle.vert" );
		const std::string fragmentShaderSrc = readFile( _shaderFolder + "triangle.frag" );

		// Convert to GLchar *
		const GLchar * vSrc = vertexShaderSrc.c_str();
		const GLchar * fSrc = fragmentShaderSrc.c_str();

		// Compile vertex shader.
		glShaderSource( vertexShader, 1, &vSrc, NULL );
		glCompileShader( vertexShader );
		// Check if compilation is ok.
		GLint compiled;
		glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compiled );
		if ( !compiled )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( vertexShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std::cerr << "Error compiling vertex shader: " << log << std::endl;
			return false;
		}

		// Compile vertex shader.
		glShaderSource( fragmentShader, 1, &fSrc, NULL );
		glCompileShader( fragmentShader );
		// Check if compilation is ok.
		glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiled );
		if ( compiled != GL_TRUE )
		{
			GLchar log[ 1024 ];
			glGetShaderInfoLog( fragmentShader, sizeof( log ), NULL, log );
			glDeleteShader( vertexShader );
			glDeleteShader( fragmentShader );
			std::cerr << "Error compiling fragment shader: " << log << std::endl;
			return false;
		}
		// ====================================================================

		// ====================================================================
		// Program.
		// ====================================================================
		// Create program.
		_program = glCreateProgram();

		// Attach shaders.
		glAttachShader( _program, vertexShader );
		glAttachShader( _program, fragmentShader );

		// Link program.
		glLinkProgram( _program );
		// Check if link is ok.
		GLint linked;
		glGetProgramiv( _program, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( _program, sizeof( log ), NULL, log );
			std::cerr << "Error linking program: " << log << std::endl;
			return false;
		}

		// Shaders are now useless.
		glDeleteShader( vertexShader );
		glDeleteShader( fragmentShader );
		// ====================================================================

		// ====================================================================
		// Get uniform locations.
		// ====================================================================
		_uModelMatrixLoc	  = glGetUniformLocation( _program, "uModelMatrix" );
		_uViewMatrixLoc		  = glGetUniformLocation( _program, "uViewMatrix" );
		_uProjectionMatrixLoc = glGetUniformLocation( _program, "uProjectionMatrix" );
		// ====================================================================
		glEnable( GL_DEPTH_TEST );
		return true;
	}

	void LabWork3::_initCamera()
	{
		_camera.setScreenSize( _windowWidth, _windowHeight );
		_camera.setPosition( Vec3f( 0, 1, 3 ) );
	}

	void LabWork3::_initBuffers(Mesh& _cube)
	{
		//	vao init
		glCreateVertexArrays( 1, &_cube._vao );
		glEnableVertexArrayAttrib( _cube._vao, 0 );
		glEnableVertexArrayAttrib( _cube._vao, 1 );

		//	vbo init
		glCreateBuffers( 1, &_cube._vboPositions );
		glNamedBufferData(
			_cube._vboPositions, _cube._vertices.size() * sizeof( Vec3f ), _cube._vertices.data(), GL_STATIC_DRAW );
		// vbo-vao link
		glVertexArrayAttribFormat( _cube._vao, 0, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _cube._vao, 0, _cube._vboPositions, 0, sizeof( Vec3f ) );

		// vbo_colors init
		glCreateBuffers( 1, &_cube._vboColors );
		glNamedBufferData( _cube._vboColors,
						   _cube._vertexColors.size() * sizeof( Vec3f ),
						   _cube._vertexColors.data(),
						   GL_STATIC_DRAW );
		// vbo_colors-vao link
		glVertexArrayAttribFormat( _cube._vao, 1, 3, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( _cube._vao, 1, _cube._vboColors, 0, sizeof( Vec3f ) );

		// ebo init
		glCreateBuffers( 1, &_cube._ebo );
		glNamedBufferData(
			_cube._ebo, _cube._indices.size() * sizeof( GLuint ), _cube._indices.data(), GL_STATIC_DRAW );

		// ebo-vao link
		glVertexArrayElementBuffer( _cube._vao, _cube._ebo );
	}

	void LabWork3::_updateViewMatrix()
	{
		glProgramUniformMatrix4fv( _program, _uViewMatrixLoc, 1, false, glm::value_ptr( _camera.getViewMatrix() ) );
	}

	void LabWork3::_updateProjectionMatrix()
	{
		glProgramUniformMatrix4fv( _program, _uProjectionMatrixLoc, 1, false, glm::value_ptr( _camera.getProjectionMatrix() ) );
	}

	LabWork3::Mesh LabWork3::_createCube(Vec3f col)
	{
		GLfloat x = 2;
		Mesh m;
		m._vertices.emplace_back( -x, x, x );		// a 0
		m._vertices.emplace_back( -x, -x, x );		// b 1
		m._vertices.emplace_back( x, -x, x );		// d 3
		m._vertices.emplace_back( x, x, x );		// c 2
		m._vertices.emplace_back( -x, x, -x );		// e 4
		m._vertices.emplace_back( -x, -x, -x );		// f 5
		m._vertices.emplace_back( x, -x, -x );		// h 7
		m._vertices.emplace_back( x, x, -x );		// g 6

		//front
		m._indices.emplace_back( 0 );
		m._indices.emplace_back( 3 );
		m._indices.emplace_back( 2 );

		m._indices.emplace_back( 0 );
		m._indices.emplace_back( 2 );
		m._indices.emplace_back( 1 );
		//right
		m._indices.emplace_back( 3 );
		m._indices.emplace_back( 7 );
		m._indices.emplace_back( 6 );

		m._indices.emplace_back( 3 );
		m._indices.emplace_back( 6 );
		m._indices.emplace_back( 2 );
		//back
		m._indices.emplace_back( 7 );
		m._indices.emplace_back( 4 );
		m._indices.emplace_back( 5 );

		m._indices.emplace_back( 7 );
		m._indices.emplace_back( 5 );
		m._indices.emplace_back( 6 );
		//left
		m._indices.emplace_back( 4 );
		m._indices.emplace_back( 0 );
		m._indices.emplace_back( 5 );

		m._indices.emplace_back( 0 );
		m._indices.emplace_back( 1 );
		m._indices.emplace_back( 5 );
		//top
		m._indices.emplace_back( 0 );
		m._indices.emplace_back( 4 );
		m._indices.emplace_back( 7 );

		m._indices.emplace_back( 0 );
		m._indices.emplace_back( 7 );
		m._indices.emplace_back( 3 );
		//bottom
		m._indices.emplace_back( 1 );
		m._indices.emplace_back( 2 );
		m._indices.emplace_back( 6 );

		m._indices.emplace_back( 1 );
		m._indices.emplace_back( 6 );
		m._indices.emplace_back( 5 );

		for ( GLuint i = 0; i < m._indices.size(); i++ )
		{
			GLfloat val = 1.0f / m._indices.size() * i;
			val *= 6;
			if ( val > 1 ) val = 1;
			m._vertexColors.emplace_back( col.x * val, col.y * val, col.z * val );
		}
		return m;
	}
} // namespace M3D_ISICG