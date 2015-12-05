// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_BROWSER_GUEST_VIEW_EXTENSION_VIEW_EXTENSION_VIEW_GUEST_H_
#define EXTENSIONS_BROWSER_GUEST_VIEW_EXTENSION_VIEW_EXTENSION_VIEW_GUEST_H_

#include "base/macros.h"
#include "components/guest_view/browser/guest_view.h"
#include "extensions/browser/extension_function_dispatcher.h"
#include "url/gurl.h"

namespace content {
class BrowserContext;
}

namespace extensions {

class ExtensionViewGuest
    : public guest_view::GuestView<ExtensionViewGuest> {
 public:
  static const char Type[];
  static guest_view::GuestViewBase* Create(
      content::WebContents* owner_web_contents);

  // Request navigating the guest to the provided |src| URL.
  // Returns true if the navigation is successful.
  bool NavigateGuest(const std::string& src, bool force_navigation);

  // GuestViewBase implementation.
  bool CanRunInDetachedState() const override;
  void CreateWebContents(const base::DictionaryValue& create_params,
                         const WebContentsCreatedCallback& callback) override;
  void DidInitialize(const base::DictionaryValue& create_params) override;
  void DidAttachToEmbedder() override;
  const char* GetAPINamespace() const override;
  int GetTaskPrefix() const override;

  // content::WebContentsObserver implementation.
  void DidCommitProvisionalLoadForFrame(
      content::RenderFrameHost* render_frame_host,
      const GURL& url,
      ui::PageTransition transition_type) override;
  void DidNavigateMainFrame(
      const content::LoadCommittedDetails& details,
      const content::FrameNavigateParams& params) override;

 private:
  ExtensionViewGuest(content::WebContents* owner_web_contents);
  ~ExtensionViewGuest() override;

  // Applies attributes to the extensionview.
  void ApplyAttributes(const base::DictionaryValue& params);

  // The full URL that the extensionview is currently navigated to.
  GURL url_;

  // The extension URL, including the extension scheme and extension ID.
  GURL extension_url_;

  DISALLOW_COPY_AND_ASSIGN(ExtensionViewGuest);
};

}  // namespace extensions

#endif  // EXTENSIONS_BROWSER_GUEST_VIEW_EXTENSION_VIEW_EXTENSION_VIEW_GUEST_H_