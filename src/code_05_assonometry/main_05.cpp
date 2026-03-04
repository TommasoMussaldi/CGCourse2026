#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../common/debugging.h"
#include "../common/renderable.h"
#include "../common/simple_shapes.h"
#include "../common/matrix_stack.h"
#include "../common/shaders.h"

float alpha_S, alpha_E,alpha_W;
float x_min , x_max , y_min  , y_max  , z_min  , z_max;

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

void retarget_viewport(
    int windowWidth,
    int windowHeight,
    float viewWidth,
    float viewHeight,
    int& outX,
    int& outY,
    int& outWidth,
    int& outHeight)
{
    float windowAspect = static_cast<float>(windowWidth) / windowHeight;
    float viewAspect = viewWidth / viewHeight;

    if (windowAspect > viewAspect)
    {
        // Window is wider → pillarbox
        outHeight = windowHeight;
        outWidth = static_cast<int>(windowHeight * viewAspect);

        outX = (windowWidth - outWidth) / 2;
        outY = 0;
    }
    else
    {
        // Window is taller → letterbox
        outWidth = windowWidth;
        outHeight = static_cast<int>(windowWidth / viewAspect);

        outX = 0;
        outY = (windowHeight - outHeight) / 2;
    }
}

glm::mat4 cavalieri_assonometry(float alpha, float x_min,float x_max, float y_min, float y_max, float z_min, float z_max) {

    float delta_x = x_max - x_min;
    float delta_y = y_max - y_min;
    float delta_z = z_max - z_min;
	float ca = glm::cos(alpha);
	float fca = fabs(ca);
	float sa = glm::sin(alpha);
	float fsa = fabs(sa);

	glm::mat4 T  = glm::translate(glm::vec3(-(x_min+x_max)*0.5, -(y_min+y_max)*0.5, -(z_min+z_max)*0.5));
	glm::mat4 S  = glm::scale(glm::vec3(2/delta_x, 2/delta_y,2/delta_z));
    glm::mat4 Sh = glm::transpose(glm::mat4x4  (1, 0, ca, 0,
                                                0, 1, sa, 0,
                                                0, 0, -1, 0,
                                                0, 0, 0, 1));
    float m = std::max(fca, fsa);
    glm::mat4 S1 = glm::scale(glm::vec3(1.0/(1+m) , 1.0/(1+ m), 1));

	return   S1*Sh * S * T;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Update viewport to match new framebuffer size
    glViewport(0, 0, width, height);
    
    // If you are using your RetargetViewport function:
    int x, y, w, h;

    retarget_viewport(width, height,
        x_max-x_min, y_max-y_min,   // your virtual view size
        x, y, w, h);

    glViewport(x, y, w, h);

    // If you also use a projection matrix,
    // update it here as well.
}

int main(int argc, char** argv) {

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Request OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Ask specifically for the core profile (recommended)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // macOS requires this for 3.2+ contexts
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024, 1024, "code_05_assonometry", NULL, NULL);


    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

  
    glfwSetKeyCallback(window, keyboard_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   // Load GL symbols *after* the context is current
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    /* query for the hardware and software specs and print the result on the console*/
    printout_opengl_glsl_info();

    check_gl_errors(__LINE__, __FILE__);

    renderable quad, frame;
    frame = shape_maker::frame();
    quad = shape_maker::cube(4);
    

    shader s;
    s.bind_attribute("aPosition", 0);
    s.create_program("shaders/basic.vert", "shaders/basic.frag");
    glUseProgram(s.program);

    /* cal glGetError and print out the result in a more verbose style
    * __LINE__ and __FILE__ are precompiler directive that replace the value with the
    * line and file of this call, so you know where the error happened
    */
    check_gl_errors(__LINE__, __FILE__);
    glm::mat4 view;
    glm::mat4 proj;

    x_min = -1.0;
    x_max = 1.0;
    y_min = -1.0; 
    y_max = 1.0;
    z_min = -1;
    z_max = 1;

	// With  the next 2 lines you'll see the scene from an angle
    //proj = glm::frustum(- 1, 1, -1, 1,1, 100 );
    //view = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	// With the next 2 lines you'll see the scene from the z-axis, with an orthographic projection 
    //proj = glm::ortho(x_min, x_max, y_min, y_max, z_min, z_max);
    view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    
	// with the next line you'll see the scene from the z-axis, with an assonometric projection
    proj = cavalieri_assonometry(glm::radians(-135.f), x_min,x_max,y_min,y_max,z_min,z_max);

    // create the matrix stack, initially it stores the identity
    matrix_stack stack;

    // multiply by projection and view matrix. This is currently done outside the rendering loop
    // because these two transformations will not change and need to be applied for everything that is rendered
    stack.mult(proj*view );

    glEnable(GL_DEPTH_TEST);
    
	glViewport(0, 0, 1024, 1024);
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2, 0.2, 0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  

        quad.bind();
        stack.push();
		stack.mult(glm::translate(glm::vec3(0, 0, 0.2))* glm::scale(glm::vec3(1, 1, 0.1))*glm::translate(glm::vec3(0, 0,1)));
        glUniformMatrix4fv(s["uM"], 1, GL_FALSE, glm::value_ptr(stack.m()));
        stack.pop();
        glUniform3f(s["uCol"], 1, 0, 0);
        glDrawElements(quad().mode, quad().count, quad().itype, NULL);

        stack.push();
        stack.mult(glm::translate(glm::vec3(0, 0, 0.4)) * glm::scale(glm::vec3(1, 1, 0.1)) * glm::translate(glm::vec3(0, 0, 1)));
        glUniformMatrix4fv(s["uM"], 1, GL_FALSE, glm::value_ptr(stack.m()));
        stack.pop();
        glUniform3f(s["uCol"], 0, 1, 0);
        glDrawElements(quad().mode, quad().count, quad().itype, NULL);

        stack.push();
        stack.mult(glm::translate(glm::vec3(0, 0, 0.6)) * glm::scale(glm::vec3(1, 1, 0.1)) * glm::translate(glm::vec3(0, 0, 1)));
        glUniformMatrix4fv(s["uM"], 1, GL_FALSE, glm::value_ptr(stack.m()));
        stack.pop();
        glUniform3f(s["uCol"], 0, 0, 1);
        glDrawElements(quad().mode, quad().count, quad().itype, NULL);

        stack.push();
        stack.mult(glm::translate(glm::vec3(0, 0, 0.8)) * glm::scale(glm::vec3(1, 1, 0.1)) * glm::translate(glm::vec3(0, 0, 1)));
        glUniformMatrix4fv(s["uM"], 1, GL_FALSE, glm::value_ptr(stack.m()));
        stack.pop();
        glUniform3f(s["uCol"], 1, 1, 0);
        glDrawElements(quad().mode, quad().count, quad().itype, NULL);


        frame.bind();
        stack.push();
		stack.mult(glm::scale(glm::vec3(3, 3, 3)));
        glUniformMatrix4fv(s["uM"], 1, GL_FALSE, glm::value_ptr(stack.m()));
        stack.pop();
        glUniform3f(s["uCol"], -1, 0, 1);
        glDrawArrays(GL_LINES, 0,6);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}