# TG Geometry Memory Analysis

This document explores the relationship between the WKB (Well-Known Binary) size of geometries and their actual memory footprint in the TG library, as reported by `tg_geom_memsize`.

## Experimental Results

The following table shows the WKB size and Memory Size (in bytes) for various geometry types and sizes.

| Geometry Type | WKB Size (bytes) | Memory Size (bytes) | Ratio (Mem/WKB) |
|---|---|---|---|
| Point | 21 | 24 | 1.14 |
| LineString(0) | 9 | 160 | 17.78 |
| LineString(2) | 41 | 160 | 3.90 |
| LineString(8) | 137 | 224 | 1.64 |
| LineString(16) | 265 | 352 | 1.33 |
| LineString(32) | 521 | 672 | 1.29 |
| LineString(64) | 1033 | 1216 | 1.18 |
| LineString(128) | 2057 | 2304 | 1.12 |
| LineString(256) | 4105 | 4480 | 1.09 |
| LineString(512) | 8201 | 8880 | 1.08 |
| LineString(1024) | 16393 | 17616 | 1.07 |
| Polygon(Triangle) | 77 | 160 | 2.08 |
| Polygon(Square) | 93 | 176 | 1.89 |
| Polygon(1024) | 16397 | 17616 | 1.07 |
| Polygon(2 Holes) | 261 | 720 | 2.76 |
| MultiPoint(100) | 2109 | 3808 | 1.81 |
| MultiLineString(10x20) | 3299 | 4352 | 1.32 |
| GeometryCollection | 71 | 312 | 4.39 |

### Indexing Impact

The following table compares memory usage across different indexing strategies for LineStrings and Polygons.

| Geometry | Index Type | WKB Size | Memory Size | Overhead vs NONE |
|---|---|---|---|---|
| LineString(0) | NONE | 9 | 160 | 0 |
| LineString(0) | NATURAL | 9 | 160 | 0 |
| LineString(2) | NONE | 41 | 160 | 0 |
| LineString(2) | NATURAL | 41 | 160 | 0 |
| LineString(8) | NONE | 137 | 224 | 0 |
| LineString(8) | NATURAL | 137 | 224 | 0 |
| LineString(16) | NONE | 265 | 352 | 0 |
| LineString(16) | NATURAL | 265 | 352 | 0 |
| LineString(32) | NONE | 521 | 608 | 0 |
| LineString(32) | NATURAL | 521 | 672 | +64 |
| LineString(64) | NONE | 1033 | 1120 | 0 |
| LineString(64) | NATURAL | 1033 | 1216 | +96 |
| LineString(128) | NONE | 2057 | 2144 | 0 |
| LineString(128) | NATURAL | 2057 | 2304 | +160 |
| LineString(256) | NONE | 4105 | 4192 | 0 |
| LineString(256) | NATURAL | 4105 | 4480 | +288 |
| LineString(1024) | NONE | 16393 | 16480 | 0 |
| LineString(1024) | NATURAL | 16393 | 17616 | +1136 |
| Polygon(16) | NONE | 269 | 352 | 0 |
| Polygon(16) | NATURAL | 269 | 352 | 0 |
| Polygon(16) | YSTRIPES | 269 | 352 | 0 |
| Polygon(32) | NONE | 525 | 608 | 0 |
| Polygon(32) | NATURAL | 525 | 672 | +64 |
| Polygon(32) | YSTRIPES | 525 | 1572 | +964 |
| Polygon(64) | NONE | 1037 | 1120 | 0 |
| Polygon(64) | NATURAL | 1037 | 1216 | +96 |
| Polygon(64) | YSTRIPES | 1037 | 2244 | +1124 |
| Polygon(128) | NONE | 2061 | 2144 | 0 |
| Polygon(128) | NATURAL | 2061 | 2304 | +160 |
| Polygon(128) | YSTRIPES | 2061 | 3588 | +1444 |
| Polygon(256) | NONE | 4109 | 4192 | 0 |
| Polygon(256) | NATURAL | 4109 | 4480 | +288 |
| Polygon(256) | YSTRIPES | 4109 | 6276 | +2084 |
| Polygon(1024) | NONE | 16397 | 16480 | 0 |
| Polygon(1024) | NATURAL | 16397 | 17616 | +1136 |
| Polygon(1024) | YSTRIPES | 16397 | 22484 | +6004 |
| Polygon(Triangle) | NONE | 77 | 160 | 0 |
| Polygon(Triangle) | NATURAL | 77 | 160 | 0 |
| Polygon(Triangle) | YSTRIPES | 77 | 160 | 0 |
| Polygon(Square) | NONE | 93 | 176 | 0 |
| Polygon(Square) | NATURAL | 93 | 176 | 0 |
| Polygon(Square) | YSTRIPES | 93 | 176 | 0 |

### MultiGeometry & Collection Indexing

MultiGeometries and GeometryCollections are automatically indexed when they contain **64 or more** geometries.

| Geometry | Size (N) | WKB Size | Memory Size | Per Item Cost (bytes) | Indexing |
|---|---|---|---|---|---|
| MultiPoint | 10 | 219 | 432 | 43.2 | No |
| MultiPoint | 50 | 1059 | 1712 | 34.2 | No |
| MultiPoint | 64 | 1353 | 2480 | 38.8 | Yes |
| MultiPoint | 100 | 2109 | 3808 | 38.1 | Yes |
| MultiPoint | 200 | 4209 | 7456 | 37.3 | Yes |
| MultiLineString(20pts) | 10 | 3299 | 4352 | 435.2 | No |
| MultiLineString(20pts) | 50 | 16459 | 21312 | 426.2 | No |
| MultiLineString(20pts) | 64 | 21065 | 27568 | 430.8 | Yes |
| MultiLineString(20pts) | 100 | 32909 | 43008 | 430.1 | Yes |

## Analysis & Formulas

### 1. Point
*   **WKB**: 21 bytes.
*   **Memory**: 24 bytes.
*   **Formula**: Fixed 24 bytes (`sizeof(struct boxed_point)`).

### 2. LineString
*   **WKB**: `9 + 16 * N` bytes.
*   **Memory**:
    *   **Header**: 80 bytes (`struct tg_ring` header).
    *   **Points**: `aligned(max(N, 4) * 16)` bytes. (Allocates space for at least 5 points including the extra closing point).
    *   **Index**:
        *   **NONE**: 0 bytes.
        *   **NATURAL**: For `N >= 32`, adds overhead. Starts at ~64 bytes and grows with `N`.
            *   Approx overhead: `~1.1 * N` bytes for large N.
    *   **Formula (NONE)**: `80 + 16 * (N + 1)`.
    *   **Formula (NATURAL)**: `80 + 16 * (N + 1) + IndexSize`.

### 3. Polygon
*   **No Holes**: Same memory layout as a LineString (the `tg_poly` struct is optimized away).
    *   **Memory**: Same as LineString of the exterior ring.
*   **Indexing**:
    *   **NONE**: Same as LineString NONE.
    *   **NATURAL**: Same as LineString NATURAL.
    *   **YSTRIPES**: Significantly higher memory usage.
        *   Adds `struct ystripes` and stripe data.
        *   Overhead is roughly `~6 * N` bytes for large N.
*   **With Holes**:
    *   **Structure**: `sizeof(struct tg_poly)` (32 bytes).
    *   **Exterior**: Full size of exterior ring.
    *   **Holes**: Full size of each hole ring.
    *   **Holes Array**: `nholes * 8` bytes (pointers).
    *   **Note**: `tg_geom_memsize` appears to overestimate the memory usage for holes by adding `nholes * sizeof(struct tg_ring)` (approx 80 bytes) instead of `nholes * sizeof(struct tg_ring*)` (8 bytes). This results in an over-reporting of ~72 bytes per hole.
    *   **Formula**: `32 + Mem(Exterior) + Sum(Mem(Holes)) + nholes * 8 + (nholes * 72 [Overhead])`.

### 4. MultiGeometry & GeometryCollection
*   **Structure**:
    *   `struct tg_geom` wrapper: 48 bytes.
    *   `struct multi` container: 64 bytes.
    *   **Pointers**: `ngeoms * 8` bytes.
*   **Children**: Sum of memory of all child geometries.
*   **Index**:
    *   `ixgeoms` array: `ngeoms * 4` bytes (if indexed).
    *   Index structure: Variable size.
*   **Formula**: `112 + 8 * ngeoms + Sum(Mem(Children)) + (Indexed ? (4 * ngeoms + IndexSize) : 0)`.

## Conclusion

*   **Small Geometries**: Significant overhead. A 2-point LineString takes 160 bytes (vs 41 bytes WKB).
*   **Large Geometries**: Very efficient. A 1024-point LineString takes 17616 bytes (vs 16393 bytes WKB), only ~7% overhead.
*   **Indexing Costs**:
    *   **NONE**: Most memory efficient, linear growth `16 * N`.
    *   **NATURAL**: Adds ~7-10% overhead for large geometries. Efficient spatial indexing.
    *   **YSTRIPES**: Adds significant overhead (~35-40% more than NATURAL for large N, and up to 3x for smaller N). Should be used when point-in-polygon performance is critical.
*   **Points**: Very compact (24 bytes).
*   **Polygons with Holes**: Have a higher reported memory footprint due to separate allocations for each ring and a potential calculation bug in `tg_geom_memsize`.
*   **Collections**: Add about 112 bytes fixed overhead plus 8 bytes per item, plus indexing costs.

For estimation purposes based on WKB:
*   **Points**: `1.2 * WKB`
*   **Large LineStrings/Polygons (NATURAL)**: `1.1 * WKB`
*   **Large LineStrings/Polygons (NONE)**: `1.0 * WKB`
*   **Large Polygons (YSTRIPES)**: `1.4 * WKB`
*   **Small LineStrings/Polygons**: `4.0 * WKB` (or fixed ~160 bytes)
*   **MultiGeometries**: `1.5 * WKB` (varies heavily with child size)