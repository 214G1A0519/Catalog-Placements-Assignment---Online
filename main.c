#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cJSON.h" 

// Function to read JSON file content
char *readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(length + 1);
    if (!content) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    fread(content, 1, length, file);
    fclose(file);
    content[length] = '\0';
    return content;
}

// Function to decode a number from a given base to decimal
long long decodeBaseValue(int base, const char *value) {
    return strtoll(value, NULL, base); // Use strtoll for long long
}

// Function for Lagrange interpolation to find the constant term
double lagrangeInterpolation(int *x, long long *y, int k) {
    double constantTerm = 0.0;
    int i, j;
    for (i = 0; i < k; i++) {
        double term = (double)y[i];
        for (j = 0; j < k; j++) {
            if (i != j) {
                term *= (0.0 - x[j]) / (x[i] - x[j]); // Substitute x = 0 for constant term
            }
        }
        constantTerm += term;
    }
    return constantTerm;
}

int main() {
    // Read JSON input from file
    char *jsonContent = readFile("testcase.json");
    cJSON *json = cJSON_Parse(jsonContent);
    if (!json) {
        fprintf(stderr, "Error parsing JSON data\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Parsed JSON successfully.\n");
    }

    // Extract the number of points (n) and the minimum number required (k)
    cJSON *keys = cJSON_GetObjectItem(json, "keys");
    int n = cJSON_GetObjectItem(keys, "n")->valueint;
    int k = cJSON_GetObjectItem(keys, "k")->valueint;
    printf("Number of roots (n): %d, Minimum required (k): %d\n", n, k);

    int x[k];
    long long y[k]; // Change y to long long
    int count = 0;

    // Parse each point from the JSON
    cJSON *item;
    cJSON_ArrayForEach(item, json) {
        if (cJSON_IsNumber(item)) continue; // Skip "keys" object

        int xi = atoi(item->string);
        cJSON *baseObj = cJSON_GetObjectItem(item, "base");
        cJSON *valueObj = cJSON_GetObjectItem(item, "value");

        if (baseObj == NULL || valueObj == NULL) {
            printf("Missing base or value for point %d\n", count);
            continue; // Skip if base or value is missing
        }

        int base = atoi(baseObj->valuestring);
        long long yi = decodeBaseValue(base, valueObj->valuestring);

        // Store the values
        x[count] = xi;
        y[count] = yi; // Store as long long
        printf("Point %d: x = %d, y = %lld\n", count, xi, yi); // Print points
        count++;
        if (count == k) break; // Only take the first k points
    }

    // Perform Lagrange interpolation to find the constant term
    double constantTerm = lagrangeInterpolation(x, y, count); // Use count instead of k
    printf("Constant term (c): %.0f\n", constantTerm);

    // Clean up and free memory
    cJSON_Delete(json);
    free(jsonContent);
    return 0;
}

