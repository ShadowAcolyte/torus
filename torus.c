#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159f

// dimensions in characters (not pixels)
int SCREEN_WIDTH;
int SCREEN_HEIGHT;

char BRIGHTNESS_CHAR_MAP[] = {
    '.', ',', '-', '~', ':', ';', '=', '!', '*', '#', '$', '@'
};

void draw_buffer(char* buff, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
            printf("%c", buff[y * width + x]);
        printf("\n");
    }
}

int main()
{
    // init
    SCREEN_WIDTH = 96;
    SCREEN_HEIGHT = 48;

    char* display_buffer;
    display_buffer = (char*) malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(char));

    float* zbuffer = (float*) malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(float));
    memset(zbuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(float));

    // camera position in units along negative z axis
    float cam_pos = 40;

    // torus data
    float r1 = 8;
    float r2 = 16;
    float rot_x = 0;
    float rot_z = 0;
    // torus center position in units along positive z axis
    float torus_pos = 20;

    while (1)
    {
        rot_x += 0.001f;
        rot_z += 0.001f;

        printf("\x1b[H");
        memset(display_buffer, ' ', SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(char));
        memset(zbuffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(float));

        float sin_rot_x = sinf(rot_x);
        float cos_rot_x = cosf(rot_x);
        float sin_rot_z = sinf(rot_z);
        float cos_rot_z = cosf(rot_z);

        for (float theta = 0; theta < 2 * PI; theta += 2 * PI / 100)
        {
            float sin_theta = sinf(theta);
            float cos_theta = cosf(theta);
            for (float phi = 0; phi < 2 * PI; phi += 2 * PI / 100)
            {
                float sin_phi = sinf(phi);
                float cos_phi = cosf(phi);

                float cx = r2 + r1 * cos_theta;
                float cy = r1 * sin_theta;

                float x = cx*(cos_rot_z * cos_phi + sin_rot_x * sin_rot_z * sin_phi) - cy * cos_rot_x * sin_rot_z; 
                float y = cx*(sin_rot_z * cos_phi - sin_rot_x * cos_rot_z * sin_phi) + cy * cos_rot_x * cos_rot_z;
                float z = (cam_pos + torus_pos) + cos_rot_x * cx * sin_phi + cy * sin_rot_x;
                float z_inv = 1/z;

                int xp = (int) (SCREEN_WIDTH / 2 + cam_pos * z_inv * x);
                int yp = (int) (SCREEN_HEIGHT / 2 - cam_pos * z_inv * y);

                float normal = cos_phi * cos_theta * sin_rot_z - cos_rot_x * cos_theta * sin_phi - sin_rot_x
                            * sin_theta + cos_rot_z * (cos_rot_x * sin_theta - cos_theta * sin_rot_x * sin_phi);
                if (normal > 0)
                {
                    if (z_inv > zbuffer[yp * SCREEN_WIDTH + xp])
                    {
                        zbuffer[yp * SCREEN_WIDTH + xp] = z_inv;
                        display_buffer[yp * SCREEN_WIDTH + xp] = BRIGHTNESS_CHAR_MAP[(int) (normal * 8)];
                    }
                }
            }
        }
        draw_buffer(display_buffer, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    return 0;
}
