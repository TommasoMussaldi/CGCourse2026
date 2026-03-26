/*
    l'obiettivo è creare un quadrato vuoto (di cui
    viene disegnato solo il perimetro).
    Gli si aggiunge uno spostamento e una variazione di colore come visto a
    lezione.
    --> uso i segmenti invece dei triangoli
*/


#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../common/debugging.h"


struct MyShaders {
    const char* aPos = "aPosition";
    const char* aCol = "aColor";
    const char* uOff = "uOffset";

    const char* vertex = R"(
        #version 410
        layout(location = 0) in vec2 aPosition;
        layout(location = 1) in vec3 aColor;
        out vec3 vColor;
        uniform vec2 uOffset;

        void main()
        {
            gl_Position = vec4(aPosition + uOffset, 0.0, 1.0);
            vColor = aColor;
        }
    )";

    const char* fragment = R"(
        #version 410
        layout(location = 0) out vec4 color;
        in vec3 vColor;
        uniform vec2 uOffset;

        void main()
        {
            color = vec4(vColor * 0.5 + vec3(abs(uOffset.x), abs(uOffset.y), 0.5), 1.0);
        }
    )";
};



GLFWwindow* start_window(int w, int h, const char* title)
{
    GLFWwindow* window;

    if (!glfwInit()) return nullptr;

    // Request OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Ask specifically for the core profile (recommended)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    // Load GL symbols *after* the context is current
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    /* query for the hardware and software specs and print the result on the console*/
    printout_opengl_glsl_info();

    return window;
}

void setup_VBO(GLuint& vbo, GLuint& posAttrInd, GLuint& colAttrInd)
{
    // coordinate vertici (X,Y) e colori RGB
    posAttrInd = 0;
    colAttrInd = 1;

    float pos_cols[] = {
        -0.25, -0.25, 1.0, 0.0, 0.0,
        0.25, -0.25, 0.0, 1.0, 0.0,
        0.25, 0.25, 0.0, 0.0, 1.0,
        -0.25, 0.25, 1.0, 1.0, 1.0
    };

    // creo un buffer in vram dove copiare i dati
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // copio i dati e lo attivo
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, pos_cols, GL_STATIC_DRAW);
    glEnableVertexAttribArray(posAttrInd);
    glEnableVertexAttribArray(colAttrInd);

    // calcolo la stride
    GLsizei stride = sizeof(float) * 5;
    // specifico come leggere i dati
    glVertexAttribPointer(posAttrInd, 2, GL_FLOAT, false, stride, 0);
    glVertexAttribPointer(colAttrInd, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 2));
}

void setup_EBO(GLuint& ebo)
{
    // indici
    GLuint indices[] = { 0,1,2, 3 };
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // Caricamento degli indici
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 4, indices, GL_STATIC_DRAW);
}

GLuint setup_geometry(GLuint& vbo_pos_col, GLuint& ebo_ind, GLuint& posAttrInd, GLuint& colAttrInd)
{
    // creo e bindo il vao
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // configuro positions e colors
    setup_VBO(vbo_pos_col, posAttrInd, colAttrInd);
    // configuro la lettura degli indici
    setup_EBO(ebo_ind);

    // unbindo il vao (per sicurezza)
    glBindVertexArray(0);

    return vao;
}

int link_shader_program(GLuint& ps)
{
    glLinkProgram(ps);

    GLint linked;
    // Validazione del programma per verificare eventuali errori di linking
    validate_shader_program(ps);
    glGetProgramiv(ps, GL_LINK_STATUS, &linked);

    if (linked) {
        glUseProgram(ps);
        return 0; // Successo
    }
    else {
        // Recupero l'errore di linking se fallisce
        char infoLog[1024];
        glGetProgramInfoLog(ps, 1024, NULL, infoLog);
        std::fprintf(stderr, "ERRORE LINKING PROGRAMMA:\n%s\n", infoLog);
        return 1; // Fallimento
    }
}

GLuint create_shader_program(const char* vs_src, const char* fs_src)
{
    GLint success;
    char infoLog[512];

    // --- Compilazione Vertex Shader ---
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vs_src, NULL);
    glCompileShader(vertex_shader);

    // Controllo errori Vertex Shader
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::fprintf(stderr, "ERRORE COMPILAZIONE VERTEX SHADER:\n%s\n", infoLog);
        std::exit(EXIT_FAILURE);
    }


    // --- Compilazione Fragment Shader ---
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fs_src, NULL);
    glCompileShader(fragment_shader);

    // Controllo errori Fragment Shader
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::fprintf(stderr, "ERRORE COMPILAZIONE FRAGMENT SHADER:\n%s\n", infoLog);
        std::exit(EXIT_FAILURE);
    }


    // --- Creazione Programma e Attachment ---
    GLuint program_shader = glCreateProgram();
    glAttachShader(program_shader, vertex_shader);
    glAttachShader(program_shader, fragment_shader);

    // Una volta associati, i singoli shader possono essere eliminati 
    // per liberare memoria (il programma mantiene la sua copia interna)
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program_shader;
}





int main(int argc, char** argv) {
    // inizializzo la finestra
    GLFWwindow* window = start_window(800, 800, "MY_DVD_CUBE");
    if (window == nullptr) return EXIT_FAILURE;

    // ====== configurazione coordinate colore indici di lettua e disegno ===
    GLuint pos_colBuffer, indexBuffer; // vbo e ebo
    GLuint positionAttribIndex, colorAttribIndex; // indici attributi

    GLuint vao = setup_geometry(pos_colBuffer, indexBuffer, positionAttribIndex, colorAttribIndex);


    // ====== Creo il programma shader (vertex e fragment shader) ===========
    MyShaders shaders;
    GLuint program_shader = create_shader_program(shaders.vertex, shaders.fragment);
    // linking e verifica
    if (link_shader_program(program_shader) != 0) {
        // Se il link fallisce, stampiamo un errore (già fatto nella funzione) e chiudiamo tutto in modo pulito.
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Recupero della locazione della variabile uniform 'uDelta'
    GLint loc = glGetUniformLocation(program_shader, shaders.uOff);

    // Controllo finale per verificare che non ci siano stati errori OpenGL durante il setup
    check_gl_errors(__LINE__, __FILE__);

    // ====== Variabili per l'animazione del movimento
    float posX = 0.0f, posY = 0.0f;       // Posizione attuale (offset)
    float speedX = 0.01f, speedY = 0.007f; // Velocità diverse per non farlo andare solo in diagonale

    // Impostazione del colore di pulizia dello schermo (Grigio scuro)
    glClearColor(0.2, 0.2, 0.2, 1);


    // ====== Loop principale di rendering
    while (!glfwWindowShouldClose(window)) {
        // Mi preparo ad aggiornare la posizione
        // Rimbalza sull'asse X (sinistra -1.0f, destra 0.5f)
        if (posX < -0.75f || posX > 0.75f) {
            speedX = -speedX;
        }

        // Rimbalza sull'asse Y (sotto -1.0f, sopra 0.5f)
        if (posY < -0.75f || posY > 0.75f) {
            speedY = -speedY;
        }

        // con speedX == speedY abbiamo un movimento piu semplice
        posX += speedX;
        posY += speedY;

        // Aggiornamento della variabile uniform nello shader
        glUniform2f(loc, posX, posY);

        // Pulizia del buffer del colore e del buffer di profondità
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Disegno degli elementi usando l'Index Buffer (disegna i segmenti che formano il quadrato)
        glBindVertexArray(vao);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Pulizia delle risorse GLFW prima della chiusura
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &pos_colBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteProgram(program_shader);

    glfwTerminate();

    return 0;
}
