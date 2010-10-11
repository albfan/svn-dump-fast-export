/*
 * Licensed under a two-clause BSD-style license.
 * See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "line_buffer.h"
#include "strbuf.h"

#define COPY_BUFFER_LEN 4096
static struct line_buffer buf_ = LINE_BUFFER_INIT;
static struct line_buffer *buf;

int buffer_init(const char *filename)
{
	buf = &buf_;

	buf->infile = filename ? fopen(filename, "r") : stdin;
	if (!buf->infile)
		return -1;
	return 0;
}

int buffer_deinit()
{
	int err;
	if (buf->infile == stdin)
		return ferror(buf->infile);
	err = ferror(buf->infile);
	err |= fclose(buf->infile);
	return err;
}

/* Read a line without trailing newline. */
char *buffer_read_line(void)
{
	char *end;
	if (!fgets(buf->line_buffer, sizeof(buf->line_buffer), buf->infile))
		/* Error or data exhausted. */
		return NULL;
	end = buf->line_buffer + strlen(buf->line_buffer);
	if (end[-1] == '\n')
		end[-1] = '\0';
	else if (feof(buf->infile))
		; /* No newline at end of file.  That's fine. */
	else
		/*
		 * Line was too long.
		 * There is probably a saner way to deal with this,
		 * but for now let's return an error.
		 */
		return NULL;
	return buf->line_buffer;
}

char *buffer_read_string(uint32_t len)
{
	strbuf_reset(&buf->blob_buffer);
	strbuf_fread(&buf->blob_buffer, len, buf->infile);
	return ferror(buf->infile) ? NULL : buf->blob_buffer.buf;
}

void buffer_copy_bytes(uint32_t len)
{
	char byte_buffer[COPY_BUFFER_LEN];
	uint32_t in;
	while (len > 0 && !feof(buf->infile) && !ferror(buf->infile)) {
		in = len < COPY_BUFFER_LEN ? len : COPY_BUFFER_LEN;
		in = fread(byte_buffer, 1, in, buf->infile);
		len -= in;
		fwrite(byte_buffer, 1, in, stdout);
		if (ferror(infile) || ferror(stdout))
			/* NEEDSWORK: handle error. */
			break;
	}
}

void buffer_skip_bytes(uint32_t len)
{
	char byte_buffer[COPY_BUFFER_LEN];
	uint32_t in;
	while (len > 0 && !feof(buf->infile) && !ferror(buf->infile)) {
		in = len < COPY_BUFFER_LEN ? len : COPY_BUFFER_LEN;
		in = fread(byte_buffer, 1, in, buf->infile);
		len -= in;
	}
}

void buffer_reset(void)
{
	strbuf_release(&buf->blob_buffer);
}
