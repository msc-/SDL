/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_UIKIT

#include "SDL_video.h"
#include "SDL_assert.h"
#include "SDL_hints.h"
#include "../SDL_sysvideo.h"
#include "../../events/SDL_events_c.h"

#include "SDL_uikitviewcontroller.h"
#include "SDL_uikitvideo.h"
#include "SDL_uikitmodes.h"
#include "SDL_uikitwindow.h"


@implementation SDL_uikitviewcontroller

@synthesize window;

- (id)initWithSDLWindow:(SDL_Window *)_window
{
    self = [self init];
    if (self == nil) {
        return nil;
    }
    self.window = _window;

    return self;
}

- (void)loadView
{
    /* do nothing. */
}

- (void)viewDidLayoutSubviews
{
    SDL_WindowData *data = window->driverdata;
    const CGSize size = data->view.bounds.size;
    int w = (int) size.width;
    int h = (int) size.height;

    SDL_SendWindowEvent(window, SDL_WINDOWEVENT_RESIZED, w, h);
}

- (NSUInteger)supportedInterfaceOrientations
{
    NSUInteger orientationMask = 0;
    const char *orientationsHint = SDL_GetHint(SDL_HINT_ORIENTATIONS);

    if (orientationsHint != NULL) {
        NSArray *orientations = [@(orientationsHint) componentsSeparatedByCharactersInSet:
                                 [NSCharacterSet characterSetWithCharactersInString:@" "]];

        if ([orientations containsObject:@"LandscapeLeft"]) {
            orientationMask |= UIInterfaceOrientationMaskLandscapeLeft;
        }
        if ([orientations containsObject:@"LandscapeRight"]) {
            orientationMask |= UIInterfaceOrientationMaskLandscapeRight;
        }
        if ([orientations containsObject:@"Portrait"]) {
            orientationMask |= UIInterfaceOrientationMaskPortrait;
        }
        if ([orientations containsObject:@"PortraitUpsideDown"]) {
            orientationMask |= UIInterfaceOrientationMaskPortraitUpsideDown;
        }
    }

    if (orientationMask == 0 && (window->flags & SDL_WINDOW_RESIZABLE)) {
        orientationMask = UIInterfaceOrientationMaskAll;  /* any orientation is okay. */
    }

    if (orientationMask == 0) {
        if (window->w >= window->h) {
            orientationMask |= UIInterfaceOrientationMaskLandscape;
        }
        if (window->h >= window->w) {
            orientationMask |= (UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown);
        }
    }

    /* Don't allow upside-down orientation on the phone, so answering calls is in the natural orientation */
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        orientationMask &= ~UIInterfaceOrientationMaskPortraitUpsideDown;
    }
    return orientationMask;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)orient
{
    NSUInteger orientationMask = [self supportedInterfaceOrientations];
    return (orientationMask & (1 << orient));
}

- (BOOL)prefersStatusBarHidden
{
    if (window->flags & (SDL_WINDOW_FULLSCREEN|SDL_WINDOW_BORDERLESS)) {
        return YES;
    } else {
        return NO;
    }
}

- (UIStatusBarStyle)preferredStatusBarStyle
{
#ifdef __IPHONE_7_0
    /* We assume most games don't have a bright white background. */
    return UIStatusBarStyleLightContent;
#else
    /* This method is only used in iOS 7+, so the return value here isn't important. */
    return UIStatusBarStyleBlackTranslucent;
#endif
}

@end

#endif /* SDL_VIDEO_DRIVER_UIKIT */

/* vi: set ts=4 sw=4 expandtab: */
