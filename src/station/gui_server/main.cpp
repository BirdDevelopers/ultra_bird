
#include <iostream>
#include "matrix_editor_view.h"
#include <gui/gles_renderer.h>
#include <gui/view_controller.h>

using namespace std;

int main(int argc, char ** argv)
{
	cout << "Starting GUI server..." << std::endl;

	gles_renderer renderer;
	renderer.init();

	view_controller main_vc;

	matrix_editor_view cur_view;
	main_vc.set_desktop_view(&cur_view);
	
	renderer.render(&main_vc);

	return 0;
}