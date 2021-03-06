/* $Id: cmd-kill-session.c,v 1.17 2009-12-04 22:14:47 tcunha Exp $ */

/*
 * Copyright (c) 2007 Nicholas Marriott <nicm@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

#include "tmux.h"

/*
 * Destroy session, detaching all clients attached to it and destroying any
 * windows linked only to this session.
 *
 * Note this deliberately has no alias to make it hard to hit by accident.
 */

int	cmd_kill_session_exec(struct cmd *, struct cmd_ctx *);

const struct cmd_entry cmd_kill_session_entry = {
	"kill-session", NULL,
	CMD_TARGET_SESSION_USAGE,
	0, "",
	cmd_target_init,
	cmd_target_parse,
	cmd_kill_session_exec,
	cmd_target_free,
	cmd_target_print
};

int
cmd_kill_session_exec(struct cmd *self, struct cmd_ctx *ctx)
{
	struct cmd_target_data	*data = self->data;
	struct session		*s;
	struct client		*c;
	u_int			 i;

	if ((s = cmd_find_session(ctx, data->target)) == NULL)
		return (-1);

	for (i = 0; i < ARRAY_LENGTH(&clients); i++) {
		c = ARRAY_ITEM(&clients, i);
		if (c != NULL && c->session == s) {
			c->session = NULL;
			server_write_client(c, MSG_EXIT, NULL, 0);
		}
	}
	recalculate_sizes();

	session_destroy(s);

	return (0);
}
