#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t data_offset;
} BMPHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t important_colors;
} BMPInfoHeader;
#pragma pack(pop)

void applyGaussianBlur(uint8_t* image_data, int width, int height, double sigma) {
    int radius = (int)ceil(3 * sigma);
    int size = 2 * radius + 1;
    double* kernel = (double*)malloc(size * sizeof(double));
    double sum = 0.0;

    // Generate Gaussian kernel
    for (int i = -radius; i <= radius; i++) {
        kernel[i + radius] = exp(-(i * i) / (2 * sigma * sigma));
        sum += kernel[i + radius];
    }

    // Normalize the kernel
    for (int i = 0; i < size; i++) {
        kernel[i] /= sum;
    }

    // Apply the Gaussian blur
    #pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double r = 0.0, g = 0.0, b = 0.0;
            double total_weight = 0.0;

            // SIMD Vectorization
            // Iterate through kernel
            #pragma omp simd reduction(+:r,g,b,total_weight)
            for (int i = -radius; i <= radius; i++) {
                for (int j = -radius; j <= radius; j++) {
                    int xx = x + j;
                    int yy = y + i;

                    // Bounds checking
                    if (xx >= 0 && xx < width && yy >= 0 && yy < height) {
                        int index = (yy * width + xx) * 3;
                        double weight = kernel[j + radius] * kernel[i + radius];
                        total_weight += weight;

                        r += weight * image_data[index];
                        g += weight * image_data[index + 1];
                        b += weight * image_data[index + 2];
                    }
                }
            }

            // Calculate index on final image
            int index = (y * width + x) * 3;
            image_data[index] = (uint8_t)(r / total_weight);
            image_data[index + 1] = (uint8_t)(g / total_weight);
            image_data[index + 2] = (uint8_t)(b / total_weight);
        }
    }

    free(kernel);
}

void printUsage(){
    printf("Usage:\ngauss.exe target_image.bmp sigma\n\
        target_image - Image to be blurred. Must be in BMP file format\n\
        sigma - The rate of blur. Must be positive\n");
}

int main(int argc, char** argv) {
    if(argc < 3){
        printf("Error! Too few arguments!\n");
        printUsage();
        return 1;
    }else if(argc > 3){
        printf("Error! Too many arguments!\n");
        printUsage();
        return 1;
    }else if(atoi(argv[2]) <= 0){
        printf("Error! Sigma value must be positive!\n");
        printUsage();
        return 1;
    }
    
    char* targetImage = argv[1];
    FILE* file = fopen(targetImage, "rb");
    if (!file) {
        printf("Error opening %s file.\n", targetImage);
        printUsage();
        return 1;
    }

    // Read headers
    BMPHeader header;
    fread(&header, sizeof(BMPHeader), 1, file);
    BMPInfoHeader info_header;
    fread(&info_header, sizeof(BMPInfoHeader), 1, file);

    // Check if the image is 24-bit BMP
    if (info_header.bits_per_pixel != 24) {
        printf("Only 24-bit BMP images are supported.\n");
        fclose(file);
        return 1;
    }

    uint8_t* image_data = (uint8_t*)malloc(info_header.image_size);
    if (!image_data) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    // Read the image data
    fread(image_data, 1, info_header.image_size, file);
    fclose(file);

    // Apply Gaussian blur to target image
    double sigma = atoi(argv[2]);
    applyGaussianBlur(image_data, info_header.width, info_header.height, sigma);

    // Create a new file for the modified image with name targatImageBlurred.bmp
    char* outputImage = strcat(strtok(targetImage, "."), "Blurred.bmp");
    FILE* modified_file = fopen(outputImage, "wb");
    if (!modified_file) {
        printf("Error creating %s file.\n", outputImage);
        free(image_data);
        return 1;
    }

    // Write the BMP header and info header to the new file
    fwrite(&header, sizeof(BMPHeader), 1, modified_file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, modified_file);

    // Write the modified image data to the new file
    fwrite(image_data, 1, info_header.image_size, modified_file);
    fclose(modified_file);

    free(image_data);

    printf("Image processing completed successfully. Saved a %s!\n", outputImage);

    return 0;
}