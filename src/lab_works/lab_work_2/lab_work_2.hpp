#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		void generateCircle( const Vec2f & center, const GLuint n, const GLfloat radius );

	  private:
		// ================ Scene data.
		std::vector<Vec2f> vertices;
		std::vector<GLuint> vindex;
		std::vector<Vec4f>	colors;

		GLdouble _time = 0;
		// ================

		// ================ GL data.
		GLuint programId = -1;
		GLuint VBOid	 = -1;
		GLuint VAOid	 = -1;
		GLuint EBOid	 = -1;
		GLuint VBOcolid	 = -1;

		GLuint uTranslationXid = -1;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		GLfloat lightIntensity = 1;
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_2_HPP__
