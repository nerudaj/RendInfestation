# Android Help

## TouchController poorly registers inputs/inputs are offset from real touch location

This happens when the application is not allowed to render into camera cutout area. SFML reports Window size equal to your phone resolution, but the OS is forcing it to render in a lower area.

I haven't found any JNI/NDK API to detect this behavior, so instead my recommended fix is to configure the application to render into cutout area, so the dimensions match.

To do so, make sure that `/cmake/vars.cmake` variable called `ANDROID_WINDOW_LAYOUT_IN_DISPLAY_CUTOUT_MODE` is set to `shortEdges`.