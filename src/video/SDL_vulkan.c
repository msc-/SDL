/*
 Simple DirectMedia Layer
 Copyright (C) 2017, Mark Callow.
 
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

/**
 *  \file SDL_vulkan.c
 *
 *  Functions for creating Vulkan surfaces on SDL windows.
 *
 *  Thanks to David McFarland (@corngood on GitHub) for providing the starting
 *  point for this.
 *
 */

#include <string.h>

#include "../SDL_internal.h"

#include <SDL_config.h>
#include <SDL_video.h>  /* For SDL_Window */
/* The following is necessary when compiling this file outside of SDL
 * due to the linux build using a generated SDL_config.h which is not
 * visible outside SDL. The other platforms use the public SDL_config.h.
 */
#if __LINUX__
#define SDL_VIDEO_DRIVER_WAYLAND 1
#define SDL_VIDEO_DRIVER_X11 1
#endif

#if SDL_VIDEO_DRIVER_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#elif SDL_VIDEO_DRIVER_COCOA
#define VK_USE_PLATFORM_MACOS_MVK
typedef struct _SDL_metalview SDL_metalview;
SDL_metalview* SDL_AddMetalView(SDL_Window* window);
#elif SDL_VIDEO_DRIVER_UIKIT
#define VK_USE_PLATFORM_IOS_MVK
typedef struct _SDL_metalview SDL_metalview;
SDL_metalview* SDL_AddMetalView(SDL_Window* window);
#elif SDL_VIDEO_DRIVER_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#else
#if SDL_VIDEO_DRIVER_WAYLAND
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif
#if SDL_VIDEO_DRIVER_X11
#define VK_USE_PLATFORM_XCB_KHR
#include <X11/Xlib-xcb.h>
#endif
#endif
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <SDL_syswm.h>

static int
SetNames(unsigned int capacity, const char** names,
         unsigned inCount, const char* const* inNames) {
    if (names) {
        if (capacity < inCount) {
            SDL_SetError("Insufficient capacity for extension names: %u < %u",
                         capacity, inCount);
            return 0;
        }
        for (unsigned i = 0; i < inCount; ++i)
            names[i] = inNames[i];
    }
    return inCount;
}

int
SDL_GetVulkanInstanceExtensions(unsigned length, const char** names) {
    const char *driver = SDL_GetCurrentVideoDriver();
    if (!driver) {
        SDL_SetError("No video driver - has SDL_Init(SDL_INIT_VIDEO) been called?");
        return 0;
    }
#if SDL_VIDEO_DRIVER_ANDROID
    if (!strcmp(driver, "android")) {
        const char* ext[] = { VK_KHR_ANDROID_SURFACE_EXTENSION_NAME };
        return SetNames(length, names, 1, ext);
    }
#endif
#if SDL_VIDEO_DRIVER_COCOA
    if (!strcmp(driver, "cocoa")) {
        const char* ext[] = { VK_MVK_MACOS_SURFACE_EXTENSION_NAME };
        return SetNames(length, names, 1, ext);
    }
#endif
#if SDL_VIDEO_DRIVER_UIKIT
    if (!strcmp(driver, "uikit")) {
        const char* ext[] = { VK_MVK_IOS_SURFACE_EXTENSION_NAME };
        return SetNames(length, names, 1, ext);
    }
#endif
#if SDL_VIDEO_DRIVER_WAYLAND
    if (!strcmp(driver, "wayland")) {
        const char* ext[] = { VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME };
        return SetNames(length, names, 1, ext);
    }
#endif
#if SDL_VIDEO_DRIVER_WINDOWS
    if (!strcmp(driver, "windows")) {
        const char* ext[] = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
        return SetNames(length, names, 1, ext);
    }
#endif
#if SDL_VIDEO_DRIVER_X11
    if (!strcmp(driver, "x11")) {
        const char* ext[] = { VK_KHR_XCB_SURFACE_EXTENSION_NAME };
        return SetNames(length, names, 1, ext);
    }
#endif

    (void)SetNames;
    (void)names;

    SDL_SetError("Unsupported video driver '%s'", driver);
    return SDL_FALSE;
}

int
SDL_CreateVulkanSurface(SDL_Window* window,
                        VkInstance instance, VkSurfaceKHR* surface) {
    if (!window) {
        SDL_SetError("'window' is null");
        return -1;
    }
    if (instance == VK_NULL_HANDLE) {
        SDL_SetError("'instance' is null");
        return -1;
    }

    SDL_SysWMinfo wminfo;
    SDL_VERSION(&wminfo.version);
    if (!SDL_GetWindowWMInfo(window, &wminfo))
        return -1;

    switch (wminfo.subsystem) {
#if SDL_VIDEO_DRIVER_ANDROID
    case SDL_SYSWM_ANDROID:
    {
        VkAndroidSurfaceCreateInfoKHR createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        createInfo.window = wminfo.info.android.window;

        VkResult r =
            vkCreateAndroidSurfaceKHR(instance, &createInfo, NULL, surface);
        if (r != VK_SUCCESS) {
            SDL_SetError("vkCreateAndroidSurfaceKHR failed: %i", (int)r);
            return -1;
        }
        return 0;
   }
#endif
#if SDL_VIDEO_DRIVER_UIKIT
    case SDL_SYSWM_UIKIT:
    {
#if !TARGET_OS_SIMULATOR
        VkIOSSurfaceCreateInfoMVK createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        // pView must be a reference to an NSView backed by a CALayer
        // instance of type CAMetalLayer.
        createInfo.pView = SDL_AddMetalView(window);
      
        if (createInfo.pView != NULL) {
            VkResult r =
            vkCreateIOSSurfaceMVK(instance, &createInfo, NULL, surface);
            if (r == VK_SUCCESS) {
                return 0;
            } else {
                SDL_SetError("vkCreateIOSSurfaceMVK failed: %i", (int)r);
            }
        }
        return -1;
#else
        SDL_SetError("Metal (& MoltenVK) not supported by the iOS simulator");
        return -1;
#endif
    }
#endif
#if SDL_VIDEO_DRIVER_COCOA
    case SDL_SYSWM_COCOA:
    {
#if TARGET_CPU_X86_64
        VkMacOSSurfaceCreateInfoMVK createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        // pView must be a reference to an NSView backed by a CALayer
        // instance of type CAMetalLayer.
        createInfo.pView = SDL_AddMetalView(window);
        
        if (createInfo.pView != NULL) {
            VkResult r =
            vkCreateMacOSSurfaceMVK(instance, &createInfo, NULL, surface);
            if (r == VK_SUCCESS) {
                return 0;
            } else {
                SDL_SetError("vkCreateMacOSSurfaceMVK failed: %i", (int)r);
            }
        }
        return -1;
#else
        SDL_SetError("MoltenVK is not supported on 32-bit architectures.");
        return -1;
#endif
    }
#endif
#if SDL_VIDEO_DRIVER_WINDOWS
    case SDL_SYSWM_WINDOWS:
    {
        VkWin32SurfaceCreateInfoKHR createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        createInfo.hinstance = wminfo.info.win.hdc; // XXX ???
        createInfo.hwnd = wminfo.info.win.window;

        VkResult r =
            vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, surface);
        if (r != VK_SUCCESS) {
            SDL_SetError("vkCreateAndroidSurfaceKHR failed: %i", (int)r);
            return -1;
        }
        return 0;
    }
#endif
#if SDL_VIDEO_DRIVER_X11
    case SDL_SYSWM_X11:
    {
        VkXcbSurfaceCreateInfoKHR createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = NULL;
        createInfo.flags = 0;
        createInfo.connection = XGetXCBConnection(wminfo.info.x11.display);
        createInfo.window = wminfo.info.x11.window;

        VkResult r = vkCreateXcbSurfaceKHR(instance, &createInfo, NULL, surface);
        if (r != VK_SUCCESS) {
            SDL_SetError("vkCreateXcbSurfaceKHR failed: %i", (int)r);
            return -1;
        }
        return 0;
    }
#endif
    default:
        (void)surface;
        SDL_SetError("Video driver (subsystem %i) does not support Vulkan",
                     (int)wminfo.subsystem);
        return 0;
    }
}
