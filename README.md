# glacier.c

A program to colorize a PNG image using the Nord color scheme.

|original|glacier.c|
|--------|---------|
|![pic.png](pic.png)|![out.png](out.png)|

## How it works

The program iterates over the pixels in the image and uses the Manhattan distance (L1 norm RGB distance) to pick the closest Nord color that makes sense for the given pixel.

## Licensing

Licensed under the [MIT License](https://opensource.org/licenses/MIT). For details, see [LICENSE](https://github.com/xqb64/glacier.c/blob/master/LICENSE).