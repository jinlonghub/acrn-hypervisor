/*
 * Copyright (C) 2018 Intel Corporation
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Copyright (C) 2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "fsutils.h"
#include "load_conf.h"
#include "history.h"
#include "log_sys.h"
#include "probeutils.h"
#include "strutils.h"

#define HISTORY_FIRST_LINE_FMT \
		"#V1.0 CURRENTUPTIME   %-24s\n"
#define HISTORY_BLANK_LINE2 \
		"#EVENT  ID                    DATE                 TYPE\n"

struct history_entry {
	const char *event;
	const char *type;
	const char *log;
	const char *lastuptime; /* for uptime */
	const char *key;
	const char *eventtime;
};

char *history_file;
static int current_lines;

static void entry_to_history_line(struct history_entry *entry,
				char newline[MAXLINESIZE])
{
	newline[0] = 0;
	if (entry->log != NULL) {
		char *ptr;
		char tmp[PATH_MAX];

		strncpy(tmp, entry->log, PATH_MAX);
		tmp[PATH_MAX - 1] = 0;
		ptr = strrchr(tmp, '/');
		if (ptr && ptr[1] == 0)
			ptr[0] = 0;
		snprintf(newline, MAXLINESIZE, "%-8s%-22s%-20s%s %s\n",
			 entry->event, entry->key, entry->eventtime,
			 entry->type, tmp);
	} else if (entry->type != NULL && entry->type[0]) {
		if (entry->lastuptime != NULL) {
			snprintf(newline, MAXLINESIZE,
				 "%-8s%-22s%-20s%-16s %s\n",
				 entry->event, entry->key,
				 entry->eventtime, entry->type,
				 entry->lastuptime);
		} else {
			snprintf(newline, MAXLINESIZE,
				 "%-8s%-22s%-20s%-16s\n",
				 entry->event, entry->key, entry->eventtime,
				 entry->type);
		}
	} else {
		snprintf(newline, MAXLINESIZE, "%-8s%-22s%-20s%s\n",
			 entry->event, entry->key, entry->eventtime,
			 entry->lastuptime);
	}
}

static void backup_history(void)
{
	int ret;
	char *des;

	ret = asprintf(&des, "%s.bak", history_file);
	if (ret < 0) {
		LOGE("compute string failed, out of memory\n");
		return;
	}

	ret = do_mv(history_file, des);
	if (ret < 0) {
		LOGE("backup %s failed, error (%s)\n", history_file,
		     strerror(errno));
		goto free;
	}

	ret = prepare_history();
	if (ret < 0) {
		LOGE("Prepare new history_file failed, exit\n");
		exit(EXIT_FAILURE);
	}

free:
	free(des);
}

void hist_raise_event(const char *event, const char *type, const char *log,
			const char *lastuptime, const char *key)
{
	char line[MAXLINESIZE];
	char eventtime[LONG_TIME_SIZE];
	struct sender_t *crashlog;
	int maxlines;
	struct history_entry entry = {
		.event = event,
		.type = type,
		.log = log,
		.lastuptime = lastuptime,
		.key = key
	};

	/* here means user have configured the crashlog sender */
	crashlog = get_sender_by_name("crashlog");
	if (!crashlog)
		return;

	maxlines = atoi(crashlog->maxlines);
	if (++current_lines >= maxlines) {
		LOGW("lines of (%s) meet quota %d, backup... Pls clean!\n",
		     history_file, maxlines);
		backup_history();
	}

	if (get_current_time_long(eventtime) <= 0)
		return;

	entry.eventtime = eventtime;
	entry_to_history_line(&entry, line);
	if (append_file(history_file, line, strnlen(line, MAXLINESIZE)) <= 0) {
		LOGE("failed to append (%s) to (%s)\n", line, history_file);
		return;
	}
}

void hist_raise_uptime(char *lastuptime)
{
	char boot_time[UPTIME_SIZE];
	char firstline[MAXLINESIZE];
	int hours;
	int ret;
	char *key;
	static int loop_uptime_event = 1;
	struct sender_t *crashlog;
	struct uptime_t *uptime;
	static int uptime_hours;

	/* user have configured the crashlog sender */
	crashlog = get_sender_by_name("crashlog");
	if (!crashlog)
		return;

	uptime = crashlog->uptime;
	uptime_hours = atoi(uptime->eventhours);

	if (lastuptime)
		hist_raise_event(uptime->name, NULL, NULL, lastuptime,
			    "00000000000000000000");
	else {
		ret = get_uptime_string(boot_time, &hours);
		if (ret < 0) {
			LOGE("cannot get uptime - %s\n", strerror(-ret));
			return;
		}

		ret = snprintf(firstline, sizeof(firstline),
			       HISTORY_FIRST_LINE_FMT, boot_time);
		if (s_not_expect(ret, sizeof(firstline))) {
			LOGE("failed to construct the firstline\n");
			return;
		}
		replace_file_head(history_file, firstline);

		if (hours / uptime_hours >= loop_uptime_event) {
			loop_uptime_event = (hours / uptime_hours) + 1;

			key = generate_event_id((const char *)uptime->name,
						uptime->name_len,
						NULL, 0, KEY_SHORT);
			if (key == NULL) {
				LOGE("generate event id failed, error (%s)\n",
				     strerror(errno));
				return;
			}

			hist_raise_event(uptime->name, NULL, NULL,
					 boot_time, key);
			free(key);
		}
	}
}

void hist_raise_infoerror(const char *type, size_t tlen)
{
	char *key;

	key = generate_event_id("ERROR", 5, type, tlen, KEY_SHORT);
	if (key == NULL) {
		LOGE("generate event id failed, error (%s)\n",
		     strerror(errno));
		return;
	}

	hist_raise_event("ERROR", type, NULL, NULL, key);
	free(key);
}

static int get_time_from_firstline(char *buffer, size_t size)
{
	char lasttime[MAXLINESIZE];
	const char *prefix = "#V1.0 CURRENTUPTIME   ";
	int len;

	len = file_read_key_value(lasttime, MAXLINESIZE, history_file, prefix,
				  strlen(prefix));
	if (len <= 0) {
		LOGW("failed to read value from %s, error %s\n",
		      history_file, strerror(-len));
		return -1;
	}
	if ((size_t)len >= size)
		return -1;

	*(char *)mempcpy(buffer, lasttime, len) = '\0';

	return 0;
}

int prepare_history(void)
{
	int ret;
	int llen;
	struct sender_t *crashlog;
	char linebuf[MAXLINESIZE];

	crashlog = get_sender_by_name("crashlog");
	if (!crashlog)
		return 0;

	if (!history_file) {
		ret = asprintf(&history_file, "%s/%s", crashlog->outdir,
			       HISTORY_NAME);
		if (ret < 0) {
			LOGE("compute string failed, out of memory\n");
			return -ENOMEM;
		}
	}

	ret = get_time_from_firstline(linebuf, MAXLINESIZE);
	if (ret == 0) {
		current_lines = count_lines_in_file(history_file);
		hist_raise_uptime(linebuf);
	} else {
		/* new history */
		LOGW("new history\n");
		llen = snprintf(linebuf, sizeof(linebuf),
				HISTORY_FIRST_LINE_FMT, "0000:00:00");
		if (s_not_expect(llen, sizeof(linebuf))) {
			LOGE("failed to construct the fristline\n");
			return -EINVAL;
		}
		ret = overwrite_file(history_file, linebuf);
		if (ret < 0) {
			LOGE("Write (%s, %s) failed, error (%s)\n",
			     history_file, linebuf,
			     strerror(errno));
			return ret;
		}
		ret = append_file(history_file, HISTORY_BLANK_LINE2,
				  sizeof(HISTORY_BLANK_LINE2) - 1);
		if (ret < 0) {
			LOGE("Write (%s, %s) failed, error (%s)\n",
			     history_file, HISTORY_BLANK_LINE2,
			     strerror(-ret));
			return ret;
		}
		current_lines = count_lines_in_file(history_file);
	}

	return 0;
}
