// Fullscreen/maximize hides only the decoration; ordinary geometry stays original.
#define knit_window_is_expanded mock_expanded
#define main tracking_baseline_main
#include "tracking.c"
#undef main
static bool expanded;
bool knit_window_is_expanded(CGRect frame) { (void)frame; return expanded; }

int main(void) {
  g_settings.border_width=12;g_settings.border_style=BORDER_STYLE_KNIT;
  g_settings.border_order=BORDER_ORDER_BELOW;g_knit_on=true;
  struct border b={.cid=1,.wid=2,.target_wid=3,.sid=1,.radius=10,.inner_radius=10};
  pthread_mutexattr_t attr;pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&b.mutex,&attr);pthread_mutexattr_destroy(&attr);
  b.context=mock_context(1,2,NULL);
  target=CGRectMake(0,25,1440,850);expanded=true;
  border_update(&b,false);
  assert(!b.visible);
  border_unhide(&b);assert(!b.visible);
  border_reorder(&b);assert(!b.visible && applied_order==0);
  // Exiting maximize/fullscreen restores the original outside geometry/depth.
  expanded=false;target=CGRectMake(0,25,700,400);
  border_update_geometry(&b);
  assert(b.visible && b.origin.x==-20 && b.origin.y==5);
  assert(CGSizeEqualToSize(b.drawing_bounds.size,target.size));
  assert(applied_order==BORDER_ORDER_BELOW);
  // Ordinary near-edge windows remain visible and retain their exact bounds.
  target.origin=CGPointMake(740,475);border_update_geometry(&b);
  assert(b.visible && b.origin.x==720 && b.origin.y==455);
  assert(CGRectEqualToRect(b.target_bounds,target));
  // Showing an existing overlay directly must also respect current state.
  expanded=true;border_unhide(&b);assert(!b.visible);
  expanded=false;border_update_geometry(&b);assert(b.visible);
  CGContextRelease(b.context);pthread_mutex_destroy(&b.mutex);
  puts("PASS: full/maximize hide, unhide/reorder protection, restore, ordinary edges retain original geometry");
  return 0;
}
