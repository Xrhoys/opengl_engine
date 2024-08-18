/* date = August 18th 2024 8:43 pm */

#ifndef ANDROID_MAIN_H
#define ANDROID_MAIN_H

struct platform_user_data
{
	android_app* app;
	
    b32 active;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
	
	platform_engine* engine;
    platform_render* render;
};

#endif //ANDROID_MAIN_H
