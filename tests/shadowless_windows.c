// A window that paints its own shadow inside a transparent margin (WeChat's
// contact card) wears no sweater; one with the WindowServer's shadow keeps
// its original geometry. Scripted WindowServer only; no window is created.
#define main tracking_baseline_main
#include "tracking.c"
#undef main

static void check_shadow_classification(void) {
  CGRect bounds = CGRectMake(272, 444, 328, 328);
  // No WindowServer shadow: frame bounds equal bounds.
  assert(window_frame_lacks_system_shadow(bounds, bounds));
  // Sub-point rounding between the two reports is still "no shadow".
  assert(window_frame_lacks_system_shadow(bounds, CGRectInset(bounds, -.25, -.25)));
  // The inactive and active shadows WindowServer reports around ordinary windows.
  CGRect inactive = CGRectMake(bounds.origin.x - 23, bounds.origin.y - 16,
                               bounds.size.width + 46, bounds.size.height + 46);
  CGRect active = CGRectMake(bounds.origin.x - 56, bounds.origin.y - 38,
                             bounds.size.width + 112, bounds.size.height + 112);
  assert(!window_frame_lacks_system_shadow(bounds, inactive));
  assert(!window_frame_lacks_system_shadow(bounds, active));
  // A shadow on only one side is still a shadow.
  CGRect bottom_only = bounds; bottom_only.size.height += 30;
  assert(!window_frame_lacks_system_shadow(bounds, bottom_only));
  // Missing answers never hide anything.
  assert(!window_frame_lacks_system_shadow(CGRectZero, CGRectZero));
  assert(!window_frame_lacks_system_shadow(bounds, CGRectNull));
  puts("PASS: frame bounds without a WindowServer shadow are recognised; shadowed, partial and missing reports are not");
}

int main(void) {
  check_shadow_classification();

  g_settings.border_width=12;g_settings.border_style=BORDER_STYLE_KNIT;
  g_settings.border_order=BORDER_ORDER_BELOW;g_knit_on=true;
  struct border b={.cid=1,.wid=2,.target_wid=3,.sid=1,.radius=16,.inner_radius=17,
                   .metadata_dirty=true};
  pthread_mutexattr_t attr;pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&b.mutex,&attr);pthread_mutexattr_destroy(&attr);
  b.context=mock_context(1,2,NULL);

  // A self-shadowed popup appears: hidden, and nothing else is queried or drawn.
  target=CGRectMake(272,444,328,328);target_self_shadowed=true;
  int queries=shadow_queries, levels_read=level_queries, painted=draws;
  border_update(&b,false);
  assert(!b.visible && b.self_shadowed && b.metadata_dirty);
  assert(shadow_queries==queries+1 && level_queries==levels_read && draws==painted);

  // Moving it, showing it directly or repairing its depth keeps it hidden.
  target.origin=CGPointMake(300,460);
  border_update_geometry(&b);
  assert(!b.visible && shadow_queries==queries+2);
  int ordered=orders;
  border_unhide(&b);assert(!b.visible);
  border_reorder(&b);assert(!b.visible && applied_order==0);
  assert(orders>=ordered && level_queries==levels_read && draws==painted);

  // If the WindowServer starts drawing its shadow, the sweater returns with
  // the original outside geometry, depth and metadata.
  target_self_shadowed=false;
  border_update_geometry(&b);
  assert(b.visible && !b.self_shadowed && !b.metadata_dirty);
  assert(b.origin.x==300-12-BORDER_PADDING && b.origin.y==460-12-BORDER_PADDING);
  assert(CGSizeEqualToSize(b.drawing_bounds.size,target.size));
  assert(applied_order==BORDER_ORDER_BELOW && level_queries==levels_read+1);

  // Ordinary moves stay on the fast path: no extra WindowServer query.
  queries=shadow_queries;
  target.origin=CGPointMake(320,470);
  border_update_geometry(&b);
  assert(b.visible && shadow_queries==queries && b.origin.x==320-12-BORDER_PADDING);

  // A focus or Space change re-reads metadata and notices a lost shadow.
  b.metadata_dirty=true;target_self_shadowed=true;
  border_update(&b,false);
  assert(!b.visible && shadow_queries==queries+1);

  CGContextRelease(b.context);pthread_mutex_destroy(&b.mutex);
  puts("PASS: self-shadowed windows stay bare through update, move, unhide and reorder; restore and fast path unchanged");
  return 0;
}
