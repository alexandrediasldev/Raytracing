#include "render_struct.h"
struct render_struct *alloc_render_struct(
	render_mode_f renderer
, struct rgb_image *image, struct scene *scene,size_t startx, size_t starty,size_t maxx,size_t maxy)
{
	struct render_struct *rend = xalloc(sizeof(struct render_struct));
	rend->image = image;
	rend->renderer = renderer;
	rend->scene = scene;
	rend->maxx = maxx;
	rend->maxy = maxy;
	rend->startx = startx;
	rend->starty = starty;
	return rend;
}
