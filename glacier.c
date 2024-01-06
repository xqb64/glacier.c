#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>

#define TABLE_MAX 4

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

struct Scheme {
    uint8_t (*palette)[3]; 
    int num_colors;
};

struct Bucket {
    char *key;
    struct Scheme value;
    struct Bucket *next;
};

struct Table {
    struct Bucket *data[4];
};

uint32_t hash(const char *key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

struct Scheme *list_find(struct Bucket *bucket, char *key) {
    struct Bucket *current = bucket;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0)
            return &current->value;
        current = current->next;
    }
    return NULL;
}

void list_insert(struct Bucket **bucket, char *key, struct Scheme value) {
    struct Bucket *new_node = malloc(sizeof(struct Bucket));
    new_node->key = key;
    new_node->value = value; 
    new_node->next = NULL;

    if (!*bucket)
        *bucket = new_node;
    else {
        struct Bucket *current = *bucket;
        while (current->next != NULL) {
            current = current->next;
        }

        current->next = new_node;
    }
}

void table_insert(struct Table *table, char *key, struct Scheme value) {
    uint32_t index = hash(key, strlen(key)) % TABLE_MAX;
    list_insert(&table->data[index], key, value);
}

struct Scheme *table_get(struct Table *table, char *key) {
    uint32_t index = hash(key, strlen(key)) % TABLE_MAX;
    return list_find(table->data[index], key);
}

void table_free(struct Table *table) {
    for (size_t i = 0; i < TABLE_MAX; i++) {
        free(table->data[i]);
    }
}

int main(int argc, char **argv) {
    struct Table schemes = {0};
    table_insert(&schemes, "frost", (struct Scheme){.palette = nord_frost, .num_colors = 4});
    table_insert(&schemes, "polar_night", (struct Scheme){.palette = nord_polar_night, .num_colors = 4});
    table_insert(&schemes, "snow_storm", (struct Scheme){.palette = nord_snow_storm, .num_colors = 3});
    table_insert(&schemes, "aurora", (struct Scheme){.palette = nord_aurora, .num_colors = 5});

    char *in_file = argv[1];

    int opt, colors_used = 0;
    char *out_file = NULL;
    uint8_t palette[16][3] = {0};
    while ((opt = getopt(argc, argv, "s:o:")) != -1) {
        switch (opt) {
        case 's': {
            struct Scheme *scheme = table_get(&schemes, optarg);
            if (!scheme) {
                fprintf(stderr, "no scheme %s\n", optarg);
                return 1;
            }
            memcpy(palette + colors_used, scheme->palette, scheme->num_colors * sizeof(*scheme->palette));
            colors_used += scheme->num_colors;
            break;
        }
        case 'o': {
            out_file = optarg;
            break;
        }
        default:
            fprintf(stderr, "no option %s\n", optarg);
            return 1;
        }
    }

    int width, height, bpp;
    uint8_t *img = stbi_load(in_file, &width, &height, &bpp, 3);
    if (!img) {
        fprintf(stderr, "glacier: couldn't load the image\n");
        return 1;
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
    int success = stbi_write_png(out_file, width, height, 3, img, width * 3);
    if (!success) {
        fprintf(stderr, "glacier: error while writing the image\n");
        return 1;
    }

    // Free the image
    stbi_image_free(img);

    // Free the table
    table_free(&schemes);

    return 0;
}