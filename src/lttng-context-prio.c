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
#include <lttng/events.h>
#include <ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/kallsyms.h>
#include <lttng/tracer.h>

static
int (*wrapper_task_prio_sym)(struct task_struct *t);

int wrapper_task_prio_init(void)
{
	wrapper_task_prio_sym = (void *) kallsyms_lookup_funcptr("task_prio");
	if (!wrapper_task_prio_sym) {
		printk(KERN_WARNING "LTTng: task_prio symbol lookup failed.\n");
		return -EINVAL;
	}
	return 0;
}

/*
 * Canary function to check for 'task_prio()' at compile time.
 *
 * From 'include/linux/sched.h':
 *
 *   extern int task_prio(const struct task_struct *p);
 */
__attribute__((unused)) static
int __canary__task_prio(const struct task_struct *p)
{
	return task_prio(p);
}

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

	prio = wrapper_task_prio_sym(current);
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(prio));
	chan->ops->event_write(ctx, &prio, sizeof(prio));
}

static
void prio_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = wrapper_task_prio_sym(current);
}

int lttng_add_prio_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;
	int ret;

	if (!wrapper_task_prio_sym) {
		ret = wrapper_task_prio_init();
		if (ret)
			return ret;
	}

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "prio")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "prio";
	field->event_field.type.type = lttng_kernel_type_integer;
	field->event_field.type.u.integer.size = sizeof(int) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(int) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(int);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_kernel_string_encoding_none;
	field->get_size = prio_get_size;
	field->record = prio_record;
	field->get_value = prio_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_prio_to_ctx);
