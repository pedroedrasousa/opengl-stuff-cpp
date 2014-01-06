
#include "Widget.h"

void Widget::AddCallBack(void (*func)(Widget *widget, void*), void *data)
{
	callback.func = func;
	callback.data = data;
}
