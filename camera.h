#ifndef CAMERA_H
#define CAMERA_H

struct game_camera
{
	v2 position;
	f32 ratio;
	f32 scale; // height = scale, width = ratio * scale
};

inline game_camera
game_InitCamera(v2 position, f32 ratio, f32 scale)
{
	game_camera camera = {};
	camera.position = position;
	camera.ratio = ratio;
	camera.scale = scale;
	
	return camera;
}

inline void
game_UpdateCamera(game_camera* camera, v2 position, f32 ratio, f32 scale)
{
	camera->position = position;
	camera->ratio = ratio;
	camera->scale = scale;
}

inline matrix
game_OrthographicProjection(v2 pos, f32 width, f32 height)
{
	f32 L = pos.x - width / 2.f;
	f32 R = pos.x + width / 2.f;
	f32 T = pos.y + height / 2.f;
	f32 B = pos.y - height / 2.f;
	matrix res = 
	{
		2.0f/(R-L),   0.0f,           0.0f,       0.0f,
		0.0f,         2.0f/(T-B),     0.0f,       0.0f,
		0.0f,         0.0f,           0.5f,       0.0f,
		(R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f,
	};
	
	return res;
}

inline matrix
game_ScreenCoordTransform(game_camera* camera)
{
	f32 zoom = 1.f / camera->scale;
	v2 scaling = V2(zoom, zoom * camera->ratio);
	v2 position = camera->position * -zoom;
	
	matrix res = GenerateTransform3(position, DEFAULT_ROTATION2, scaling);
	
	return res;
}

inline v2
game_ScreenToWorldSpace(game_camera* camera, f32 width, f32 height, v2 pt)
{
	v2 clip = V2(pt.x / ((f32)width / 2.f) - 1.f, 1.f - pt.y / ((f32)height / 2.f));
	
	v2 position = camera->position;
	v2 stride = V2(width / 2.f, height / 2.f) / camera->scale;
	v2 result = position + clip * stride;
	
	return result;
}

inline v2
game_PointToCameraSpace(game_camera* camera, v2 pt, f32 width, f32 height)
{
	pt -= camera->position;
	pt *= camera->scale;
	pt.x += width / 2.f;
	pt.y += height / 2.f;
	
	return pt; 
}

inline void
game_CameraWorldBoundary(game_camera* camera, f32 width, f32 height, v2& p0, v2& p1)
{
	v2 position = camera->position;
	v2 stride = V2(width / 2.f, height / 2.f) / camera->scale;
	p0 = position - stride;
	p1 = position + stride;
}

#endif //CAMERA_H
