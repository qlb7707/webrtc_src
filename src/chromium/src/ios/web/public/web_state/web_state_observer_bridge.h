// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IOS_WEB_PUBLIC_WEB_STATE_WEB_STATE_OBSERVER_BRIDGE_H_
#define IOS_WEB_PUBLIC_WEB_STATE_WEB_STATE_OBSERVER_BRIDGE_H_

#import <Foundation/Foundation.h>

#include <string>

#import "base/ios/weak_nsobject.h"
#import "ios/web/public/web_state/web_state_observer.h"

class GURL;

// Observes page lifecyle events from Objective-C. To use as a
// web::WebStateObserver, wrap in a web::WebStateObserverBridge.
@protocol CRWWebStateObserver<NSObject>
@optional

// Invoked by WebStateObserverBridge::ProvisionalNavigationStarted.
- (void)webState:(web::WebState*)webState
    didStartProvisionalNavigationForURL:(const GURL&)URL;

// Invoked by WebStateObserverBridge::NavigationItemCommitted.
- (void)webState:(web::WebState*)webState
    didCommitNavigationWithDetails:
        (const web::LoadCommittedDetails&)load_details;

// Invoked by WebStateObserverBridge::PageLoaded.
- (void)webStateDidLoadPage:(web::WebState*)webState;

// Invoked by WebStateObserverBridge::InterstitialDismissed.
- (void)webStateDidDismissInterstitial:(web::WebState*)webState;

// Invoked by WebStateObserverBridge::UrlHashChanged.
- (void)webStateDidChangeURLHash:(web::WebState*)webState;

// Invoked by WebStateObserverBridge::HistoryStateChanged.
- (void)webStateDidChangeHistoryState:(web::WebState*)webState;

// Invoked by WebStateObserverBridge::DocumentSubmitted.
- (void)webState:(web::WebState*)webState
    didSubmitDocumentWithFormNamed:(const std::string&)formName
                     userInitiated:(BOOL)userInitiated;

// Invoked by WebStateObserverBridge::FormActivityRegistered.
// TODO(ios): Method should take data transfer object rather than parameters.
- (void)webState:(web::WebState*)webState
    didRegisterFormActivityWithFormNamed:(const std::string&)formName
                               fieldName:(const std::string&)fieldName
                                    type:(const std::string&)type
                                   value:(const std::string&)value
                                 keyCode:(int)keyCode
                            inputMissing:(BOOL)inputMissing;

// Invoked by WebStateObserverBridge::AutocompleteRequested.
- (void)webState:(web::WebState*)webState
    requestAutocompleteForFormNamed:(const std::string&)formName
                          sourceURL:(const GURL&)sourceURL
                      userInitiated:(BOOL)userInitiated;

// Invoked by WebStateObserverBridge::FaviconUrlUpdated.
- (void)webState:(web::WebState*)webState
    didUpdateFaviconURLCandidates:
        (const std::vector<web::FaviconURL>&)candidates;

// Note: after |webStateDestroyed:| is invoked, the WebState being observed
// is no longer valid.
- (void)webStateDestroyed:(web::WebState*)webState;

// Invoked by WebStateObserverBridge::DidStopLoading.
- (void)webStateDidStopLoading:(web::WebState*)webState;

// Invoked by WebStateObserverBridge::DidStartLoading.
- (void)webStateDidStartLoading:(web::WebState*)webState;

@end

namespace web {

class WebState;

// Bridge to use an id<CRWWebStateObserver> as a web::WebStateObserver.
// Will be added/removed as an observer of the underlying WebState during
// construction/destruction. Instances should be owned by instances of the
// class they're bridging.
class WebStateObserverBridge : public web::WebStateObserver {
 public:
  WebStateObserverBridge(web::WebState* web_state,
                         id<CRWWebStateObserver> observer);
  ~WebStateObserverBridge() override;

  // web::WebStateObserver methods.
  void ProvisionalNavigationStarted(const GURL& url) override;
  void NavigationItemCommitted(
      const LoadCommittedDetails& load_details) override;
  void PageLoaded(
      web::PageLoadCompletionStatus load_completion_status) override;
  void InsterstitialDismissed() override;
  void UrlHashChanged() override;
  void HistoryStateChanged() override;
  void DocumentSubmitted(const std::string& form_name,
                         bool user_initiated) override;
  void FormActivityRegistered(const std::string& form_name,
                              const std::string& field_name,
                              const std::string& type,
                              const std::string& value,
                              int key_code,
                              bool input_missing) override;
  void AutocompleteRequested(const GURL& source_url,
                             const std::string& form_name,
                             bool user_initiated) override;
  void FaviconUrlUpdated(const std::vector<FaviconURL>& candidates) override;
  void WebStateDestroyed() override;
  void DidStartLoading() override;
  void DidStopLoading() override;

 private:
  base::WeakNSProtocol<id<CRWWebStateObserver>> observer_;
  DISALLOW_COPY_AND_ASSIGN(WebStateObserverBridge);
};

}  // namespace web

#endif  // IOS_WEB_PUBLIC_WEB_STATE_WEB_STATE_OBSERVER_BRIDGE_H_