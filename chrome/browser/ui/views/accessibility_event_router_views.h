// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_ACCESSIBILITY_EVENT_ROUTER_VIEWS_H_
#define CHROME_BROWSER_UI_VIEWS_ACCESSIBILITY_EVENT_ROUTER_VIEWS_H_
#pragma once

#include <string>

#include "base/basictypes.h"
#include "base/gtest_prod_util.h"
#include "base/singleton.h"
#include "base/task.h"
#include "chrome/browser/accessibility_events.h"
#include "views/accessibility/accessibility_types.h"

class Profile;
namespace views {
class View;
}

// NOTE: This class is part of the Accessibility Extension API, which lets
// extensions receive accessibility events. It's distinct from code that
// implements platform accessibility APIs like MSAA or ATK.
//
// Singleton class that adds listeners to many views, then sends an
// accessibility notification whenever a relevant event occurs in an
// accessible view.
//
// Views are not accessible by default. When you register a root widget,
// that widget and all of its descendants will start sending accessibility
// event notifications. You can then override the default behavior for
// specific descendants using other methods.
//
// You can use Profile::PauseAccessibilityEvents to prevent a flurry
// of accessibility events when a window is being created or initialized.
class AccessibilityEventRouterViews {
 public:
  // Internal information about a particular view to override the
  // information we get directly from the view.
  struct ViewInfo {
    ViewInfo() : ignore(false) {}

    // If nonempty, will use this name instead of the view's label.
    std::string name;

    // If true, will ignore this widget and not send accessibility events.
    bool ignore;
  };

  // Get the single instance of this class.
  static AccessibilityEventRouterViews* GetInstance();

  // Handle an accessibility event generated by a view.
  void HandleAccessibilityEvent(
      views::View* view, AccessibilityTypes::Event event_type);

 private:
  AccessibilityEventRouterViews();
  virtual ~AccessibilityEventRouterViews();

  friend struct DefaultSingletonTraits<AccessibilityEventRouterViews>;
  FRIEND_TEST_ALL_PREFIXES(AccessibilityEventRouterViewsTest,
                           TestFocusNotification);

  // Checks the type of the view and calls one of the more specific
  // Send*Notification methods, below.
  void DispatchAccessibilityNotification(
      views::View* view, NotificationType type);

  // Return the name of a view.
  std::string GetViewName(views::View* view);

  // Each of these methods constructs an AccessibilityControlInfo object
  // and sends a notification of a specific accessibility event.
  void SendButtonNotification(
      views::View* view, NotificationType type, Profile* profile);
  void SendLinkNotification(
      views::View* view, NotificationType type, Profile* profile);
  void SendMenuNotification(
      views::View* view, NotificationType type, Profile* profile);
  void SendMenuItemNotification(
      views::View* view, NotificationType type, Profile* profile);
  void SendLocationBarNotification(
      views::View* view, NotificationType type, Profile* profile);

  // Return true if it's an event on a menu.
  bool IsMenuEvent(views::View* view, NotificationType type);

  // Recursively explore all menu items of |menu| and return in |count|
  // the total number of items, and in |index| the 0-based index of
  // |item|, if found. Initialize |count| to zero before calling this
  // method. |index| will be unchanged if the item is not found, so
  // initialize it to -1 to detect this case.
  void RecursiveGetMenuItemIndexAndCount(
      views::View* menu, views::View* item, int* index, int* count);

  // The profile associated with the most recent window event  - used to
  // figure out where to route a few events that can't be directly traced
  // to a window with a profile (like menu events).
  Profile* most_recent_profile_;

  // Used to defer handling of some events until the next time
  // through the event loop.
  ScopedRunnableMethodFactory<AccessibilityEventRouterViews> method_factory_;
};

#endif  // CHROME_BROWSER_UI_VIEWS_ACCESSIBILITY_EVENT_ROUTER_VIEWS_H_
