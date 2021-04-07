/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-ppid.c
 *
 * LTTng PPID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <lttng/events.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng/tracer.h>

static
size_t ppid_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(pid_t));
	size += sizeof(pid_t);
	return size;
}

static
void ppid_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	pid_t ppid;

	/*
	 * TODO: when we eventually add RCU subsystem instrumentation,
	 * taking the rcu read lock here will trigger RCU tracing
	 * recursively. We should modify the kernel synchronization so
	 * it synchronizes both for RCU and RCU sched, and rely on
	 * rcu_read_lock_sched_notrace.
	 */
	rcu_read_lock();
	ppid = task_tgid_nr(current->real_parent);
	rcu_read_unlock();
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(ppid));
	chan->ops->event_write(ctx, &ppid, sizeof(ppid));
}

static
void ppid_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	pid_t ppid;

	/*
	 * TODO: when we eventually add RCU subsystem instrumentation,
	 * taking the rcu read lock here will trigger RCU tracing
	 * recursively. We should modify the kernel synchronization so
	 * it synchronizes both for RCU and RCU sched, and rely on
	 * rcu_read_lock_sched_notrace.
	 */
	rcu_read_lock();
	ppid = task_tgid_nr(current->real_parent);
	rcu_read_unlock();
	value->s64 = ppid;
}

int lttng_add_ppid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "ppid")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "ppid";
	field->event_field.type.type = lttng_kernel_type_integer;
	field->event_field.type.u.integer.size = sizeof(pid_t) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(pid_t) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(pid_t);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = ppid_get_size;
	field->record = ppid_record;
	field->get_value = ppid_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_ppid_to_ctx);
