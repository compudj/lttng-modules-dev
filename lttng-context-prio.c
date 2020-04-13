/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-prio.c
 *
 * LTTng priority context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <lttng-tracer.h>

static
size_t prio_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(int));
	size += sizeof(int);
	return size;
}

static
void prio_record(struct lttng_ctx_field *field,
		struct lib_ring_buffer_ctx *ctx,
		struct lttng_channel *chan)
{
	int prio;

	prio = task_prio(current);
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(prio));
	chan->ops->event_write(ctx, &prio, sizeof(prio));
}

static
void prio_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = task_prio(current);
}

int lttng_add_prio_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "prio")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "prio";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.integer.size = sizeof(int) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(int) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(int);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = prio_get_size;
	field->record = prio_record;
	field->get_value = prio_get_value;
	lttng_context_update(*ctx);
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_prio_to_ctx);
