#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COLOR 255

int main(int argc, char **argv) {
    // read a file from argv[1]
    int width, height, bpp;
    unsigned char *img = stbi_load(argv[1], &width, &height, &bpp, 3);
    if (img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }

    unsigned char nord_frost[4][3] = {
        {143, 188, 187},
        {136, 192, 208},
        {129, 161, 193},
        {94, 129, 172}
    };

    unsigned char nord_polar_night[4][3] = {
        {46, 52, 64},
        {59, 66, 82},
        {67, 76, 94},
        {76, 86, 106}
    };

    unsigned char nord_snow_storm[3][3] = {
        {216, 222, 233},
        {229, 233, 240},
        {236, 239, 244},
    };

    unsigned char nord_aurora[5][3] = {
        {191, 97, 106},
        {208, 135, 112},
        {235, 203, 139},
        {163, 190, 140},
        {180, 142, 173}
    };

    char *theme = argv[2];

    printf("theme: %s\n", theme);

    unsigned char nord[16][3] = {0};

    int num_colors = 0;

    // check theme and build up color pallete (nord) using strstr
    if (strstr(theme, "frost") != NULL) {
        memcpy(nord + num_colors, nord_frost, sizeof(nord_frost));
        num_colors += 4;
    }
    
    if (strstr(theme, "polar") != NULL) {
        memcpy(nord + num_colors, nord_polar_night, sizeof(nord_polar_night));
        num_colors += 4;
    }

    if (strstr(theme, "snow") != NULL) {
        memcpy(nord + num_colors, nord_snow_storm, sizeof(nord_snow_storm));
        num_colors += 3;
    }
    
    if (strstr(theme, "aurora") != NULL) {
        memcpy(nord + num_colors, nord_aurora, sizeof(nord_aurora));
        num_colors += 5;
    }

    printf("num_colors: %d\n", num_colors);

    // convert to nord color palette

    for (size_t i = 0; i < width * height * 3; i += 3) {
        unsigned char r = img[i];
        unsigned char g = img[i + 1];
        unsigned char b = img[i + 2];

        unsigned char min = 255;
        int min_index = 0;
        for (size_t j = 0; j < num_colors; j++) {
            unsigned char r_diff = abs(r - nord[j][0]);
            unsigned char g_diff = abs(g - nord[j][1]);
            unsigned char b_diff = abs(b - nord[j][2]);
            unsigned short diff = r_diff + g_diff + b_diff;
            if (diff < min) {
                min = diff;
                min_index = j;
            }
        }

        img[i] = nord[min_index][0];
        img[i + 1] = nord[min_index][1];
        img[i + 2] = nord[min_index][2];
    }

    // write to file

    char *filename = "nordified.png";
    int success = stbi_write_png(filename, width, height, 3, img, width * 3);
    if (success == 0) {
        printf("Error in writing the image\n");
        exit(1);
    }

    // free the image
    stbi_image_free(img);

    
}