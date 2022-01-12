#include "lab_work_1.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>

#include "GL/Shader.hpp"

namespace M3D_ISICG
{
	const std::string LabWork1::_shaderFolder = "src/lab_works/lab_work_1/shaders/";

	LabWork1::~LabWork1()
	{
		glDeleteProgram( programId );
		glDeleteBuffers( 1, &VBOid );
		glDisableVertexArrayAttrib( VAOid, 0 );
		glDeleteVertexArrays( 1, &VAOid );
	}

	bool checkProgramLink1(const GLuint& programId) {
		GLint linked;
		glGetProgramiv( programId, GL_LINK_STATUS, &linked );
		if ( !linked )
		{
			GLchar log[ 1024 ];
			glGetProgramInfoLog( programId, sizeof( log ), NULL, log );
			std ::cerr << " Error linking program : " << log << std ::endl;
			return false;
		}
		return true;
	}

	bool LabWork1::init() {
		std::cout << "Initializing lab work 1..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// init shaders
		shader vertexShader( "vertex", readFile( _shaderFolder + "lw1.vert" ), GL_VERTEX_SHADER );
		if ( !vertexShader.compile() )
			return false;
		shader fragmentShader( "fragment", readFile( _shaderFolder + "lw1.frag" ), GL_FRAGMENT_SHADER );
		if ( !fragmentShader.compile() )
			return false;
		
		// create program
		programId = glCreateProgram();

		// attach & link
		vertexShader.attachToProgram( programId );
		fragmentShader.attachToProgram( programId );
		glLinkProgram( programId );

		// Check if link is ok.
		if ( !checkProgramLink1( programId ) )
			return false;

		// delete shaders
		vertexShader.del();
		fragmentShader.del();

		// init vertices values
		vertices.emplace_back( 0.5, -0.5 );
		vertices.emplace_back( -0.5, 0.5 );
		vertices.emplace_back( 0.5, 0.5 );
		
		// GL-functions
		glCreateBuffers( 1, &VBOid );
		glNamedBufferData( VBOid, vertices.size() * sizeof( Vec2f ), vertices.data(), GL_STATIC_DRAW );

		glCreateVertexArrays( 1, &VAOid );

		glEnableVertexArrayAttrib( VAOid, 0 );

		glVertexArrayAttribFormat( VAOid, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( VAOid, 0, VBOid, 0, sizeof( Vec2f) );

		glUseProgram( programId );

		glDisable( GL_DEPTH_TEST );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork1::animate( const float p_deltaTime ) {}

	void LabWork1::render() { 
		glClear( GL_COLOR_BUFFER_BIT );
		glBindVertexArray( VAOid );
		glDrawArrays( GL_TRIANGLES, 0, 3 );
		glBindVertexArray( 0 );
	}

	void LabWork1::handleEvents( const SDL_Event & p_event ) {}

	void LabWork1::displayUI()
	{
		ImGui::Begin( "Settings lab work 1" );
		ImGui::Text( "No setting available!" );
		ImGui::End();
	}

} // namespace M3D_ISICG
