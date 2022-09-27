
/* Copyright (c) 2012-2014,2017-2018 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __KGSL_SYNC_H
#define __KGSL_SYNC_H

#include <linux/sync_file.h>
#include "kgsl_device.h"

#define KGSL_TIMELINE_NAME_LEN 32

/**
 * struct kgsl_sync_timeline - A sync timeline associated with a kgsl context
 * @kref: Refcount to keep the struct alive until all its fences are released
 * @name: String to describe this timeline
 * @fence_context: Used by the fence driver to identify fences belonging to
 *		   this context
 * @child_list_head: List head for all fences on this timeline
 * @lock: Spinlock to protect this timeline
 * @last_timestamp: Last timestamp when signaling fences
 * @device: kgsl device
 * @context: kgsl context
 */
struct kgsl_sync_timeline {
	struct kref kref;
	char name[KGSL_TIMELINE_NAME_LEN];

	u64 fence_context;

	struct list_head child_list_head;

	spinlock_t lock;
	unsigned int last_timestamp;
	struct kgsl_device *device;
	struct kgsl_context *context;
};

/**
 * struct kgsl_sync_fence - A struct containing a fence and other data
 *				associated with it
 * @fence: The fence struct
 * @sync_file: Pointer to the sync file
 * @parent: Pointer to the kgsl sync timeline this fence is on
 * @child_list: List of fences on the same timeline
 * @context_id: kgsl context id
 * @timestamp: Context timestamp that this fence is associated with
 */
struct kgsl_sync_fence {
	struct fence fence;
	struct sync_file *sync_file;
	struct kgsl_sync_timeline *parent;
	struct list_head child_list;
	u32 context_id;
	unsigned int timestamp;
};

/**
 * struct kgsl_sync_fence_cb - Used for fence callbacks
 * fence_cb: Fence callback struct
 * fence: Pointer to the fence for which the callback is done
 * priv: Private data for the callback
 * func: Pointer to the kgsl function to call. This function should return
 * false if the sync callback is marked for cancellation in a separate thread.
 */
struct kgsl_sync_fence_cb {
	struct fence_cb fence_cb;
	struct fence *fence;
	void *priv;
	bool (*func)(void *priv);
};

struct kgsl_syncsource;

#if defined(CONFIG_SYNC_FILE)
int kgsl_add_fence_event(struct kgsl_device *device,
	u32 context_id, u32 timestamp, void __user *data, int len,
	struct kgsl_device_private *owner);

int kgsl_sync_timeline_create(struct kgsl_context *context);

void kgsl_sync_timeline_destroy(struct kgsl_context *context);

void kgsl_sync_timeline_put(struct kgsl_sync_timeline *ktimeline);

struct kgsl_sync_fence_cb *kgsl_sync_fence_async_wait(int fd,
					bool (*func)(void *priv), void *priv,
					struct event_fence_info *info_ptr);

void kgsl_sync_fence_async_cancel(struct kgsl_sync_fence_cb *kcb);

long kgsl_ioctl_syncsource_create(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data);
long kgsl_ioctl_syncsource_destroy(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data);
long kgsl_ioctl_syncsource_create_fence(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data);
long kgsl_ioctl_syncsource_signal_fence(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data);

void kgsl_syncsource_put(struct kgsl_syncsource *syncsource);

void kgsl_syncsource_process_release_syncsources(
		struct kgsl_process_private *private);

void kgsl_dump_fence(struct kgsl_drawobj_sync_event *event,
					char *fence_str, int len);

#else
static inline int kgsl_add_fence_event(struct kgsl_device *device,
	u32 context_id, u32 timestamp, void __user *data, int len,
	struct kgsl_device_private *owner)
{
	return -EINVAL;
}

static inline int kgsl_sync_timeline_create(struct kgsl_context *context)
{
	context->ktimeline = NULL;
	return 0;
}

static inline void kgsl_sync_timeline_destroy(struct kgsl_context *context)
{
}

static inline void kgsl_sync_timeline_put(struct kgsl_sync_timeline *ktimeline)
{
}


static inline struct kgsl_sync_fence_cb *kgsl_sync_fence_async_wait(int fd,
					bool (*func)(void *priv), void *priv,
					struct event_fence_info *info_ptr)
{
	return NULL;
}

static inline void
kgsl_sync_fence_async_cancel(struct kgsl_sync_fence_cb *kcb)
{
}

static inline long
kgsl_ioctl_syncsource_create(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data)
{
	return -ENOIOCTLCMD;
}

static inline long
kgsl_ioctl_syncsource_destroy(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data)
{
	return -ENOIOCTLCMD;
}

static inline long
kgsl_ioctl_syncsource_create_fence(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data)
{
	return -ENOIOCTLCMD;
}

static inline long
kgsl_ioctl_syncsource_signal_fence(struct kgsl_device_private *dev_priv,
					unsigned int cmd, void *data)
{
	return -ENOIOCTLCMD;
}

static inline void kgsl_syncsource_put(struct kgsl_syncsource *syncsource)
{

}

static inline void kgsl_syncsource_process_release_syncsources(
		struct kgsl_process_private *private)
{

}

static inline void kgsl_dump_fence(struct kgsl_drawobj_sync_event *event,
					char *fence_str, int len)
{
}

#endif /* CONFIG_SYNC_FILE */

#endif /* __KGSL_SYNC_H */
