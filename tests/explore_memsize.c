// How to run this:
// In the project root directory (one level upper of this file), run:
// gcc -O2 tests/explore_memsize.c tg.c -o explore_memsize -lm && ./explore_memsize

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tg.h"

void print_stats(const char *label, struct tg_geom *geom) {
    size_t wkb_size = tg_geom_wkb(geom, NULL, 0);
    size_t mem_size = tg_geom_memsize(geom);
    printf("%s, %zu, %zu\n", label, wkb_size, mem_size);
}

const char* index_name(enum tg_index ix) {
    switch (ix) {
        case TG_NONE: return "NONE";
        case TG_NATURAL: return "NATURAL";
        case TG_YSTRIPES: return "YSTRIPES";
        default: return "DEFAULT";
    }
}

void test_points() {
    struct tg_point p = {10.0, 20.0};
    struct tg_geom *geom = tg_geom_new_point(p);
    print_stats("Point", geom);
    tg_geom_free(geom);
}

void test_linestrings() {
    // Testing sizes and indexes for LineStrings
    // LineStrings support TG_NONE and TG_NATURAL. TG_YSTRIPES is for rings only.
    int sizes[] = {0, 2, 8, 16, 32, 64, 128, 256, 512, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    enum tg_index indexes[] = {TG_NONE, TG_NATURAL};
    int num_indexes = 2;

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        struct tg_point *points = malloc(n * sizeof(struct tg_point));
        for (int j = 0; j < n; j++) {
            points[j] = (struct tg_point){(double)j, (double)j};
        }
        
        for (int k = 0; k < num_indexes; k++) {
            enum tg_index ix = indexes[k];
            struct tg_line *line = tg_line_new_ix(points, n, ix);
            struct tg_geom *geom = tg_geom_new_linestring(line);
            
            char label[64];
            snprintf(label, sizeof(label), "LineString(%d, %s)", n, index_name(ix));
            print_stats(label, geom);

            tg_geom_free(geom);
            tg_line_free(line);
        }
        free(points);
    }
}

void test_polygons() {
    // Simple polygon (triangle)
    {
        struct tg_point points[] = {{0,0}, {10,0}, {0,10}, {0,0}};
        enum tg_index indexes[] = {TG_NONE, TG_NATURAL, TG_YSTRIPES};
        int num_indexes = 3;

        for (int k = 0; k < num_indexes; k++) {
            enum tg_index ix = indexes[k];
            struct tg_ring *ring = tg_ring_new_ix(points, 4, ix);
            struct tg_poly *poly = tg_poly_new(ring, NULL, 0);
            struct tg_geom *geom = tg_geom_new_polygon(poly);
            
            char label[64];
            snprintf(label, sizeof(label), "Polygon(Triangle, %s)", index_name(ix));
            print_stats(label, geom);

            tg_geom_free(geom);
            tg_poly_free(poly);
            tg_ring_free(ring);
        }
    }

    // Square
    {
        struct tg_point points[] = {{0,0}, {10,0}, {10,10}, {0,10}, {0,0}};
        enum tg_index indexes[] = {TG_NONE, TG_NATURAL, TG_YSTRIPES};
        int num_indexes = 3;

        for (int k = 0; k < num_indexes; k++) {
            enum tg_index ix = indexes[k];
            struct tg_ring *ring = tg_ring_new_ix(points, 5, ix);
            struct tg_poly *poly = tg_poly_new(ring, NULL, 0);
            struct tg_geom *geom = tg_geom_new_polygon(poly);
            
            char label[64];
            snprintf(label, sizeof(label), "Polygon(Square, %s)", index_name(ix));
            print_stats(label, geom);

            tg_geom_free(geom);
            tg_poly_free(poly);
            tg_ring_free(ring);
        }
    }

    // Testing sizes and indexes for Polygons
    // Polygons (Rings) support TG_NONE, TG_NATURAL, and TG_YSTRIPES.
    int sizes[] = {16, 32, 64, 128, 256, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    enum tg_index indexes[] = {TG_NONE, TG_NATURAL, TG_YSTRIPES};
    int num_indexes = 3;

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        struct tg_point *points = malloc(n * sizeof(struct tg_point));
        for (int j = 0; j < n; j++) {
            points[j] = (struct tg_point){(double)j, (double)j};
        }
        points[n-1] = points[0]; // Close the ring

        for (int k = 0; k < num_indexes; k++) {
            enum tg_index ix = indexes[k];
            struct tg_ring *ring = tg_ring_new_ix(points, n, ix);
            struct tg_poly *poly = tg_poly_new(ring, NULL, 0);
            struct tg_geom *geom = tg_geom_new_polygon(poly);
            
            char label[64];
            snprintf(label, sizeof(label), "Polygon(%d, %s)", n, index_name(ix));
            print_stats(label, geom);

            tg_geom_free(geom);
            tg_poly_free(poly);
            tg_ring_free(ring);
        }
        free(points);
    }

    // Polygon with holes
    {
        struct tg_point ext_pts[] = {{0,0}, {100,0}, {100,100}, {0,100}, {0,0}};
        struct tg_ring *ext = tg_ring_new(ext_pts, 5);

        struct tg_point hole1_pts[] = {{10,10}, {20,10}, {20,20}, {10,20}, {10,10}};
        struct tg_ring *hole1 = tg_ring_new(hole1_pts, 5);

        struct tg_point hole2_pts[] = {{30,30}, {40,30}, {40,40}, {30,40}, {30,30}};
        struct tg_ring *hole2 = tg_ring_new(hole2_pts, 5);

        const struct tg_ring *holes[] = {hole1, hole2};
        struct tg_poly *poly = tg_poly_new(ext, holes, 2);
        struct tg_geom *geom = tg_geom_new_polygon(poly);

        print_stats("Polygon(2 Holes)", geom);

        tg_geom_free(geom);
        tg_poly_free(poly);
        tg_ring_free(ext);
        tg_ring_free(hole1);
        tg_ring_free(hole2);
    }
}

void test_multigeoms() {
    // MultiPoint
    int sizes[] = {10, 50, 64, 100, 200};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        struct tg_point *points = malloc(n * sizeof(struct tg_point));
        for (int j = 0; j < n; j++) {
            points[j] = (struct tg_point){(double)j, (double)j};
        }
        struct tg_geom *geom = tg_geom_new_multipoint(points, n);
        
        char label[64];
        snprintf(label, sizeof(label), "MultiPoint(%d)", n);
        print_stats(label, geom);

        tg_geom_free(geom);
        free(points);
    }

    // MultiLineString
    {
        int nlines_arr[] = {10, 50, 64, 100};
        int num_sizes = sizeof(nlines_arr) / sizeof(nlines_arr[0]);
        int npoints = 20;
        struct tg_point *points = malloc(npoints * sizeof(struct tg_point));
        for(int k=0; k<npoints; k++) points[k] = (struct tg_point){0,0};

        for (int i = 0; i < num_sizes; i++) {
            int nlines = nlines_arr[i];
            struct tg_line **lines = malloc(nlines * sizeof(struct tg_line*));
            for (int j = 0; j < nlines; j++) {
                lines[j] = tg_line_new(points, npoints);
            }

            struct tg_geom *geom = tg_geom_new_multilinestring((const struct tg_line *const *)lines, nlines);
            
            char label[64];
            snprintf(label, sizeof(label), "MultiLineString(%dx%d)", nlines, npoints);
            print_stats(label, geom);

            tg_geom_free(geom);
            for (int j = 0; j < nlines; j++) tg_line_free(lines[j]);
            free(lines);
        }
        free(points);
    }
}

void test_collections() {
    int sizes[] = {10, 50, 64, 100};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        struct tg_geom **geoms = malloc(n * sizeof(struct tg_geom*));
        for (int j = 0; j < n; j++) {
            struct tg_point p = {(double)j, (double)j};
            geoms[j] = tg_geom_new_point(p);
        }

        struct tg_geom *gc = tg_geom_new_geometrycollection((const struct tg_geom *const *)geoms, n);
        
        char label[64];
        snprintf(label, sizeof(label), "GeometryCollection(%d Points)", n);
        print_stats(label, gc);

        tg_geom_free(gc);
        for (int j = 0; j < n; j++) tg_geom_free(geoms[j]);
        free(geoms);
    }
}

int main() {
    printf("Type, WKBSize, MemSize\n");
    test_points();
    test_linestrings();
    test_polygons();
    test_multigeoms();
    test_collections();
    return 0;
}
