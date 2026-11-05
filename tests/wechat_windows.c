// Real renderer lifecycle; only the foreign metadata boundary is scripted.
#define main tracking_baseline_main
#include "tracking.c"
#undef main
int main(void) {
  g_settings.border_width=12;g_settings.border_style=BORDER_STYLE_KNIT;
  g_settings.border_order=BORDER_ORDER_BELOW;g_knit_on=true;
  struct border b={.cid=1,.wid=2,.target_wid=3,.owner_pid=506,.sid=1,
    .radius=16,.inner_radius=17,.metadata_dirty=true};
  pthread_mutexattr_t attr;pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&b.mutex,&attr);pthread_mutexattr_destroy(&attr);
  b.context=mock_context(1,2,NULL);target=CGRectMake(272,444,328,328);
  target_self_shadowed=false;
  target_window_supported=true;border_update(&b,false);assert(b.visible);
  target_window_supported=false;border_update(&b,false);assert(!b.visible);
  border_unhide(&b);assert(!b.visible);
  border_reorder(&b);assert(!b.visible && applied_order==0);
  target.origin.x+=20;border_update_geometry(&b);assert(!b.visible);
  target_window_supported=true;border_update(&b,false);assert(b.visible);
  // A reused window changing its role is rejected at each direct reveal path.
  target_window_supported=false;border_unhide(&b);assert(!b.visible);
  target_window_supported=true;border_update(&b,false);assert(b.visible);
  target_window_supported=false;border_reorder(&b);assert(!b.visible);
  target_window_supported=true;border_update(&b,false);assert(b.visible);
  CGContextRelease(b.context);pthread_mutex_destroy(&b.mutex);
  puts("PASS: unsupported WeChat windows stay bare through update, geometry, unhide and reorder; verified windows restore");
  return 0;
}
