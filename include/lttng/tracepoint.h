/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/tracepoint.h
 *
 * LTTng adaptation layer for Linux kernel 3.15+ tracepoints.
 *
 * Copyright (C) 2014 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_TRACEPOINT_H
#define _LTTNG_TRACEPOINT_H

#if 1	//TODO use kernel version.
#define	LTTNG_TRACEPOINT_MAY_FAULT	TRACEPOINT_MAY_FAULT
#else
#define	LTTNG_TRACEPOINT_MAY_FAULT	0
#endif

int lttng_tracepoint_probe_register(const char *name, void *probe, void *data, unsigned int flags);
int lttng_tracepoint_probe_unregister(const char *name, void *probe, void *data);
int lttng_tracepoint_init(void);
void lttng_tracepoint_exit(void);

#endif /* _LTTNG_TRACEPOINT_H */
