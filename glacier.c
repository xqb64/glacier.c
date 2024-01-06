#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static uint8_t nord_frost[4][3] = {
    {143, 188, 187},
    {136, 192, 208},
    {129, 161, 193},
    {94, 129, 172}
};

static uint8_t nord_polar_night[4][3] = {
    {46, 52, 64},
    {59, 66, 82},
    {67, 76, 94},
    {76, 86, 106}
};

static uint8_t nord_snow_storm[3][3] = {
    {216, 222, 233},
    {229, 233, 240},
    {236, 239, 244},
};

static uint8_t nord_aurora[5][3] = {
    {191, 97, 106},
    {208, 135, 112},
    {235, 203, 139},
    {163, 190, 140},
    {180, 142, 173}
};

int main(int argc, char **argv) {
    int width, height, bpp;
    
    uint8_t *img = stbi_load(argv[1], &width, &height, &bpp, 3);
    if (!img) {
        fprintf(stderr, "glacier: couldn't load the image\n");
        return 1;
    }

    char *schemes = argv[2];

    uint8_t palette[16][3] = {0};

    unsigned colors_used = 0;

    if (strstr(schemes, "frost")) {
        memcpy(palette + colors_used, nord_frost, sizeof(nord_frost));
        colors_used += 4;
    }
    
    if (strstr(schemes, "polar_night")) {
        memcpy(palette + colors_used, nord_polar_night, sizeof(nord_polar_night));
        colors_used += 4;
    }

    if (strstr(schemes, "snow_storm")) {
        memcpy(palette + colors_used, nord_snow_storm, sizeof(nord_snow_storm));
        colors_used += 3;
    }
    
    if (strstr(schemes, "aurora")) {
        memcpy(palette + colors_used, nord_aurora, sizeof(nord_aurora));
        colors_used += 5;
    }

    // For each of the (r,g,b) values in the image
    for (size_t i = 0; i < width * height * 3; i += 3) {
        uint8_t min = 255;
        int min_index = 0;

        for (size_t j = 0; j < colors_used; j++) {
            // Compute the Manhattan distance between the color of the pixel
            // and the Nord color
            uint8_t r_diff = abs(img[i] - palette[j][0]);
            uint8_t g_diff = abs(img[i + 1] - palette[j][1]);
            uint8_t b_diff = abs(img[i + 2] - palette[j][2]);
            uint16_t diff = r_diff + g_diff + b_diff;
            
            // If the distance is smaller than the current minimum, set it as minimum
            if (diff < min) {
                min = diff;
                min_index = j;
            }
        }

        // Recolor the pixel
        img[i] = palette[min_index][0];
        img[i + 1] = palette[min_index][1];
        img[i + 2] = palette[min_index][2];
    }

    // Write the image to file
    const char *filename = "out.png";
    int success = stbi_write_png(filename, width, height, 3, img, width * 3);
    if (!success) {
        fprintf(stderr, "glacier: error while writing the image\n");
        return 1;
    }

    // Free the image
    stbi_image_free(img);

    return 0;
}