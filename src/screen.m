#import <AppKit/AppKit.h>
#include <assert.h>
#include <pthread.h>
#include "misc/screen.h"

static bool same_frame(CGRect a, CGRect b) {
  // Allow only WindowServer/AppKit pixel-rounding differences in a full frame.
  return fabs(a.origin.x-b.origin.x)<=1 && fabs(a.origin.y-b.origin.y)<=1
      && fabs(a.size.width-b.size.width)<=1 && fabs(a.size.height-b.size.height)<=1;
}
static CGRect quartz_frame(NSRect frame,CGFloat desktop_top) {
  return CGRectMake(NSMinX(frame),desktop_top-NSMaxY(frame),NSWidth(frame),NSHeight(frame));
}
bool knit_window_is_expanded(CGRect window) {
  assert(pthread_main_np());
  NSArray<NSScreen*>* screens=NSScreen.screens;
  if(!screens.count)return false;
  CGFloat desktop_top=NSMaxY(screens[0].frame);
  for(NSScreen* screen in screens) {
    if(same_frame(window,quartz_frame(screen.frame,desktop_top))
        || same_frame(window,quartz_frame(screen.visibleFrame,desktop_top)))return true;
  }
  return false;
}
