#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "../common/debugging.h"
#include "../common/renderable.h"
#include "../common/shaders.h"
#include "../common/simple_shapes.h"
#include "../common/matrix_stack.h"

/*
GLM library for math  https://github.com/g-truc/glm
it's a header-only library. You can just copy the folder glm into 3dparty
and set the path properly.
*/
#include <glm/glm.hpp>  
#include <glm/ext.hpp>  
#include <glm/gtx/string_cast.hpp>

int width = 1000;
int height = 800;

/* projection matrix*/
glm::mat4 proj;

/* view matrix and view_frame*/
glm::mat4 view, view_frame;

/* a bool variable that indicates if we are currently rotating the trackball*/
bool is_trackball_dragged;

/* p0 and p1 points on the sphere (just like in the slides) */
glm::vec3 p0, p1;

/* matrix to transform the scene according to the trackball */
glm::mat4 trackball_matrix;

float scaling_factor = 1.f;
glm::mat4  scaling_matrix;


/*
o: origin of the ray
d: direction of the ray
c: center of the sphere (in this setup is 0,0,0 )
radius: radius of the sphere
*/
bool ray_sphere_intersection(glm::vec3& int_point, glm::vec3 o, glm::vec3 d, glm::vec3 c, float radius) {
	glm::vec3 oc = o - c;
	float A = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
	float B = 2 * glm::dot(d, oc);
	float C = glm::dot(oc, oc) - radius * radius;

	float dis = B * B - 4 * A * C;

	if (dis > 0) {
		float t0 = (-B - sqrt(dis)) / (2 * A);
		float t1 = (-B + sqrt(dis)) / (2 * A);
		float t = std::min<float>(t0, t1);
		int_point = o + glm::vec3(t * d[0], t * d[1], t * d[2]);
		return true;
	}
	return false;
}

/* EXERCIZIO
*   la versione corrente di viewport_to_ray crea il raggio passante per il pixel pX,pY
*   e per farlo utilizza i dati di left (-1), right (1), bottom (-0.8),  top (0.8) e near
*   specificati dalla funzione glm::frustum
*   Ne segue che se cambiamo la proiezione questa funzione non calcola piú il raggio giusto.
*   Provate a fare la versione che prende la matrice di proiezione e la viewport come parametri:
*  	void viewport_to_ray(glm::mat4 proj, glm::vec4 viewport, double pX, double pY, glm::vec4  &orig, glm::vec4 &  dir) {
*/
void viewport_to_ray(double pX, double pY, glm::vec4& orig, glm::vec4& dir) {
	glm::vec2 pos2;
	pos2.x = (float)(-1.0 + (pX / width) * (1.0 - (-1.0)));
	pos2.y = (float)(-0.8 + ((height - pY) / height) * (0.8 - (-0.8)));

	// convert mouse position from the screen to  view space
	orig = glm::vec4(glm::vec3(0.f, 0.f, 0.f), 1.f);
	dir = glm::vec4(glm::vec3(pos2, -2.f), 0.f);
}

/* handles the intersection between the position under the mouse and the sphere.
*/
bool cursor_sphere_intersection(glm::vec3& int_point, double xpos, double ypos) {

	bool hit = false;
	glm::vec4 orig, dir;

	/*
	* build the ray passing through the pixel xpos,ypos
	*/
	viewport_to_ray(xpos, ypos, orig, dir);

	// convert the ray from view space to world space
	orig = view_frame * orig;
	dir = view_frame * dir;

	glm::vec3 c = glm::vec4(glm::vec3(0.f, 0.f, 0.f), 1.f);

	hit = ray_sphere_intersection(int_point, orig, dir, c, 2.f);
	if (hit)
		int_point -= c;

	// return true if there is intersection
	return hit;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec3 int_point;

		/*
		mouse button has been pressed, check if it's over the sphere
		*/
		if (cursor_sphere_intersection(int_point, xpos, ypos)) {
			//the first point has been found
			p0 = int_point;

			// set the state of trackball being dragged to true
			is_trackball_dragged = true;
		}
	}
	else
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			// mouse button has been released, the trackball is no more being  dragged
			is_trackball_dragged = false;
}

/* callback function called when the mouse is moving */
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// if the trackball is not being dragged then do nothing
	if (!is_trackball_dragged)
		return;

	// if it is dragged then p0 has been already found, 
	// check if the trackball is still under the  mouse pointer
	if (cursor_sphere_intersection(p1, xpos, ypos)) {
		/* here we have both p0 and p1, compute the rotation matrix
			corresponding to the movement between p0 and p1
			(check the slides)
		*/
		glm::vec3 rotation_vector = glm::cross(glm::normalize(p0), glm::normalize(p1));

		/* avoid near null rotation axis*/
		if ( glm::length(rotation_vector) > 0.01) {
			float alpha = glm::asin(glm::length(rotation_vector));
			glm::mat4 delta_rot = glm::rotate(glm::mat4(1.f), alpha, rotation_vector);
			trackball_matrix = delta_rot * trackball_matrix;

			/*p1 becomes the p0 value for the next movement */
			p0 = p1;
		}
	}
	else
		is_trackball_dragged = false;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scaling_factor *= (yoffset > 0) ? 1.1f : 0.97f;
	std::cout << scaling_factor << std::endl;
	scaling_matrix = glm::scale(glm::mat4(1.f), glm::vec3(scaling_factor, scaling_factor, scaling_factor));
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

void window_size_callback(GLFWwindow* window, int _width, int _height)
{
	width = _width;
	height = _height;
	glViewport(0, 0, width, height);
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1000, 800, "code_07_manipulation", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	/* declare the callback functions on mouse events */
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);


	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Load GL symbols *after* the context is current
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::fprintf(stderr, "Failed to initialize GLAD\n");
		glfwDestroyWindow(window);
		glfwTerminate();
		return EXIT_FAILURE;
	}

	printout_opengl_glsl_info();

	/* load the shaders */
	shader basic_shader;
	basic_shader.create_program("shaders/basic.vert", "shaders/basic.frag");

	/* Set the uT matrix to Identity */
	glUseProgram(basic_shader.program);
	//	glUniformMatrix4fv(basic_shader["uT"], 1, GL_FALSE, &glm::mat4(1.0)[0][0]);
	glUseProgram(0);

	check_gl_errors(__LINE__, __FILE__);

	/* create a  cube   centered at the origin with side 2*/
	renderable r_cube = shape_maker::cube(0.5f, 0.3f, 0.0f);

	/* create a  sphere   centered at the origin with radius 1*/
	renderable r_sphere = shape_maker::sphere(3);


	/* create 3 lines showing the reference frame*/
	renderable r_frame = shape_maker::frame(4.0);


	check_gl_errors(__LINE__, __FILE__);

	/* Transformation to setup the point of view on the scene */
	proj = glm::frustum(-1.f, 1.f, -0.8f, 0.8f, 2.f, 20.f);
	view = glm::lookAt(glm::vec3(0, 6, 8.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	view_frame = glm::inverse(view);

	glEnable(GL_DEPTH_TEST);

	matrix_stack stack;
	trackball_matrix = scaling_matrix = glm::mat4(1.f);


	/* define the viewport  */
	glViewport(0, 0, width, height);

	glUseProgram(basic_shader.program);
	glUniformMatrix4fv(basic_shader["uProj"], 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(basic_shader["uView"], 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE,glm::value_ptr(glm::mat4(1.f)));
	glUniform3f(basic_shader["uColor"], 1.0, 0.0, 0.0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1, 1, 1, 1);
		check_gl_errors(__LINE__, __FILE__);

		//	trackball_matrix = glm::rotate(glm::mat4(1.f), glm::radians(float(clock()) / 10.f), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(basic_shader["uTrackball"], 1, GL_FALSE, glm::value_ptr(scaling_matrix * trackball_matrix));


		r_cube.bind();

		// draw a frame (red = x, green = y, blue = z )
		stack.push();
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.05, 1.0, 0.05)));
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(0, 1.0, 0)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.0, 1.0, 0.0);
		glDrawElements(r_cube().mode, r_cube().count, r_cube().itype, 0);
		stack.pop();

		stack.push();
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(1, 0.05, 0.05)));
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(1.0, 0.0, 0)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 1.0, 0.0, 0.0);
		glDrawElements(r_cube().mode, r_cube().count, r_cube().itype, 0);
		stack.pop();

		stack.push();
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.05f, 0.05f, 1.f)));
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(0.0, 0.0, 1.f)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.0, 0.0, 1.0);
		glDrawElements(r_cube().mode, r_cube().count, r_cube().itype, 0);
		stack.pop();

		// draw one sphere
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.5, 0.5, 0.5);
		r_sphere.bind();
		glDrawElements(r_sphere().mode, r_sphere().count, r_sphere().itype, 0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glUseProgram(0);
	glfwTerminate();
	return 0;
}