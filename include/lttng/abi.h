/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng/abi.h
 *
 * LTTng ABI header
 *
 * Copyright (C) 2010-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#ifndef _LTTNG_ABI_H
#define _LTTNG_ABI_H

#include <linux/fs.h>

/*
 * Major/minor version of ABI exposed to lttng tools. Major number
 * should be increased when an incompatible ABI change is done.
 */
#define LTTNG_MODULES_ABI_MAJOR_VERSION		2
#define LTTNG_MODULES_ABI_MINOR_VERSION		5

#define LTTNG_KERNEL_SYM_NAME_LEN	256
#define LTTNG_KERNEL_SESSION_NAME_LEN	256
#define LTTNG_KERNEL_SESSION_CREATION_TIME_ISO8601_LEN	26

enum lttng_kernel_instrumentation {
	LTTNG_KERNEL_TRACEPOINT	= 0,
	LTTNG_KERNEL_KPROBE	= 1,
	LTTNG_KERNEL_FUNCTION	= 2,
	LTTNG_KERNEL_KRETPROBE	= 3,
	LTTNG_KERNEL_NOOP	= 4,	/* not hooked */
	LTTNG_KERNEL_SYSCALL	= 5,
	LTTNG_KERNEL_UPROBE	= 6,
};

/*
 * LTTng consumer mode
 */
enum lttng_kernel_output {
	LTTNG_KERNEL_SPLICE	= 0,
	LTTNG_KERNEL_MMAP	= 1,
};

/*
 * LTTng DebugFS ABI structures.
 */
#define LTTNG_KERNEL_CHANNEL_PADDING	LTTNG_KERNEL_SYM_NAME_LEN + 32
struct lttng_kernel_channel {
	uint64_t subbuf_size;			/* in bytes */
	uint64_t num_subbuf;
	unsigned int switch_timer_interval;	/* usecs */
	unsigned int read_timer_interval;	/* usecs */
	uint32_t output;			/* enum lttng_kernel_output (splice, mmap) */
	int overwrite;				/* 1: overwrite, 0: discard */
	char padding[LTTNG_KERNEL_CHANNEL_PADDING];
} __attribute__((packed));

struct lttng_kernel_kretprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

/*
 * Either addr is used, or symbol_name and offset.
 */
struct lttng_kernel_kprobe {
	uint64_t addr;

	uint64_t offset;
	char symbol_name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_function_tracer {
	char symbol_name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_uprobe {
	int fd;
} __attribute__((packed));

struct lttng_kernel_event_callsite_uprobe {
	uint64_t offset;
} __attribute__((packed));

struct lttng_kernel_event_callsite {
	union {
		struct lttng_kernel_event_callsite_uprobe uprobe;
	} u;
} __attribute__((packed));

enum lttng_kernel_syscall_entryexit {
	LTTNG_KERNEL_SYSCALL_ENTRYEXIT	= 0,
	LTTNG_KERNEL_SYSCALL_ENTRY	= 1,
	LTTNG_KERNEL_SYSCALL_EXIT	= 2,
};

enum lttng_kernel_syscall_abi {
	LTTNG_KERNEL_SYSCALL_ABI_ALL = 0,
	LTTNG_KERNEL_SYSCALL_ABI_NATIVE = 1,	/* Not implemented. */
	LTTNG_KERNEL_SYSCALL_ABI_COMPAT = 2,	/* Not implemented. */
};

enum lttng_kernel_syscall_match {
	LTTNG_KERNEL_SYSCALL_MATCH_NAME = 0,
	LTTNG_KERNEL_SYSCALL_MATCH_NR = 1,		/* Not implemented. */
};

struct lttng_kernel_syscall {
	uint8_t entryexit;	/* enum lttng_kernel_syscall_entryexit */
	uint8_t abi;		/* enum lttng_kernel_syscall_abi */
	uint8_t match;		/* enum lttng_kernel_syscall_match */
	uint8_t padding;
	uint32_t nr;		/* For LTTNG_SYSCALL_MATCH_NR */
} __attribute__((packed));

/*
 * For syscall tracing, name = "*" means "enable all".
 */
#define LTTNG_KERNEL_EVENT_PADDING1	8
#define LTTNG_KERNEL_EVENT_PADDING2	LTTNG_KERNEL_SYM_NAME_LEN + 32
struct lttng_kernel_event {
	char name[LTTNG_KERNEL_SYM_NAME_LEN];	/* event name */
	uint32_t instrumentation;		/* enum lttng_kernel_instrumentation */
	uint64_t token;				/* User-provided token */
	char padding[LTTNG_KERNEL_EVENT_PADDING1];

	/* Per instrumentation type configuration */
	union {
		struct lttng_kernel_kretprobe kretprobe;
		struct lttng_kernel_kprobe kprobe;
		struct lttng_kernel_function_tracer ftrace;
		struct lttng_kernel_uprobe uprobe;
		struct lttng_kernel_syscall syscall;
		char padding[LTTNG_KERNEL_EVENT_PADDING2];
	} u;
} __attribute__((packed));

#define LTTNG_KERNEL_EVENT_NOTIFIER_PADDING1	16
struct lttng_kernel_event_notifier {
	struct lttng_kernel_event event;

	char padding[LTTNG_KERNEL_EVENT_NOTIFIER_PADDING1];
} __attribute__((packed));

struct lttng_kernel_tracer_version {
	uint32_t major;
	uint32_t minor;
	uint32_t patchlevel;
} __attribute__((packed));

struct lttng_kernel_tracer_abi_version {
	uint32_t major;
	uint32_t minor;
} __attribute__((packed));

struct lttng_kernel_session_name {
	char name[LTTNG_KERNEL_SESSION_NAME_LEN];
} __attribute__((packed));

struct lttng_kernel_session_creation_time {
	char iso8601[LTTNG_KERNEL_SESSION_CREATION_TIME_ISO8601_LEN];
} __attribute__((packed));

enum lttng_kernel_calibrate_type {
	LTTNG_KERNEL_CALIBRATE_KRETPROBE,
};

struct lttng_kernel_calibrate {
	uint32_t type;	/* enum lttng_kernel_calibrate_type (input) */
} __attribute__((packed));

struct lttng_kernel_syscall_mask {
	uint32_t len;	/* in bits */
	char mask[];
} __attribute__((packed));

enum lttng_kernel_context_type {
	LTTNG_KERNEL_CONTEXT_PID		= 0,
	LTTNG_KERNEL_CONTEXT_PERF_COUNTER	= 1,
	LTTNG_KERNEL_CONTEXT_PROCNAME		= 2,
	LTTNG_KERNEL_CONTEXT_PRIO		= 3,
	LTTNG_KERNEL_CONTEXT_NICE		= 4,
	LTTNG_KERNEL_CONTEXT_VPID		= 5,
	LTTNG_KERNEL_CONTEXT_TID		= 6,
	LTTNG_KERNEL_CONTEXT_VTID		= 7,
	LTTNG_KERNEL_CONTEXT_PPID		= 8,
	LTTNG_KERNEL_CONTEXT_VPPID		= 9,
	LTTNG_KERNEL_CONTEXT_HOSTNAME		= 10,
	LTTNG_KERNEL_CONTEXT_CPU_ID		= 11,
	LTTNG_KERNEL_CONTEXT_INTERRUPTIBLE	= 12,
	LTTNG_KERNEL_CONTEXT_PREEMPTIBLE	= 13,
	LTTNG_KERNEL_CONTEXT_NEED_RESCHEDULE	= 14,
	LTTNG_KERNEL_CONTEXT_MIGRATABLE		= 15,
	LTTNG_KERNEL_CONTEXT_CALLSTACK_KERNEL	= 16,
	LTTNG_KERNEL_CONTEXT_CALLSTACK_USER	= 17,
	LTTNG_KERNEL_CONTEXT_CGROUP_NS		= 18,
	LTTNG_KERNEL_CONTEXT_IPC_NS		= 19,
	LTTNG_KERNEL_CONTEXT_MNT_NS		= 20,
	LTTNG_KERNEL_CONTEXT_NET_NS		= 21,
	LTTNG_KERNEL_CONTEXT_PID_NS		= 22,
	LTTNG_KERNEL_CONTEXT_USER_NS		= 23,
	LTTNG_KERNEL_CONTEXT_UTS_NS		= 24,
	LTTNG_KERNEL_CONTEXT_UID		= 25,
	LTTNG_KERNEL_CONTEXT_EUID		= 26,
	LTTNG_KERNEL_CONTEXT_SUID		= 27,
	LTTNG_KERNEL_CONTEXT_GID		= 28,
	LTTNG_KERNEL_CONTEXT_EGID		= 29,
	LTTNG_KERNEL_CONTEXT_SGID		= 30,
	LTTNG_KERNEL_CONTEXT_VUID		= 31,
	LTTNG_KERNEL_CONTEXT_VEUID		= 32,
	LTTNG_KERNEL_CONTEXT_VSUID		= 33,
	LTTNG_KERNEL_CONTEXT_VGID		= 34,
	LTTNG_KERNEL_CONTEXT_VEGID		= 35,
	LTTNG_KERNEL_CONTEXT_VSGID		= 36,
	LTTNG_KERNEL_CONTEXT_TIME_NS		= 37,
};

struct lttng_kernel_perf_counter_ctx {
	uint32_t type;
	uint64_t config;
	char name[LTTNG_KERNEL_SYM_NAME_LEN];
} __attribute__((packed));

#define LTTNG_KERNEL_CONTEXT_PADDING1	16
#define LTTNG_KERNEL_CONTEXT_PADDING2	LTTNG_KERNEL_SYM_NAME_LEN + 32
struct lttng_kernel_context {
	uint32_t ctx;	/*enum lttng_kernel_context_type */
	char padding[LTTNG_KERNEL_CONTEXT_PADDING1];

	union {
		struct lttng_kernel_perf_counter_ctx perf_counter;
		char padding[LTTNG_KERNEL_CONTEXT_PADDING2];
	} u;
} __attribute__((packed));

#define LTTNG_KERNEL_FILTER_BYTECODE_MAX_LEN		65536
struct lttng_kernel_filter_bytecode {
	uint32_t len;
	uint32_t reloc_offset;
	uint64_t seqnum;
	char data[0];
} __attribute__((packed));

enum lttng_kernel_tracker_type {
	LTTNG_KERNEL_TRACKER_UNKNOWN		= -1,

	LTTNG_KERNEL_TRACKER_PID		= 0,
	LTTNG_KERNEL_TRACKER_VPID		= 1,
	LTTNG_KERNEL_TRACKER_UID		= 2,
	LTTNG_KERNEL_TRACKER_VUID		= 3,
	LTTNG_KERNEL_TRACKER_GID		= 4,
	LTTNG_KERNEL_TRACKER_VGID		= 5,
};

struct lttng_kernel_tracker_args {
	uint32_t type;	/* enum lttng_kernel_tracker_type */
	int32_t id;
};

/* LTTng file descriptor ioctl */
/* lttng/abi-old.h reserve 0x40, 0x41, 0x42, 0x43, and 0x44. */
#define LTTNG_KERNEL_SESSION			_IO(0xF6, 0x45)
#define LTTNG_KERNEL_TRACER_VERSION		\
	_IOR(0xF6, 0x46, struct lttng_kernel_tracer_version)
#define LTTNG_KERNEL_TRACEPOINT_LIST		_IO(0xF6, 0x47)
#define LTTNG_KERNEL_WAIT_QUIESCENT		_IO(0xF6, 0x48)
#define LTTNG_KERNEL_CALIBRATE			\
	_IOWR(0xF6, 0x49, struct lttng_kernel_calibrate)
#define LTTNG_KERNEL_SYSCALL_LIST		_IO(0xF6, 0x4A)
#define LTTNG_KERNEL_TRACER_ABI_VERSION		\
	_IOR(0xF6, 0x4B, struct lttng_kernel_tracer_abi_version)
#define LTTNG_KERNEL_EVENT_NOTIFIER_GROUP_CREATE    _IO(0xF6, 0x4C)

/* Session FD ioctl */
/* lttng/abi-old.h reserve 0x50, 0x51, 0x52, and 0x53. */
#define LTTNG_KERNEL_METADATA			\
	_IOW(0xF6, 0x54, struct lttng_kernel_channel)
#define LTTNG_KERNEL_CHANNEL			\
	_IOW(0xF6, 0x55, struct lttng_kernel_channel)
#define LTTNG_KERNEL_SESSION_START		_IO(0xF6, 0x56)
#define LTTNG_KERNEL_SESSION_STOP		_IO(0xF6, 0x57)
#define LTTNG_KERNEL_SESSION_TRACK_PID		\
	_IOR(0xF6, 0x58, int32_t)
#define LTTNG_KERNEL_SESSION_UNTRACK_PID	\
	_IOR(0xF6, 0x59, int32_t)

/*
 * ioctl 0x58 and 0x59 are duplicated here. It works, since _IOR vs _IO
 * are generating two different ioctl numbers, but this was not done on
 * purpose. We should generally try to avoid those duplications.
 */
#define LTTNG_KERNEL_SESSION_LIST_TRACKER_PIDS	_IO(0xF6, 0x58)
#define LTTNG_KERNEL_SESSION_METADATA_REGEN	_IO(0xF6, 0x59)

/* lttng/abi-old.h reserve 0x5A and 0x5B. */
#define LTTNG_KERNEL_SESSION_STATEDUMP		_IO(0xF6, 0x5C)
#define LTTNG_KERNEL_SESSION_SET_NAME		\
	_IOR(0xF6, 0x5D, struct lttng_kernel_session_name)
#define LTTNG_KERNEL_SESSION_SET_CREATION_TIME		\
	_IOR(0xF6, 0x5E, struct lttng_kernel_session_creation_time)

/* Channel FD ioctl */
/* lttng/abi-old.h reserve 0x60 and 0x61. */
#define LTTNG_KERNEL_STREAM			_IO(0xF6, 0x62)
#define LTTNG_KERNEL_EVENT			\
	_IOW(0xF6, 0x63, struct lttng_kernel_event)
#define LTTNG_KERNEL_SYSCALL_MASK		\
	_IOWR(0xF6, 0x64, struct lttng_kernel_syscall_mask)

/* Event and Channel FD ioctl */
/* lttng/abi-old.h reserve 0x70. */
#define LTTNG_KERNEL_CONTEXT			\
	_IOW(0xF6, 0x71, struct lttng_kernel_context)

/* Event, Event notifier, Channel and Session ioctl */
/* lttng/abi-old.h reserve 0x80 and 0x81. */
#define LTTNG_KERNEL_ENABLE			_IO(0xF6, 0x82)
#define LTTNG_KERNEL_DISABLE			_IO(0xF6, 0x83)

/* Event and Event notifier FD ioctl */
#define LTTNG_KERNEL_FILTER			_IO(0xF6, 0x90)
#define LTTNG_KERNEL_ADD_CALLSITE		_IO(0xF6, 0x91)

/* Session FD ioctl (continued) */
#define LTTNG_KERNEL_SESSION_LIST_TRACKER_IDS	\
	_IOR(0xF6, 0xA0, struct lttng_kernel_tracker_args)
#define LTTNG_KERNEL_SESSION_TRACK_ID		\
	_IOR(0xF6, 0xA1, struct lttng_kernel_tracker_args)
#define LTTNG_KERNEL_SESSION_UNTRACK_ID		\
	_IOR(0xF6, 0xA2, struct lttng_kernel_tracker_args)

/* Event notifier group file descriptor ioctl */
#define LTTNG_KERNEL_EVENT_NOTIFIER_CREATE \
	_IOW(0xF6, 0xB0, struct lttng_kernel_event_notifier)
#define LTTNG_KERNEL_EVENT_NOTIFIER_GROUP_NOTIFICATION_FD \
	_IO(0xF6, 0xB1)

/*
 * LTTng-specific ioctls for the lib ringbuffer.
 *
 * Operations applying to the current sub-buffer need to occur between
 * a get/put or get_next/put_next ioctl pair.
 */

/* returns the timestamp begin of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_TIMESTAMP_BEGIN	_IOR(0xF6, 0x20, uint64_t)
/* returns the timestamp end of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_TIMESTAMP_END	_IOR(0xF6, 0x21, uint64_t)
/* returns the number of events discarded of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_EVENTS_DISCARDED	_IOR(0xF6, 0x22, uint64_t)
/* returns the packet payload size of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_CONTENT_SIZE	_IOR(0xF6, 0x23, uint64_t)
/* returns the packet size of the current sub-buffer*/
#define LTTNG_RING_BUFFER_GET_PACKET_SIZE	_IOR(0xF6, 0x24, uint64_t)
/* returns the stream id (invariant for the stream) */
#define LTTNG_RING_BUFFER_GET_STREAM_ID		_IOR(0xF6, 0x25, uint64_t)
/* returns the current timestamp as perceived from the tracer */
#define LTTNG_RING_BUFFER_GET_CURRENT_TIMESTAMP	_IOR(0xF6, 0x26, uint64_t)
/* returns the packet sequence number of the current sub-buffer */
#define LTTNG_RING_BUFFER_GET_SEQ_NUM		_IOR(0xF6, 0x27, uint64_t)
/* returns the stream instance id (invariant for the stream) */
#define LTTNG_RING_BUFFER_INSTANCE_ID		_IOR(0xF6, 0x28, uint64_t)

#ifdef CONFIG_COMPAT
/* returns the timestamp begin of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_TIMESTAMP_BEGIN \
	LTTNG_RING_BUFFER_GET_TIMESTAMP_BEGIN
/* returns the timestamp end of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_TIMESTAMP_END \
	LTTNG_RING_BUFFER_GET_TIMESTAMP_END
/* returns the number of events discarded of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_EVENTS_DISCARDED \
	LTTNG_RING_BUFFER_GET_EVENTS_DISCARDED
/* returns the packet payload size of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_CONTENT_SIZE \
	LTTNG_RING_BUFFER_GET_CONTENT_SIZE
/* returns the packet size of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_PACKET_SIZE \
	LTTNG_RING_BUFFER_GET_PACKET_SIZE
/* returns the stream id (invariant for the stream) */
#define LTTNG_RING_BUFFER_COMPAT_GET_STREAM_ID \
	LTTNG_RING_BUFFER_GET_STREAM_ID
/* returns the current timestamp as perceived from the tracer */
#define LTTNG_RING_BUFFER_COMPAT_GET_CURRENT_TIMESTAMP \
	LTTNG_RING_BUFFER_GET_CURRENT_TIMESTAMP
/* returns the packet sequence number of the current sub-buffer */
#define LTTNG_RING_BUFFER_COMPAT_GET_SEQ_NUM	\
	LTTNG_RING_BUFFER_GET_SEQ_NUM
/* returns the stream instance id (invariant for the stream) */
#define LTTNG_RING_BUFFER_COMPAT_INSTANCE_ID	\
	LTTNG_RING_BUFFER_INSTANCE_ID
#endif /* CONFIG_COMPAT */

#endif /* _LTTNG_ABI_H */
