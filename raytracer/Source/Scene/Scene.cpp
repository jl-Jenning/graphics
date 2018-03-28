#include "Scene.h"
#include "TestModelH.h"
#include "Post.h"

using namespace glm;
using glm::vec3;

static vec3 pixels[SCREEN_WIDTH][SCREEN_HEIGHT];

void drawPixels(screen * screen);

Scene::Scene()
{
    vec4 camPos(0,0,-3,1);
    this->cam = Camera(CAM_FOCAL_LENGTH, camPos);
    this-> light = Light(
            vec4(0, -0.5, -0.7, 1),
            14.f * vec3(1,1,1));
    LoadTestModel(this->objects);
    this->bvh = BVH(objects);
}

void Scene::Update(KeyStroke_t * keystate)
{
    if ( keystate[SDL_SCANCODE_UP] )
	{
        this->cam.forward();
	}
	if ( keystate[SDL_SCANCODE_DOWN] )
	{
		this->cam.backward();
	}
	if ( keystate[SDL_SCANCODE_LEFT] )
	{
	    this->cam.left();
	}
	if ( keystate[SDL_SCANCODE_RIGHT] )
	{
	    this->cam.right();
	}
}

void Scene::Draw(screen* screen)
{
    vec3 color;
    vec4 rayFromOrigin, rayFromCam;
    vec3 d = vec3();
    Ray r;
 
    for(int y = 0; y < SCREEN_HEIGHT; y++){
        for(int x = 0; x < SCREEN_WIDTH; x++){
            rayFromOrigin.x = x - SCREEN_WIDTH/2;
            rayFromOrigin.y = y - SCREEN_HEIGHT/2;
            rayFromOrigin.z = cam.focalLength;
            rayFromOrigin[3] = 1;

            color = vec3(0);

            rayFromCam = cam.R * rayFromOrigin;

            d = glm::normalize(vec3(rayFromCam[0],
                        rayFromCam[1],
                        rayFromCam[2]));

            r.initial = cam.position;
            r.direction = d;

            shootRay(r, color, bvh, light);
            pixels[x][y] = color;
        }
    }
    post_processing(pixels);
    drawPixels(screen);
}

void drawPixels(screen * screen)
{
    for (int i = 0; i < SCREEN_WIDTH; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT; j ++)
        {
            PutPixelSDL(screen, i, j, pixels[i][j]);
        }
    }

}