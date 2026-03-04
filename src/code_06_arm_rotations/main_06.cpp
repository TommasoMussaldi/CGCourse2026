#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
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


int main(void)
{

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1000, 400, "code_06_arm_rotation", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

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

	shader basic_shader;
	basic_shader.create_program("shaders/basic.vert", "shaders/basic.frag");


	/* create a  cube   centered at the origin with side 2*/
	renderable r_cube = shape_maker::cube(0.5, 0.3, 0.0);

	/* create a  sphere   centered at the origin with side 2*/
	renderable r_sphere = shape_maker::sphere(2);

	/* create a  cylinder with base on the XZ plane, and height=2*/
	renderable r_cyl = shape_maker::cylinder(30, 0.2, 0.1, 0.5);

	/* define the three axis to show a frame */
	renderable r_frame = shape_maker::frame();

	check_gl_errors(__LINE__, __FILE__);

	/* Transformation to setup the point of view on the scene */
	glm::mat4 proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 5 * 1.5, 10.f * 1.5), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));


	matrix_stack stack;

	/*Initialize the matrix to implement the continuos rotation aroun the y axis*/
	glm::mat4 R = glm::mat4(1.f);


	glEnable(GL_DEPTH_TEST);
	glUseProgram(basic_shader.program);
	glUniformMatrix4fv(basic_shader["uView"], 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(basic_shader["uProj"], 1, GL_FALSE, &proj[0][0]);

	glm::mat4 rot_cyl = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0, 0, 1));

	float angle = 0;
	glm::mat4 elbow_rotation;

	auto draw_robot_arm = [&]() {
		stack.push();
		/* render box and cylinders so that the look like a ribit arm */
		glUniform1i(basic_shader["uShade"], 1);
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.8, 0.4, 0.0);
		r_sphere.bind();
		glDrawElements(r_sphere().mode, r_sphere().count, r_sphere().itype, 0);

		stack.push();
		stack.mult(rot_cyl);
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.7f, 1.5f, 0.7f)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.8, 0.6, 0.0);
		r_cyl.bind();
		glDrawElements(r_cyl().mode, r_cyl().count, r_cyl().itype, 0);
		stack.pop();


		/*
		* elbow frame
		*/
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(3.f, 0.f, 0.f)));
		stack.mult(elbow_rotation);

		stack.push();
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.8f, 0.8f, 0.8f)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.0, 0.4, 0.8);
		r_sphere.bind();
		glDrawElements(r_sphere().mode, r_sphere().count, r_sphere().itype, 0);
		stack.pop();

		stack.push();
		stack.mult(rot_cyl);
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.6f, 1.5f, 0.6f)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.0, 0.6, 0.8);
		r_cyl.bind();
		glDrawElements(r_cyl().mode, r_cyl().count, r_cyl().itype, 0);
		stack.pop();

		/* wrist frame
		*/
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(3.f, 0.f, 0.f)));

		stack.push();
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.6f, 0.6f, 0.6f)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.4, 0.8, 0.0);
		r_sphere.bind();
		glDrawElements(r_sphere().mode, r_sphere().count, r_sphere().itype, 0);
		stack.pop();

		stack.push();
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(0.6f, 0.0f, 0.0f)));
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.4f, 0.4f, 0.4f)));
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.0f, 0.0f)));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.6, 0.8, 0.0);
		r_cube.bind();
		glDrawElements(r_cube().mode, r_cube().count, r_cube().itype, 0);
		stack.pop();

		/*
		* the following translation is common to the three fingers, therefore is one more frame
		* in the hierarchy, although it does not make sense to move it
		*/
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(1.4f, 0.0f, 0.0f)));

		stack.push();
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.4f, 0.1f, 0.1f)));
		stack.mult(rot_cyl);
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.8, 0.5, 0.0);
		r_cyl.bind();
		glDrawElements(r_cyl().mode, r_cyl().count, r_cyl().itype, 0);
		stack.pop();

		stack.push();
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.2f, 0.0f)));
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.4f, 0.1f, 0.1f)));
		stack.mult(rot_cyl);
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.8, 0.5, 0.0);
		r_cyl.bind(); glDrawElements(r_cyl().mode, r_cyl().count, r_cyl().itype, 0);
		stack.pop();

		stack.push();
		stack.mult(glm::translate(glm::mat4(1.f), glm::vec3(0.f, -0.2f, 0.0f)));
		stack.mult(glm::scale(glm::mat4(1.f), glm::vec3(0.4f, 0.1f, 0.1f)));
		stack.mult(rot_cyl);

		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &stack.m()[0][0]);
		glUniform3f(basic_shader["uColor"], 0.8, 0.5, 0.0);
		r_cyl.bind();
		glDrawElements(r_cyl().mode, r_cyl().count, r_cyl().itype, 0);
		stack.pop();


		stack.pop();

		glm::mat4 scale_frame = glm::scale(glm::mat4(1.f), glm::vec3(8.f, 8.f, 8.f));
		r_frame.bind();
		glm::mat4 scaleframe_matrix = glm::scale(glm::mat4(1.0), glm::vec3(3, 3, 3));
		glUniformMatrix4fv(basic_shader["uModel"], 1, GL_FALSE, &scale_frame[0][0]);
		glUniform1i(basic_shader["uShade"], 0);
		glDrawArrays(GL_LINES, 0, 6);

		};



	stack.load_identity();
	float t = 0.f;
	float dt = 0.005f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* one full rotation every 6 seconds*/
		//angle = (60.f * clock() / CLOCKS_PER_SEC);

		R = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f));

		// uncomment the next line for stopping the rotation
		// R = glm::mat4(1.f);
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform1i(basic_shader["uShade"], 1);
		glUniformMatrix4fv(basic_shader["uRot"], 1, GL_FALSE, &R[0][0]);

		// make so the t runs from 0 to 1 and back continuosly
		if (t > 1.0 || t < 0.0)
			dt *= -1;
		t += dt;

		/* define the two rotations == the two orthonormal frames
		* among which to interpolate
		*/
		float start_angle = 0.f;
		glm::vec3 start_rotation_axis = glm::vec3(0.0f, 0.0f, 1.0f);
		float end_angle = 90.f;
		glm::vec3 end_rotation_axis = glm::vec3(0.0f, 0.0f, 1.0f);

		/* Using matrices
		*/
		glm::mat4 rot_a = glm::rotate(glm::mat4(1.f), glm::radians(start_angle), start_rotation_axis);
		glm::mat4 rot_b = glm::rotate(glm::mat4(1.f), glm::radians(end_angle), end_rotation_axis);
		glViewport(0, 0, 500, 400);
		elbow_rotation = rot_a * (1 - t) + rot_b * t;

		// uncomment to see what happens if we make the transformation area-preserving
	    //float s = 1.f / pow(glm::determinant(elbow_rotation), 1.f / 3.f);
		//elbow_rotation = glm::scale(elbow_rotation, glm::vec3(s, s, s));

		/* render the arm*/
		draw_robot_arm();


		glViewport(500, 0, 500, 400);
		/* Using quaternions with slerp
		*/
		glm::quat quaternion_A = glm::angleAxis(glm::radians(start_angle), start_rotation_axis);
		glm::quat quaternion_B = glm::angleAxis(glm::radians(end_angle), end_rotation_axis);
		elbow_rotation = glm::mat4_cast(glm::slerp(quaternion_A, quaternion_B, t));
		draw_robot_arm();



		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glUseProgram(0);
	glfwTerminate();
	return 0;
}