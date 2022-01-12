#include "lab_work_2.hpp"
#include "imgui.h"
#include "utils/read_file.hpp"
#include <iostream>
#include "glm/gtc/type_ptr.hpp"
#include "utils/random.hpp"

#include "GL/Shader.hpp"

namespace M3D_ISICG
{
	const std::string LabWork2::_shaderFolder = "src/lab_works/lab_work_2/shaders/";

	LabWork2::~LabWork2()
	{
		glDeleteProgram( programId );
		glDeleteBuffers( 1, &VBOid );
		glDeleteBuffers( 1, &VBOcolid );
		glDisableVertexArrayAttrib( VAOid, 0 );
		glDeleteVertexArrays( 1, &VAOid );
		glDeleteBuffers( 1, &EBOid );
	}

	bool checkProgramLink2( const GLuint & programId )
	{
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

	bool LabWork2::init()
	{
		std::cout << "Initializing lab work 2..." << std::endl;
		// Set the color used by glClear to clear the color buffer (in render()).
		glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );

		// init shaders
		shader vertexShader( "vertex", readFile( _shaderFolder + "lw2.vert" ), GL_VERTEX_SHADER );
		if ( !vertexShader.compile() )
			return false;
		shader fragmentShader( "fragment", readFile( _shaderFolder + "lw2.frag" ), GL_FRAGMENT_SHADER );
		if ( !fragmentShader.compile() )
			return false;

		// create program
		programId = glCreateProgram();

		// attach & link
		vertexShader.attachToProgram( programId );
		fragmentShader.attachToProgram( programId );
		glLinkProgram( programId );

		// Check if link is ok.
		if ( !checkProgramLink2( programId ) )
			return false;

		// delete shaders
		vertexShader.del();
		fragmentShader.del();

		// init colors
		colors.emplace_back( 0.0f, 0.0f, 1.0f, 1.0f );
		colors.emplace_back( 1.0f, 0.0f, 0.0f, 1.0f );
		colors.emplace_back( 0.0f, 1.0f, 0.0f, 1.0f ); 
		colors.emplace_back( 1.0f, 0.0f, 1.0f, 1.0f );

		// init vertices values and indices
		generateCircle( Vec2f( 0.3, -0.2 ), 64, 0.5 );

		/* GL - functions */
		//	vao init
		glCreateVertexArrays( 1, &VAOid );
		glEnableVertexArrayAttrib( VAOid, 0 );
		glEnableVertexArrayAttrib( VAOid, 1 );

		//	vbo init
		glCreateBuffers( 1, &VBOid );
		glNamedBufferData( VBOid, vertices.size() * sizeof( Vec2f ), vertices.data(), GL_STATIC_DRAW );
		// vbo-vao link
		glVertexArrayAttribFormat( VAOid, 0, 2, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( VAOid, 0, VBOid, 0, sizeof( Vec2f ) );

		// vbo_colors init
		glCreateBuffers( 1, &VBOcolid );
		glNamedBufferData( VBOcolid, colors.size() * sizeof( Vec4f ), colors.data(), GL_STATIC_DRAW );
		// vbo_colors-vao link
		glVertexArrayAttribFormat( VAOid, 1, 4, GL_FLOAT, GL_FALSE, 0 );
		glVertexArrayVertexBuffer( VAOid, 1, VBOcolid, 0, sizeof( Vec4f ) );

		// ebo init
		glCreateBuffers( 1, &EBOid );
		glNamedBufferData( EBOid, vindex.size() * sizeof( GLuint ), vindex.data(), GL_STATIC_DRAW );

		// ebo-vao link
		glVertexArrayElementBuffer( VAOid, EBOid );

		// program init
		glUseProgram( programId );

		// uniform variables
		uTranslationXid = glGetUniformLocation( programId, "uTranslationX" );
		//glProgramUniform1f( programId, uTranslationXid, 0.4 );
		glDisable( GL_DEPTH_TEST );

		std::cout << "Done!" << std::endl;
		return true;
	}

	void LabWork2::animate( const float p_deltaTime ) {
		_time += p_deltaTime;
		double val = glm::sin( _time );
		glProgramUniform1f( programId, uTranslationXid, val );
	}

	void LabWork2::render()
	{
		glClear( GL_COLOR_BUFFER_BIT );
		glBindVertexArray( VAOid );
		glDrawElements( GL_TRIANGLES, vindex.size(), GL_UNSIGNED_INT, 0 );
		//glDrawArrays( GL_TRIANGLES, 0, 6 );
		glBindVertexArray( 0 );
	}

	void LabWork2::handleEvents( const SDL_Event & p_event ) {}

	void LabWork2::displayUI()
	{
		ImGui::Begin( "Settings lab work 2" );
		if (ImGui::SliderFloat("light-intensity", &lightIntensity, 0, 1)) {
			GLuint varInd = glGetUniformLocation( programId, "lightIntensity" );
			glProgramUniform1f( programId, varInd, lightIntensity );
		}
		if(ImGui::ColorEdit3( "bg-color", glm::value_ptr( _bgColor ) ))
			glClearColor( _bgColor.x, _bgColor.y, _bgColor.z, _bgColor.w );
		ImGui::End();
	}

	void LabWork2::generateCircle(const Vec2f& center, const GLuint n, const GLfloat radius) {
		size_t currentIndice = vertices.size();
		float angle = 360.f / n;
		vertices.emplace_back( center );
		for (int i = 0; i < n; i++) {
			vertices.emplace_back( center.x + radius * std::cos( (float)(i)*angle * (float)M_PI / 180.f ),
								   center.y + radius * std::sin( (float)(i)*angle * (float)M_PI / 180.f ) );
		}
		for ( int i = 1; i < n; i++ ) {
			vindex.emplace_back( currentIndice );
			vindex.emplace_back( currentIndice + i );
			vindex.emplace_back( currentIndice + i + 1 );
			colors.emplace_back( getRandomVec3f(), 1.0f );
		}
		vindex.emplace_back( currentIndice );
		vindex.emplace_back( currentIndice + n );
		vindex.emplace_back( currentIndice + 1 );
	}

} // namespace M3D_ISICG
