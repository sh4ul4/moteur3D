#ifndef __TRACKBALL_CAMERA_HPP__
#define __TRACKBALL_CAMERA_HPP__

#include "GL/gl3w.h"
#include "common/base_camera.hpp"
#include "define.hpp"

namespace M3D_ISICG
{
	class TrackballCamera : public BaseCamera
	{
	  public:
		TrackballCamera() {}

		inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
		inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }
		inline const Mat4f & getUMVPMatrix() const { return _uMVPMatrix; }
		inline const Mat4f & getUMVMatrix() const { return _uMVMatrix; }
		Vec3f				 getPosition();

		void updatePosition();

		void setPosition( const Vec3f & p_position );
		void setLookAt( const Vec3f & p_lookAt );
		void setFovy( const float p_fovy );

		void setScreenSize( const int p_width, const int p_height );

		void moveFront( const float p_delta );
		void moveRight( const float p_delta );
		void moveUp( const float p_delta );
		void rotate( const float p_yaw, const float p_pitch );

		void print() const;

		void _computeUMVPMatrix( const Mat4f & modelMatrix );
		void _computeUMVMatrix( const Mat4f & modelMatrix );

	  private:
		void _computeViewMatrix();
		void _computeProjectionMatrix();
		void update();
		void _updateVectors();

	  public:
		Vec3f _subjectPosition = Vec3f(0.f);
		float _subjectDistance = 10;
	};
} // namespace M3D_ISICG

#endif // __TRACKBALL_CAMERA_HPP__
