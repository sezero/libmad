/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: decoder.c,v 1.2 2000/03/19 06:43:38 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <errno.h>

# include "stream.h"
# include "frame.h"
# include "synth.h"
# include "decoder.h"

void mad_decoder_init(struct mad_decoder *decoder)
{
  decoder->mode        = -1;

  decoder->input_func  = 0;
  decoder->filter_func = 0;
  decoder->output_func = 0;
  decoder->error_func  = 0;

  decoder->input_data  = 0;
  decoder->filter_data = 0;
  decoder->output_data = 0;
  decoder->error_data  = 0;
}

int mad_decoder_finish(struct mad_decoder *decoder)
{
  if (decoder->mode == MAD_DECODER_ASYNC) {
    pid_t pid;
    int status;

    do {
      pid = waitpid(decoder->async.pid, &status, 0);
    }
    while (pid == -1 && errno == EINTR);

    decoder->mode = -1;

    if (pid == -1)
      return -1;

    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
  }

  return 0;
}

void mad_decoder_input(struct mad_decoder *decoder,
		       int (*func)(void *, struct mad_stream *), void *data)
{
  decoder->input_func = func;
  decoder->input_data = data;
}

void mad_decoder_filter(struct mad_decoder *decoder,
			int (*func)(void *, struct mad_frame *), void *data)
{
  decoder->filter_func = func;
  decoder->filter_data = data;
}

void mad_decoder_output(struct mad_decoder *decoder,
			int (*func)(void *, struct mad_frame const *,
				    struct mad_synth const *), void *data)
{
  decoder->output_func = func;
  decoder->output_data = data;
}

void mad_decoder_error(struct mad_decoder *decoder,
		       int (*func)(void *, struct mad_stream *,
				   struct mad_frame *),
		       void *data)
{
  decoder->error_func = func;
  decoder->error_data = data;
}

static
int error_default(void *data, struct mad_stream *stream,
		  struct mad_frame *frame)
{
  int bad_last_frame;

  switch (stream->error) {
  case MAD_ERR_BADCRC:
    bad_last_frame = *(int *) data;

    if (bad_last_frame)
      mad_frame_mute(frame);
    /* else keep previous frame's subband samples */

    return MAD_DECODER_IGNORE;

  default:
    return MAD_DECODER_CONTINUE;
  }
}

static
int send(struct mad_decoder const *decoder, union mad_control const *control)
{
  char *ptr;
  int len, count;

  for (ptr = (char *) control, count = sizeof(*control);
       count; count -= len, ptr += len) {
    do {
      len = write(decoder->async.out, ptr, count);
    }
    while (len == -1 && errno == EINTR);

    if (len == -1)
      return MAD_DECODER_BREAK;
  }

  return MAD_DECODER_CONTINUE;
}

static
int receive(struct mad_decoder *decoder, union mad_control *control)
{
  char *ptr;
  int len, count;

  for (ptr = (char *) control, count = sizeof(*control);
       count; count -= len, ptr += len) {
    do {
      len = read(decoder->async.in, ptr, count);
    }
    while (len == -1 && errno == EINTR);

    if (len == -1) {
      if (errno == EAGAIN)
	return MAD_DECODER_IGNORE;
      else
	return MAD_DECODER_BREAK;
    }
    else if (len == 0)
      return MAD_DECODER_STOP;
  }

  return MAD_DECODER_CONTINUE;
}

static
int run_sync(struct mad_decoder *decoder)
{
  int (*error_func)(void *, struct mad_stream *, struct mad_frame *);
  void *error_data;
  int bad_last_frame = 0;
  struct mad_stream *stream;
  struct mad_frame *frame;
  struct mad_synth *synth;
  int result = 0;

  if (decoder->input_func == 0)
    return 0;

  if (decoder->error_func) {
    error_func = decoder->error_func;
    error_data = decoder->error_data;
  }
  else {
    error_func = error_default;
    error_data = &bad_last_frame;
  }

  stream = &decoder->sync->stream;
  frame  = &decoder->sync->frame;
  synth  = &decoder->sync->synth;

  mad_stream_init(stream);
  mad_frame_init(frame);
  mad_synth_init(synth);

  while (1) {
    switch (decoder->input_func(decoder->input_data, stream)) {
    case MAD_DECODER_STOP:
      goto done;
    case MAD_DECODER_BREAK:
      goto fail;
    case MAD_DECODER_IGNORE:
      continue;
    case MAD_DECODER_CONTINUE:
    default:
      break;
    }

    while (1) {
      if (decoder->mode == MAD_DECODER_ASYNC) {
	union mad_control control;

	switch (receive(decoder, &control)) {
	case MAD_DECODER_BREAK:
	  goto fail;

	case MAD_DECODER_STOP:
	  goto done;

	case MAD_DECODER_CONTINUE:
	  if (send(decoder, &control) == MAD_DECODER_BREAK)
	    goto fail;
	  break;

	case MAD_DECODER_IGNORE:
	default:
	  break;
	}
      }

      if (mad_frame_decode(frame, stream) == -1) {
	if (!MAD_RECOVERABLE(stream->error))
	  break;

	switch (error_func(error_data, stream, frame)) {
	case MAD_DECODER_STOP:
	  goto done;
	case MAD_DECODER_BREAK:
	  goto fail;
	case MAD_DECODER_IGNORE:
	  break;
	case MAD_DECODER_CONTINUE:
	default:
	  continue;
	}
      }

      bad_last_frame = (frame->flags & MAD_FLAG_CRCFAILED);

      if (decoder->filter_func) {
	switch (decoder->filter_func(decoder->filter_data, frame)) {
	case MAD_DECODER_STOP:
	  goto done;
	case MAD_DECODER_BREAK:
	  goto fail;
	case MAD_DECODER_IGNORE:
	  continue;
	case MAD_DECODER_CONTINUE:
	default:
	  break;
	}
      }

      mad_synth_frame(synth, frame);

      if (decoder->output_func) {
	switch (decoder->output_func(decoder->output_data, frame, synth)) {
	case MAD_DECODER_STOP:
	  goto done;
	case MAD_DECODER_BREAK:
	  goto fail;
	case MAD_DECODER_IGNORE:
	case MAD_DECODER_CONTINUE:
	default:
	  break;
	}
      }
    }

    if (stream->error != MAD_ERR_BUFLEN)
      goto fail;
  }

 fail:
  result = -1;

 done:
  mad_synth_finish(synth);
  mad_frame_finish(frame);
  mad_stream_finish(stream);

  return result;
}

static
int run_async(struct mad_decoder *decoder)
{
  pid_t pid;
  int ptoc[2], ctop[2], flags;

  if (pipe(ptoc) == -1)
    return -1;

  if (pipe(ctop) == -1) {
    close(ptoc[0]);
    close(ptoc[1]);
    return -1;
  }

  flags = fcntl(ptoc[0], F_GETFL);
  if (flags == -1 ||
      fcntl(ptoc[0], F_SETFL, flags | O_NONBLOCK) == -1) {
    close(ctop[0]);
    close(ctop[1]);
    close(ptoc[0]);
    close(ptoc[1]);
    return -1;
  }

  pid = fork();
  if (pid == -1) {
    close(ctop[0]);
    close(ctop[1]);
    close(ptoc[0]);
    close(ptoc[1]);
    return -1;
  }

  decoder->async.pid = pid;

  if (pid) {
    /* parent */

    close(ptoc[0]);
    close(ctop[1]);

    decoder->async.in  = ctop[0];
    decoder->async.out = ptoc[1];

    return 0;
  }

  /* child */

  close(ptoc[1]);
  close(ctop[0]);

  decoder->async.in  = ptoc[0];
  decoder->async.out = ctop[1];

  _exit(run_sync(decoder));

  /* not reached */
  return -1;
}

int mad_decoder_run(struct mad_decoder *decoder, int flags)
{
  int result;
  int (*run)(struct mad_decoder *);

  decoder->sync = malloc(sizeof(*decoder->sync));
  if (decoder->sync == 0)
    return -1;

  if (flags & MAD_DECODER_ASYNC) {
    decoder->mode = MAD_DECODER_ASYNC;
    run = run_async;
  }
  else {
    decoder->mode = MAD_DECODER_SYNC;
    run = run_sync;
  }

  result = run(decoder);

  free(decoder->sync);
  decoder->sync = 0;

  return result;
}

int mad_decoder_command(struct mad_decoder *decoder,
			union mad_control *control)
{
  if (decoder->mode != MAD_DECODER_ASYNC)
    return MAD_DECODER_BREAK;

  if (send(decoder, control) == MAD_DECODER_BREAK)
    return MAD_DECODER_BREAK;

  return receive(decoder, control);
}
