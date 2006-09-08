
#include "xynth_.h"

void s_frame_content (s_frame_t *frame)
{
	s_object_t *object;
	object = frame->object;
	switch (frame->style & FRAME_MSHAPE) {
		case FRAME_NOFRAME:
		case FRAME_MENUBARPANEL:
		case FRAME_TOOLBARPANEL:
			frame->linewidth = 1;
			frame->midlinewidth = 0;
			break;
		case FRAME_PANEL:
		case FRAME_STYLEDPANEL:
			frame->midlinewidth = 0;
			break;
		case FRAME_WINPANEL:
		case FRAME_POPUPPANEL:
		case FRAME_LINEEDITPANEL:
		case FRAME_TABWIDGETPANEL:
			frame->linewidth = 2;
			frame->midlinewidth = 0;
			break;
		case FRAME_BOX:
		case FRAME_HLINE:
		case FRAME_VLINE:
		case FRAME_GROUPBOXPANEL:
			if ((frame->style & FRAME_MSHADOW) == FRAME_PLAIN) {
				frame->midlinewidth = 0;
			}
			break;
	}

	switch (frame->style & FRAME_MSHAPE) {
		case FRAME_NOFRAME:
			s_object_set_content(object, object->surface->buf->x,
			                      object->surface->buf->y,
			                      object->surface->buf->w,
			                      object->surface->buf->h);
			return;
		case FRAME_BOX:
		case FRAME_HLINE:
		case FRAME_VLINE:
		case FRAME_GROUPBOXPANEL:
			switch (frame->style & FRAME_MSHADOW) {
				case FRAME_PLAIN:
sbox_plain:				s_object_set_content(object, object->surface->buf->x + frame->linewidth,
                                                              object->surface->buf->y + frame->linewidth,
                                                              object->surface->buf->w - (frame->linewidth * 2),
                                                              object->surface->buf->h - (frame->linewidth * 2));
					break;
				case FRAME_RAISED:
				case FRAME_SUNKEN:
					s_object_set_content(object, object->surface->buf->x + (frame->linewidth * 2) + (frame->midlinewidth),
					                      object->surface->buf->y + (frame->linewidth * 2) + (frame->midlinewidth),
					                      object->surface->buf->w - (frame->linewidth * 4) - (frame->midlinewidth * 2),
					                      object->surface->buf->h - (frame->linewidth * 4) - (frame->midlinewidth * 2));
					break;
			}
			break;
		case FRAME_PANEL:
		case FRAME_WINPANEL:
		case FRAME_POPUPPANEL:
		case FRAME_STYLEDPANEL:
		case FRAME_MENUBARPANEL:
		case FRAME_TOOLBARPANEL:
		case FRAME_LINEEDITPANEL:
		case FRAME_TABWIDGETPANEL:
			goto sbox_plain;
			break;
	}
}

void s_frame_draw (s_object_t *object)
{
        int i;
        int j;
        int c[4];
        s_frame_t *frame;
        frame = (s_frame_t *) object->data;
        
	s_frame_content(frame);

        switch (frame->style & FRAME_MSHAPE) {
		case FRAME_NOFRAME:		return;
		case FRAME_BOX:
		case FRAME_GROUPBOXPANEL:
			switch (frame->style & FRAME_MSHADOW) {
				case FRAME_PLAIN:
sbox_plain:				s_fillbox(object->surface, 0, 0, object->surface->buf->w, object->surface->buf->h, s_rgbcolor(object->surface, 0, 0, 0));
					break;
				case FRAME_RAISED:
					c[0] = s_rgbcolor(object->surface, 96, 96, 96);
					c[1] = s_rgbcolor(object->surface, 128, 128, 128);
					c[2] = s_rgbcolor(object->surface, 255, 255, 255);
sbox_raised:				for (i = 0; i < frame->linewidth; i++) {
						s_fillbox(object->surface, i, i, object->surface->buf->w - (i * 2), object->surface->buf->h - (i * 2), c[2]);
						s_fillbox(object->surface, i + 1, i + 1, object->surface->buf->w - (i * 2) - 1, object->surface->buf->h - (i * 2) - 1, c[0]);
					}
					s_fillbox(object->surface, i, i, object->surface->buf->w - (i * 2), object->surface->buf->h - (i * 2), c[1]);
					for (j = 0; j < frame->linewidth; j++) {
						s_fillbox(object->surface, j + i + frame->midlinewidth, j + i + frame->midlinewidth, object->surface->buf->w - ((j + i + frame->midlinewidth) * 2), object->surface->buf->h - ((j + i + frame->midlinewidth) * 2), c[0]);
						s_fillbox(object->surface, j + 1 + i + frame->midlinewidth, j + 1 + i + frame->midlinewidth, object->surface->buf->w - ((j + i + frame->midlinewidth) * 2) - 1, object->surface->buf->h - ((j + i + frame->midlinewidth) * 2) - 1, c[2]);
					}
					break;
				case FRAME_SUNKEN:
					c[2] = s_rgbcolor(object->surface, 96, 96, 96);
					c[1] = s_rgbcolor(object->surface, 128, 128, 128);
					c[0] = s_rgbcolor(object->surface, 255, 255, 255);
					goto sbox_raised;
					break;
			}
			break;
		case FRAME_PANEL:
		case FRAME_STYLEDPANEL:
			switch (frame->style & FRAME_MSHADOW) {
				case FRAME_PLAIN:
					goto sbox_plain;
					break;
				case FRAME_RAISED:
					c[0] = s_rgbcolor(object->surface, 96, 96, 96);
					c[1] = s_rgbcolor(object->surface, 255, 255, 255);
spanel_raised:				for (i = 0; i < frame->linewidth; i++) {
						s_fillbox(object->surface, i, i, object->surface->buf->w - (i * 2), object->surface->buf->h - (i * 2), c[0]);
						s_fillbox(object->surface, i, i, object->surface->buf->w - (i * 2) - 1, object->surface->buf->h - (i * 2) - 1, c[1]);
					}
					break;
				case FRAME_SUNKEN:
					c[1] = s_rgbcolor(object->surface, 96, 96, 96);
					c[0] = s_rgbcolor(object->surface, 255, 255, 255);
					goto spanel_raised;
					break;
			}
			break;
		case FRAME_WINPANEL:
		case FRAME_POPUPPANEL:
			switch (frame->style & FRAME_MSHADOW) {
				case FRAME_PLAIN:
					goto sbox_plain;
					break;
				case FRAME_RAISED:
swinpanel_raised:			c[0] = s_rgbcolor(object->surface, 0, 0, 0);
					c[1] = s_rgbcolor(object->surface, 96, 96, 96);
					c[2] = s_rgbcolor(object->surface, 220, 220, 220);
					c[3] = s_rgbcolor(object->surface, 255, 255, 255);
					s_fillbox(object->surface, 0, 0, object->surface->buf->w, object->surface->buf->h, c[0]);
					s_fillbox(object->surface, 0, 0, object->surface->buf->w - 1, object->surface->buf->h - 1, c[2]);
					s_fillbox(object->surface, 1, 1, object->surface->buf->w - 2, object->surface->buf->h - 2, c[1]);
					s_fillbox(object->surface, 1, 1, object->surface->buf->w - 3, object->surface->buf->h - 3, c[3]);
					break;
swinpanel_sunken:		case FRAME_SUNKEN:
					c[0] = s_rgbcolor(object->surface, 0, 0, 0);
					c[1] = s_rgbcolor(object->surface, 96, 96, 96);
					c[2] = s_rgbcolor(object->surface, 220, 220, 220);
					s_fillbox(object->surface, 0, 0, object->surface->buf->w, object->surface->buf->h, c[1]);
					s_fillbox(object->surface, 1, 1, object->surface->buf->w - 1, object->surface->buf->h - 1, c[2]);
					s_fillbox(object->surface, 1, 1, object->surface->buf->w - 2, object->surface->buf->h - 2, c[0]);
					s_fillbox(object->surface, 2, 2, object->surface->buf->w - 3, object->surface->buf->h - 3, c[2]);
					break;
			}
			break;
		case FRAME_MENUBARPANEL:
			s_fillbox(object->surface, 0, 0, object->surface->buf->w, object->surface->buf->h, s_rgbcolor(object->surface, 220, 220, 220));
			break;
		case FRAME_TOOLBARPANEL:
			c[0] = s_rgbcolor(object->surface, 96, 96, 96);
			c[1] = s_rgbcolor(object->surface, 255, 255, 255);
			s_fillbox(object->surface, 0, 0, object->surface->buf->w, object->surface->buf->h, c[0]);
			s_fillbox(object->surface, 0, 0, object->surface->buf->w - 1, object->surface->buf->h - 1, c[1]);
			break;
		case FRAME_LINEEDITPANEL:
		case FRAME_TABWIDGETPANEL:
			switch (frame->style & FRAME_MSHADOW) {
				case FRAME_PLAIN:
				case FRAME_RAISED:
					goto swinpanel_raised;
					break;
				case FRAME_SUNKEN:
					goto swinpanel_sunken;
					break;
			}
			break;
		case FRAME_HLINE:
		case FRAME_VLINE:
			break;
	}

	s_fillbox(object->surface, object->content->x - object->surface->buf->x,
	              object->content->y - object->surface->buf->y,
	              object->content->w,
	              object->content->h,
	              s_rgbcolor(object->surface, 220, 220, 220));
}

int s_frame_init (s_window_t *window, s_frame_t **frame, int w, int h, unsigned int style, s_object_t *parent)
{
	(*frame) = (s_frame_t *) s_malloc(sizeof(s_frame_t));
	(*frame)->style = style;
	(*frame)->linewidth = 1;
	(*frame)->midlinewidth = 0;
	if (s_object_init(window, &((*frame)->object), w, h, s_frame_draw, parent)) {
		goto err0;
	}
	(*frame)->object->data = *frame;
	return 0;
err0:	s_free(*frame);
	return -1;
}

int s_frame_uninit (s_frame_t *frame)
{
	s_object_uninit(frame->object);
	s_free(frame);
	return 0;
}