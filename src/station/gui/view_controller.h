
#ifndef GUI_VIEW_CONTROLLER_H
#define GUI_VIEW_CONTROLLER_H

#include <vector>

class view;

class view_controller
{
public:
	view_controller()
		:pDesktopView(nullptr)
	{}

	void set_desktop_view(view * d_view)
	{
		pDesktopView = d_view;
	}
private:
	view * pDesktopView;
	std::vector<view*> pViewsStack;
};

#endif
