
#ifndef __FONT_H__
#define __FONT_H__

#include <windows.h>
#include <gl/gl.h>

#include <list>

#define FONT_HORIZ_ALIGN_LEFT	0
#define FONT_HORIZ_ALIGN_CENTER	1
#define FONT_HORIZ_ALIGN_RIGHT	2

#define FONT_VERT_ALIGN_TOP		0
#define FONT_VERT_ALIGN_CENTER	1
#define FONT_VERT_ALIGN_BOTTOM	2

class Font {
public:
	
	Font(const char *name);
	~Font();

	void Enable(int w,int h);
	void Disable();
    void SetAlignment(int h, int v);
	void Print(int x, int y, const char *str, ...);
	int GetVertInterval();
	int GetMaxWidth()	{ return maxWidth;	};
	int GetMaxHeight()	{ return maxHeight;	};

private:
	
	GLuint tex_id;
	GLuint list_id;

	int step;

	int width[256];	
	int height[256];

	int maxWidth;
	int maxHeight;

	int hOffset[256];
	int minVertOffset;
	
	int hInterval;
	int vInterval;

	int hAlign;
	int vAlign;

	std::list<int> m_sliders;
};

#endif /* __FONT_H__ */
