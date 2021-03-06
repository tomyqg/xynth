/***************************************************************************
    begin                : Tue Dec 26 2006
    copyright            : (C) 2006 - 2009 by Alper Akcan
    email                : alper.akcan@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include "../../../lib/xynth_.h"
#include "server.h"
#include "caca.h"

#include <unistd.h>

int s_video_caca_kbd_init (s_server_conf_t *cfg)
{
	s_video_caca_data_t *priv = xynth_server->driver->driver_data;
	if (s_pipe_api_pipe(priv->keybd_fd)) {
		debugf(DSER | DSYS, "pipe failed");
		goto err0;
	}
	return priv->keybd_fd[0];
err0:   priv->keybd_fd[0] = -1;
	priv->keybd_fd[1] = -1;
	return -1;
}

int s_video_caca_kbd_update (s_video_input_data_t *keybd)
{
	s_video_input_data_t data;
	s_video_caca_data_t *priv = xynth_server->driver->driver_data;
	s_pipe_api_read(priv->keybd_fd[0], &data, sizeof(s_video_input_data_t));
	keybd->keybd.ascii = data.keybd.ascii;
	keybd->keybd.scancode = data.keybd.scancode;
	keybd->keybd.keycode = data.keybd.keycode;
	keybd->keybd.button = data.keybd.button;
	keybd->keybd.state = data.keybd.state;
	return 0;
}

void s_video_caca_kbd_uninit (void)
{
	s_video_caca_data_t *priv = xynth_server->driver->driver_data;
	s_pipe_api_close(priv->keybd_fd[0]);
	s_pipe_api_close(priv->keybd_fd[1]);
	priv->keybd_fd[0] = -1;
	priv->keybd_fd[1] = -1;
}
