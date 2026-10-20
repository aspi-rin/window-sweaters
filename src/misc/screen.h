#pragma once
#include <CoreGraphics/CoreGraphics.h>
#include <stdbool.h>

// Fullscreen-sized or maximized-to-work-area window, in WindowServer coordinates.
// Ordinary windows retain the original rendering regardless of edge proximity.
bool knit_window_is_expanded(CGRect window);
