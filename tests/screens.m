// Native screen-state adapter with scripted displays. No visible windows.
#import <AppKit/AppKit.h>
#import <objc/runtime.h>
#include <assert.h>
#include "misc/screen.h"
@interface FixtureScreen : NSObject
@property NSRect frame;
@property NSRect visibleFrame;
@end
@implementation FixtureScreen
@end
static NSArray* fixture_screens;
static NSArray* scripted_screens(id receiver,SEL selector){(void)receiver;(void)selector;return fixture_screens;}
static FixtureScreen* screen(NSRect frame,NSRect visible){
  FixtureScreen* s=[FixtureScreen new];s.frame=frame;s.visibleFrame=visible;return s;
}
int main(void){@autoreleasepool{
  Method method=class_getClassMethod(NSScreen.class,@selector(screens));
  IMP original=method_setImplementation(method,(IMP)scripted_screens);
  FixtureScreen* primary=screen(NSMakeRect(0,0,1440,900),NSMakeRect(0,60,1440,815));
  FixtureScreen* left=screen(NSMakeRect(-1920,-180,1920,1080),NSMakeRect(-1880,-180,1880,1055));
  FixtureScreen* above=screen(NSMakeRect(0,900,1440,900),NSMakeRect(0,900,1440,875));
  fixture_screens=@[primary,left,above];
  assert(knit_window_is_expanded(CGRectMake(0,0,1440,900)));
  assert(knit_window_is_expanded(CGRectMake(0,25,1440,815)));
  assert(knit_window_is_expanded(CGRectMake(-1920,0,1920,1080)));
  assert(knit_window_is_expanded(CGRectMake(-1880,25,1880,1055)));
  assert(knit_window_is_expanded(CGRectMake(0,-900,1440,900)));
  assert(knit_window_is_expanded(CGRectMake(0,-875,1440,875)));
  assert(knit_window_is_expanded(CGRectMake(.5,25.5,1439.5,814.5))); // pixel rounding only
  // Edge/corner contact, near-max size, height-only zoom and spanning windows
  // all retain their normal sweater: only a complete expanded frame hides it.
  assert(!knit_window_is_expanded(CGRectMake(0,25,700,400)));
  assert(!knit_window_is_expanded(CGRectMake(740,440,700,400)));
  assert(!knit_window_is_expanded(CGRectMake(0,25,1435,810)));
  assert(!knit_window_is_expanded(CGRectMake(100,25,700,815)));
  assert(!knit_window_is_expanded(CGRectMake(-100,25,1440,815)));
  // Dock changes affect the maximized frame; ordinary geometry stays visible.
  primary.visibleFrame=NSMakeRect(80,0,1360,875);
  assert(knit_window_is_expanded(CGRectMake(80,25,1360,875)));
  assert(!knit_window_is_expanded(CGRectMake(0,25,1440,815)));
  fixture_screens=@[];assert(!knit_window_is_expanded(CGRectMake(0,0,1440,900)));
  method_setImplementation(method,original);
  puts("PASS: full/maximize, primary/left/above displays, ordinary edges, partial zoom, Dock work area and rounding");
  return 0;
}}
