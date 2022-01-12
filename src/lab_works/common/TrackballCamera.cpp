#include "TrackballCamera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace M3D_ISICG
{
	void TrackballCamera::setScreenSize( const int p_width, const int p_height )
	{
		_screenWidth  = p_width;
		_screenHeight = p_height;
		_aspectRatio  = float( _screenWidth ) / _screenHeight;
		_updateVectors();
		_computeViewMatrix();
		_computeProjectionMatrix();
	}

	void TrackballCamera::updatePosition() {
		_updateVectors();
		_position = _subjectPosition
					+ Vec3f( _invDirection.x * _subjectDistance,
							 _invDirection.y * _subjectDistance,
							 _invDirection.z * _subjectDistance );
		_computeViewMatrix();
	}

	void TrackballCamera::moveFront( const float p_delta )
	{
		_subjectPosition -= _invDirection * p_delta;
		updatePosition();
	}

	void TrackballCamera::moveRight( const float p_delta )
	{
		_subjectPosition += _right * p_delta;
		updatePosition();
	}

	void TrackballCamera::moveUp( const float p_delta )
	{
		_subjectPosition += _up * p_delta;
		updatePosition();
	}

	void TrackballCamera::rotate( const float p_yaw, const float p_pitch )
	{
		_yaw   = glm::mod( _yaw + p_yaw, 360.f );
		_pitch = glm::clamp( _pitch + p_pitch, -89.f, 89.f );
		updatePosition();
	}

	void TrackballCamera::print() const
	{
		std::cout << "======== Camera ========" << std::endl;
		std::cout << "Position: " << glm::to_string( _position ) << std::endl;
		std::cout << "View direction: " << glm::to_string( -_invDirection ) << std::endl;
		std::cout << "Right: " << glm::to_string( _right ) << std::endl;
		std::cout << "Up: " << glm::to_string( _up ) << std::endl;
		std::cout << "Yaw: " << _yaw << std::endl;
		std::cout << "Pitch: " << _pitch << std::endl;
		std::cout << "========================" << std::endl;
	}

	Vec3f TrackballCamera::getPosition() { return _position; }

	void TrackballCamera::setPosition( const Vec3f & p_position )
	{
		_subjectPosition = p_position;
		updatePosition();
	}

	void TrackballCamera::setLookAt( const Vec3f & p_lookAt )
	{
		_invDirection = p_lookAt + _position;
		_computeViewMatrix();
	}

	void TrackballCamera::setFovy( const float p_fovy )
	{
		_fovy = p_fovy;
		_computeProjectionMatrix();
	}

	void TrackballCamera::_computeViewMatrix()
	{
		_viewMatrix = glm::lookAt( _position, _position - _invDirection, _up );
	}

	void TrackballCamera::_computeProjectionMatrix()
	{
		_projectionMatrix = glm::perspective( glm::radians( _fovy ), _aspectRatio, _zNear, _zFar );
	}

	void TrackballCamera::_computeUMVPMatrix( const Mat4f & modelMatrix )
	{
		_uMVPMatrix = _projectionMatrix * _viewMatrix * modelMatrix;
	}

	void TrackballCamera::_computeUMVMatrix( const Mat4f & modelMatrix ) { _uMVMatrix = _viewMatrix * modelMatrix; }

	void TrackballCamera::update()
	{
		updatePosition();
		_computeProjectionMatrix();
		_computeViewMatrix();
	}

	void TrackballCamera::_updateVectors()
	{
		const float yaw	  = glm::radians( _yaw );
		const float pitch = glm::radians( _pitch );
		_invDirection	  = glm::normalize(
			Vec3f( glm::cos( yaw ) * glm::cos( pitch ), glm::sin( pitch ), glm::sin( yaw ) * glm::cos( pitch ) ) );
		_right = glm::normalize( glm::cross( Vec3f( 0.f, 1.f, 0.f ), _invDirection ) ); // We suppose 'y' as up.
		_up	   = glm::normalize( glm::cross( _invDirection, _right ) );
	}

} // namespace M3D_ISICG
