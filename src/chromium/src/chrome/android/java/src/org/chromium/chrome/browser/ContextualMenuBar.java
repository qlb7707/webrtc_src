// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.content.Context;
import android.content.res.TypedArray;
import android.support.v7.app.ActionBar;
import android.util.Property;

import org.chromium.chrome.R;

/**
 * This class handles the animation for the contextual menu bar and adds a custom
 * ActionMode.Callback to the menu bar.
 */
public class ContextualMenuBar {

    private static final int SLIDE_DURATION_MS = 200;

    private CustomSelectionActionModeCallback mCustomSelectionActionModeCallback;
    private ObjectAnimator mCurrentAnimation = null;
    private boolean mShowingContextualActionBar = false;
    private float mTabStripHeight;
    private final Context mContext;
    private final ActionBarDelegate mActionBarDelegate;

    /** Property for animating the top margin of ActionBarDelegate. */
    public static final Property<ActionBarDelegate, Integer> TOP_MARGIN_ANIM_PROPERTY =
            new Property<ActionBarDelegate, Integer>(Integer.class, "controlTopMargin") {
                @Override
                public Integer get(ActionBarDelegate delegate) {
                    return delegate.getControlTopMargin();
                }
                @Override
                public void set(ActionBarDelegate delegate, Integer value) {
                    delegate.setControlTopMargin(value);
                }
            };

    /**
     * This is an interface for objects that the contextualMenuBar can use for animating the action
     * bar.
     */
    public interface ActionBarDelegate {

        /**
         * Sets the top margin of the control container.
         * @param margin The new top margin of the control container.
         */
        public void setControlTopMargin(int margin);

        /**
         * @return The top margin of the control container.
         */
        public int getControlTopMargin();

        /**
         * @return The action bar that will be animated in and out.
         */
        public ActionBar getSupportActionBar();

        /**
         * Change the background visibility for the action bar.
         * @param visible Whether the background should be visible.
         */
        public void setActionBarBackgroundVisibility(boolean visible);
    }

    /**
     * Creates the contextual menu bar and ties it to an action bar using the given action bar
     * delegate.
     * @param context The context which the contextual action menu bar should be using for accessing
     *                resources.
     * @param actionBarDelegate The delegate for communicating with the action bar while animating
     *                          it.
     */
    public ContextualMenuBar(Context context, ActionBarDelegate actionBarDelegate) {
        mActionBarDelegate = actionBarDelegate;
        mContext = context;
        mTabStripHeight = mContext.getResources().getDimension(R.dimen.tab_strip_height);
    }

    /**
     * @return The delegate handling action bar positioning for the contextual menu bar.
     */
    public ActionBarDelegate getActionBarDelegate() {
        return mActionBarDelegate;
    }

    /**
     * Sets the custom ActionMode.Callback
     * @param customSelectionActionModeCallback
     */
    public void setCustomSelectionActionModeCallback(
            CustomSelectionActionModeCallback customSelectionActionModeCallback) {
        if (customSelectionActionModeCallback.equals(mCustomSelectionActionModeCallback)) return;
        mCustomSelectionActionModeCallback = customSelectionActionModeCallback;
        mCustomSelectionActionModeCallback.setContextualMenuBar(this);
    }

    /**
     * @return The custom ActionMode.Callback.
     */
    public CustomSelectionActionModeCallback getCustomSelectionActionModeCallback() {
        return mCustomSelectionActionModeCallback;
    }

    /**
     * @return The current action bar height.
     */
    private int queryCurrentActionBarHeight() {
        ActionBar actionBar = mActionBarDelegate.getSupportActionBar();
        if (actionBar != null) return actionBar.getHeight();

        TypedArray styledAttributes =
                mContext.obtainStyledAttributes(new int[] {R.attr.actionBarSize});
        int height = styledAttributes.getDimensionPixelSize(0, 0);
        styledAttributes.recycle();
        return height;
    }

    /**
     * Show controls after orientation change if previously visible.
     */
    public void showControlsOnOrientationChange() {
        if (mShowingContextualActionBar && mCurrentAnimation == null) {
            showControls();
        }
    }

    /**
     * Animation for the textview if the action bar is visible.
     */
    public void showControls() {
        if (mCurrentAnimation != null) mCurrentAnimation.cancel();

        mCurrentAnimation = ObjectAnimator.ofInt(mActionBarDelegate, TOP_MARGIN_ANIM_PROPERTY,
                (int) (Math.max(0, queryCurrentActionBarHeight() - mTabStripHeight))).setDuration(
                SLIDE_DURATION_MS);

        mCurrentAnimation.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                mCurrentAnimation = null;
            }
        });

        mCurrentAnimation.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                ActionBar actionBar = mActionBarDelegate.getSupportActionBar();
                if (actionBar != null) {
                    animation.setIntValues((int) (Math.max(0,
                            queryCurrentActionBarHeight() - mTabStripHeight)));
                }
            }
        });

        mActionBarDelegate.setActionBarBackgroundVisibility(true);
        mCurrentAnimation.start();
        mShowingContextualActionBar = true;
    }

    /**
     * Hide animation for the textview if the action bar is not visible.
     */
    public void hideControls() {
        if (mCurrentAnimation != null) mCurrentAnimation.cancel();

        mCurrentAnimation = ObjectAnimator.ofInt(mActionBarDelegate, TOP_MARGIN_ANIM_PROPERTY,
                0).setDuration(SLIDE_DURATION_MS);

        mCurrentAnimation.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                mCurrentAnimation = null;
                mActionBarDelegate.setActionBarBackgroundVisibility(false);
            }
        });

        mCurrentAnimation.start();
        mShowingContextualActionBar = false;
    }
}