# d3d9extern.dll

Allows for drawing text and images onto a D3D9Device.
(Soon also videos I hope!)

Depending on `DirectX SDK June 2010`, `DirectShow` and some other libs from Microsoft
## The Video Player does NOT work yet!

# Use case

This is primarily intended to be used with GML, but I'm sure it can be used for any DX9 game with some tinkering.


# Performance (in GMS 1.4)

- Worse than expected.
- Done in VM mode.
- YYC may speed up invoking DLL calls, but also normal function calls.

## First test
Measuring time with GMSs `get_timer()/1000` after each `draw_sprite()` (GML) and `dx9_draw_image()`
- Both draw the same image to the same position

GML-VM: ~0.01 milliseconds

DX9-DLL: 0.029 milliseconds

## Second test (masstest)

Measuring the same way, but this time with a for loop doing 1 million iterations of drawing the same image
- The DLL already contains a for-loop to eliminate the reoccurance of invoking the DLL function every time

GML-VM: ~1400 milliseconds

DX9-DLL: ~3400 milliseconds

## Results

Result of these tests is: GM's drawing is more optimized than my janky DLL,
even though both use DX9.

I will try doing more advanced stuff with this DLL that native GML2016 drawing doesn't support,
maybe even video playing?
