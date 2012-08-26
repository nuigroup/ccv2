#include <CLEyeMulticam.h>

#include <opencv\cv.h>
#include <opencv\highgui.h>

class CLEyeCamera
{
public:
	CLEyeCamera(GUID guid, CLEyeCameraColorMode colorMode, CLEyeCameraResolution resolution, int fps)
	{
		this->_guid = guid;
		this->_colorMode = colorMode;
		this->_resolution = resolution;
		this->_fps = fps;

		this->_camera = CLEyeCreateCamera(_guid, _colorMode, _resolution, _fps);

		CLEyeCameraGetFrameDimensions(_camera, _width, _height);

		CLEyeCameraStart(_camera);
	};

	~CLEyeCamera()
	{
		CLEyeCameraStop(_camera);
		CLEyeDestroyCamera( _camera );
	}

	void getFrame(IplImage* pFrame)
	{
		PBYTE pCapBuffer = NULL;
		cvGetImageRawData( pFrame, &pCapBuffer );
		CLEyeCameraGetFrame( _camera, pCapBuffer );
	}

	int width()
	{
		return _width;
	}

	int height()
	{
		return _height;
	}
protected:
private:
	GUID _guid;

	CLEyeCameraColorMode _colorMode;
	CLEyeCameraResolution _resolution;
	int _fps;

	CLEyeCameraInstance _camera;

	int _width;
	int _height;
};
