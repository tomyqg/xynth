/***************************************************************************
    begin                : Mon Jun 11 2007
    copyright            : (C) 2007 - 2014 by Alper Akcan
    email                : alper.akcan@gmail.com
 ***************************************************************************/

/*
  Copyright (c) <year>, <copyright holder>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. All advertising materials mentioning features or use of this software
     must display the following acknowledgement:
     This product includes software developed by the <organization>.
  4. Neither the name of the <organization> nor the
     names of its contributors may be used to endorse or promote products
     derived from this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "xynth_.h"

int s_region_create (s_region_t **region)
{
	s_region_t *reg;
	reg = (s_region_t *) malloc(sizeof(s_region_t));
	if (reg == NULL) {
		goto err0;
	}
	memset(reg, 0, sizeof(s_region_t));
	*region = reg;
	return 0;
err0:	*region = NULL;
	return -1;
}

int s_region_destroy (s_region_t *region)
{
	if (region->rects) {
		free(region->rects);
	}
	free(region);
	return 0;
}

int s_region_clear (s_region_t *region)
{
	region->nrects = 0;
	region->extents.x = 0;
	region->extents.y = 0;
	region->extents.w = 0;
	region->extents.h = 0;
	free(region->rects);
	region->rects = NULL;
	return 0;
}

int s_region_extents_calculate (s_region_t *region)
{
	int n;
	s_rect_t *e;
	s_rect_t *r;
	r = region->rects;
	n = region->nrects;
	e = &region->extents;
	if (n == 0) {
		e->x = 0;
		e->y = 0;
		e->w = 0;
		e->h = 0;
		return 0;
	}
	*e = *r;
	n--;
	r++;
	while (n--) {
		s_region_rect_union(e, r, e);
		r++;
	}
	return 0;
}

int s_region_intrect (s_region_t *region, s_rect_t *rect, s_region_t *result)
{
	int n;
	s_rect_t t;
	s_rect_t *r;
	r = s_region_rectangles(region);
	n = s_region_num_rectangles(region);
	while (n--) {
		if (s_region_rect_intersect(r, rect, &t) == 0) {
			if (s_region_addrect(result, &t)) {
				return -1;
			}
		}
		r++;
	}
	return 0;
}

int s_region_subrect (s_region_t *region, s_rect_t *rect)
{
	int n;
	s_rect_t r;
	s_rect_t t;
	for (n = 0; n < region->nrects; n++) {
		r = region->rects[n];
		if (s_region_rect_intersect(&r, rect, &t) == 0) {
			/* subrect calls unify region via add rect if necesary */
			if (s_region_rect_substract(&r, &t, region)) {
				return -1;
			}
			if (s_region_delrect(region, &r)) {
				return -1;
			}
			n--;
		}
	}
	return 0;
}

int s_region_delrect (s_region_t *region, s_rect_t *rect)
{
	int n;
	s_rect_t *r;
	for (n = 0; n < region->nrects;) {
		r = &region->rects[n];
		if (!(r->x - rect->x || r->y - rect->y ||
		      r->w - rect->w || r->h - rect->h)) {
			if (region->nrects == 1) {
				region->nrects = 0;
				free(region->rects);
				region->rects = NULL;
				break;
			}
			region->nrects -= 1;
			if (n < region->nrects) {
				memmove(region->rects + n, region->rects + n + 1, (region->nrects - n) * sizeof(s_rect_t));
			}
			region->rects = (s_rect_t *) realloc(region->rects, sizeof(s_rect_t) * region->nrects);
			s_region_extents_calculate(region);
			break;
		} else {
			n++;
		}
	}
	return 0;
}

int s_region_addrect (s_region_t *region, s_rect_t *rect)
{
	int n;
	s_rect_t *r;
	if (rect->w <= 0 || rect->h <= 0) {
		return 0;
	}
	r = region->rects;
	n = region->nrects;
	while (n--) {
		if (!(r->x - rect->x || r->y - rect->y ||
		      r->w - rect->w || r->h - rect->h)) {
			return 0;
		}
		r++;
	}
	region->rects = (s_rect_t *) realloc(region->rects, sizeof(s_rect_t) * (region->nrects + 1));
	if (region->rects != NULL) {
		region->rects[region->nrects] = *rect;
		region->nrects += 1;
	} else {
		return -1;
	}
	if (region->nrects == 1) {
		region->extents = *rect;
	} else {
		s_region_rect_union(&region->extents, rect, &region->extents);
	}
	return 0;
}

int s_region_intregion (s_region_t *region, s_region_t *intersect, s_region_t *result)
{
	int n;
	s_rect_t *r;
	r = s_region_rectangles(intersect);
	n = s_region_num_rectangles(intersect);
	while (n--) {
		if (s_region_intrect(region, r, result)) {
			return -1;
		}
		r++;
	}
	return 0;
}

int s_region_subregion (s_region_t *region, s_region_t *sub)
{
	int n;
	s_rect_t *r;
	r = s_region_rectangles(sub);
	n = s_region_num_rectangles(sub);
	while (n--) {
		if (s_region_subrect(region, r)) {
			return -1;
		}
		r++;
	}
	return 0;
}

int s_region_delregion (s_region_t *region, s_region_t *del)
{
	int n;
	s_rect_t *r;
	r = s_region_rectangles(del);
	n = s_region_num_rectangles(del);
	while (n--) {
		if (s_region_delrect(region, r)) {
			return -1;
		}
		r++;
	}
	return 0;
}

int s_region_addregion (s_region_t *region, s_region_t *add)
{
	int n;
	s_rect_t *r;
	r = s_region_rectangles(add);
	n = s_region_num_rectangles(add);
	while (n--) {
		if (s_region_addrect(region, r)) {
			return -1;
		}
		r++;
	}
	return 0;
}

int s_region_num_rectangles (s_region_t *region)
{
	return region->nrects;
}

s_rect_t * s_region_rectangles (s_region_t *region)
{
	return region->rects;
}

int s_region_extents (s_region_t *region, s_rect_t *extents)
{
	*extents = region->extents;
	return 0;
}

int s_region_copy (s_region_t **region, s_region_t *src)
{
	s_region_t *r;
	r = (s_region_t *) malloc(sizeof(s_region_t));
	if (r == NULL) {
		goto err0;
	}
	memcpy(r, src, sizeof(s_region_t));
	r->rects = (s_rect_t *) malloc(sizeof(s_rect_t) * r->nrects);
	if (r->rects == NULL) {
		goto err1;
	}
	memcpy(r->rects, src->rects, sizeof(s_rect_t) * r->nrects);
	*region = r;
	return 0;
err1:	free(r);
err0:	*region = NULL;
	return -1;
}

int s_region_unify (s_region_t *region)
{
	int n;
	s_rect_t *r;
	s_rect_t ext;
	s_region_t *rev;
	/* idea is simple
	 * unified = reverse(reverse(region))
	 */
	if (s_region_reverse(region, &rev)) {
		return -1;
	}
	if (s_region_extents(region, &ext)) {
		s_region_destroy(rev);
		return -1;
	}
	if (s_region_clear(region)) {
		s_region_destroy(rev);
		return -1;
	}
	if (s_region_addrect(region, &ext)) {
		s_region_destroy(rev);
		return -1;
	}
	r = s_region_rectangles(rev);
	n = s_region_num_rectangles(rev);
	while (n--) {
		if (s_region_subrect(region, r)) {
			s_region_destroy(rev);
			return -1;
		}
		r++;
	}
	s_region_destroy(rev);
	return 0;
}

int s_region_reverse (s_region_t *region, s_region_t **result)
{
	int n;
	s_rect_t *r;
	s_region_t *rg;
	if (s_region_create(&rg)) {
		*result = NULL;
		return -1;
	}
	if (s_region_addrect(rg, &region->extents)) {
		s_region_destroy(rg);
		return -1;
	}
	r = s_region_rectangles(region);
	n = s_region_num_rectangles(region);
	while (n--) {
		if (s_region_subrect(rg, r)) {
			s_region_destroy(rg);
			return -1;
		}
		r++;
	}
	*result = rg;
	return 0;
}

int s_region_combine (s_region_t *region)
{
	return 0;
}

int s_region_rect_union (s_rect_t *rect1, s_rect_t *rect2, s_rect_t *result)
{
	int x31;
	int x32;
	int y31;
	int y32;
	x31 = MIN(rect1->x, rect2->x);
	x32 = MAX(rect1->x + rect1->w, rect2->x + rect2->w);
	y31 = MIN(rect1->y, rect2->y);
	y32 = MAX(rect1->y + rect1->h, rect2->y + rect2->h);
	result->x = x31;
	result->y = y31;
	result->w = x32 - x31;
	result->h = y32 - y31;
	return 0;
}

int s_region_rect_intersect (s_rect_t *rect1, s_rect_t *rect2, s_rect_t *result)
{
	int x31;
	int x32;
	int y31;
	int y32;
	x31 = MAX(rect1->x, rect2->x);
	x32 = MIN(rect1->x + rect1->w - 1, rect2->x + rect2->w - 1);
	y31 = MAX(rect1->y, rect2->y);
	y32 = MIN(rect1->y + rect1->h - 1, rect2->y + rect2->h - 1);
	if ((x31 > x32) || (y31 > y32)) {
		return -1;
	}
	result->x = x31;
	result->y = y31;
	result->w = x32 - x31 + 1;
	result->h = y32 - y31 + 1;
	return 0;
}

int s_region_rect_substract (s_rect_t *rect1, s_rect_t *rect2, s_region_t *result)
{
	/* first one */
	int x0;
	int y0;
	int w0;
	int h0;
	/* second one */
	int x1;
	int y1;
	int w1;
	int h1;
        s_rect_t rtmp;
	if (s_region_rect_intersect(rect1, rect2, &rtmp) != 0) {
		/* xxxxxxxx
		   xxxxxxxx
		   xxxxxxxx  ........
		   xxxxxxxx  ........
		             ........
		             ........
		 */
		if (s_region_addrect(result, rect1)) {
			return -1;
		} else {
			return 0;
		}
	}
	x0 = rect1->x;
	y0 = rect1->y;
	w0 = rect1->w;
	h0 = rect1->h;
	x1 = rtmp.x;
	y1 = rtmp.y;
	w1 = rtmp.w;
	h1 = rtmp.h;
	/* xxxxxxxx
	   xx....xx
	   xx....xx
	   xxxxxxxx
	 */
	rtmp.x = x0;
	rtmp.y = y0;
	rtmp.w = x1 - x0;
	rtmp.h = h0;
	if (s_region_addrect(result, &rtmp)) {
		return -1;
	}
	rtmp.x = x1;
	rtmp.y = y0;
	rtmp.w = w1;
	rtmp.h = y1 - y0;
	if (s_region_addrect(result, &rtmp)) {
		return -1;
	}
	rtmp.x = x1 + w1;
	rtmp.y = y0;
	rtmp.w = (x0 + w0) - (x1 + w1);
	rtmp.h = h0;
	if (s_region_addrect(result, &rtmp)) {
		return -1;
	}
	rtmp.x = x1;
	rtmp.y = y1 + h1;
	rtmp.w = w1;
	rtmp.h = (y0 + h0) - (y1 + h1);
	if (s_region_addrect(result, &rtmp)) {
		return -1;
	}
	return 0;
}
