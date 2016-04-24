//GLEW manages function pointers to OpenGL.
#define GLEW_STATIC
#include<GL\glew.h>
//GLFW provides windowing and user input functions.
#include<GLFW\glfw3.h>
//need to add this otherwise cout is not found in std namespace
#include<iostream>
#include<string>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	//Need to initialize GLEW before calling any openGL functions.
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	//for the sake of simplicity we are providing values in NDC. real applications wont necessarily have it and we will
	//need to transform them to NDC in VS.
	//these are vertices for 4 corners of a rectangle or 2 triangles
	/*GLfloat vertices[] = {
	0.5f, 0.5f, 0.0f,	//top right
	0.5f, -0.5f, 0.0f,	//bottom right
	-0.5f, -0.5f, 0.0f,	//bottom left
	-0.5f, 0.5f, 0.0f	//top left
	};*/
	//creating a new array which also holds the color value for each vertex along with its position
	//adding color data as well. 
	GLfloat vertices[] = {
		//Positions		  //color
		0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f,	//top right
	   -0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 0.0f,	//bottom right
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f	//bottom left
	};
	//In this array we specify the order in which we want to draw the 2 triangles. This way we need not store the 2 common vertices for our 2 triangles
	GLuint indices[] = {
		0, 1, 3,	//First triangle
		1, 2, 3		//second triangle
	};
	//vertex buffer object id
	GLuint VBO;
	//vertex array object id (explained later)
	GLuint VAO;
	//element buffer object id. This is used to store the index elements in a buffer just like VBO
	GLuint EBO;
	glGenVertexArrays(1, &VAO);
	//generate '1' buffer object which will hold the vertex data on GPUs memory. VBO is the unique ID of this buffer object.
	glGenBuffers(1, &VBO);
	//create am element buffer object
	glGenBuffers(1, &EBO);
	//read till we unbind this and then continue reading from here: in a normal OpenGL program there are many vertex buffers, usually 1 for each object, which we want to bind at the time of their
	//drawing and specify how the data needs to be interpreted in shader attributes. this means that for each object we have to bind its vetretx buffer and then define its vertex attribute pointers
	//each time we want to draw them. this leads to lot of repeated and cumbersome code. OpenGL allows us to store the bind and attribute pointer statements in a vertex array object. 
	//once saved in a vertex array object, we just need to bind the appropriate VAO and its state will be restored.
	//to save the state we bind the VAO and then any vertex bind, attrib pointer and attrib pointer enable statements are stored in VAO until we unbind the VAO
	glBindVertexArray(VAO);
	//Next we need to bind this newly created buffer object to vertex buffer object target which is GL_ARRAY_BUFFER.
	//OpenGL allows us to bind to several buffer objects at once as long as they are of different buffer types.
	//After this point, any call we make to the GL_ARRAY_BUFFER target will be used to configure the currently bound buffer VBO.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//copies the previously defined vertex data into vertex buffer memory. 4th argument says that the data will most likely not change at all or very rarely
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//just like VBO we need to bind EBO to element array buffer. this call will be saved in the VAO too so no need to redefine it everytime we want to use it.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//so far we have stored our vertex data in GPU memory
	//however, we have not specified how to interpret vertex data and connect the vertex data to the attributes(variables) in vertex shader
	//tell opengl how to interpret the data in VBO and connect it to 'posiiton' in attribute in VS
	//1st argument specifies which VS attribute we want to configure. position is given a location of 0 in layout in VS so we use that
	//2nd argument specifies the size of the VA. it is vec3 so we specify 3
	//3rd specifies the type of data which is float. each value in vector is a float
	//4th specifies if we want to normalize to NDC. we set to false because we are already giving normalized values
	//5th argument is the stride between consecutive VA sets. After we added the color data, this changes. Each new vertex start after 6th byte.
	//6th is the offset of where the position data begins in the buffer. it is 0 in this case.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
	//we also have to enable the VA giving the location of the VA as argument
	glEnableVertexAttribArray(0);
	//color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	//unbind the VAO. all statements between bind and unbind are now stored in the VAO.
	glBindVertexArray(0);

	//vertex shader is stored in a string and later compiled at run time
	//shaders always begin with a version declaration
	//vertex shader input is called as vertex attribute
	//HW limits the number of VAs we can declare. GLSL guarantees 16 4-component wise
	//below is 1 way to write a shader but there is a better way after that
	//const GLchar *vertexShaderSource = "#version 330 core\n layout(location = 0) in vec3 position;\n void main()\n { gl_Position = vec4(position.x, position.y, position.z, 1.0);	}";
	//Now since we have more data to pass to vertex shader, we have to update our shader code.
	const GLchar *vertexShaderSource = "#version 330 core\n layout(location = 0) in vec3 position;\n layout(location = 1) in vec3 color;\n out vec3 ourColor;\n void main()\n { gl_Position = vec4(position, 1.0);	\n ourColor = color;}";
	//below 3 lines are optional and used to check how many VAs are supported by HW
	GLint nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << std::endl;
	GLuint vertexShader;
	//create a vertex shader object referenced by an ID
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//bind the shader source to the vertex shader object. 2nd parameter is the number of strings we want to pass.
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//if we want to check the result of compilation, we can do it this way
	//create a variable to store the result of compilation
	GLint success;
	//create a storage container for error message (if any)
	GLchar infoLog[512];
	//check if compilation was successful
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	//if compilation failed, get the error message and print it
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "EROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//repeat the same steps for fragment shader
	//The output of vertex shader should have the same name and type as the input of fragment shader so that they can be linked together.
	//below line is an example of passing data from vertex shader to fragment shader.
	//const GLchar* fragmentShaderSource = "#version 330 core\n in vec4 vertexColor;\n out vec4 color;\n void main()\n { color = vertexColor; }";
	//another way to pass data from application to the shader. Uniforms are different from vertex attributes'
	//uniforms are global. it is unique per SPO and can be accessed from any shader until its updated
	//Notice how instead of taking color value from the output of vertex shader we are taking it from a uniform
	//since we are not using the uniform in VS there is no need to define it there. 
	const GLchar* fragmentShaderSource = "#version 330 core\n in vec3 ourColor;\n out vec4 color;\n void main()\n { color = vec4(ourColor, 1.0f); }";
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Shader program object (SPO) is the final linked version of multiple shaders combined.
	//to use the previously compiled shaders we have to link them to a SPO and activate the SPO
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	//next we need to attach both the compiled shaders to the SPO
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//finally link the shaders together. The output of one shader will the input of next
	glLinkProgram(shaderProgram);
	//get the status of linking. notice the differences
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//we can now delete the individual VS and FS because they are linked into the SPO
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glViewport(0, 0, 800, 600);
	glfwSetKeyCallback(window, key_callback);
	//set opengl state to draw in wireframe mode. any subsequent draw calls will be affected.
	//1st argument tells to apply it to front and back of all triangles.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//game loop. each loop is 1 frame
	while (!glfwWindowShouldClose(window))
	{
		//poll for any user input events and call the appropriate function.
		glfwPollEvents();
		//set the defualt clear color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//activate and use the SPO
		//glClear needs the bit which specifies the buffer we want to clear
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		//Here we will update the uniform color in fragment shader to gradually change color over time
		//first we retrieve the running time in seconds
		GLfloat timeValue = glfwGetTime();
		//then we vary the color in the range from 0.0 - 1.0 by using the sin function
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		//next we query for the location of uniform variable ourColor in fragment shader
		GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		//lastly we set the uniform value to the varying green color. before updating the uniform the SPO should be in use
		//notice that we are using a single color for all fragments in 1 frame because all fragments in the FS use the same color.
		//what if we want different colors for different fragments? calculate new color value in fragment shader instead of gameloop 
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		//bind the vertex array we want to use
		glBindVertexArray(VAO);
		//draw primitives using currently active shader. 2nd argument specifies the starting index of the vertex array. last argument tells how many vertices we want to draw.
		glDrawArrays(GL_TRIANGLES, 0, 3);
		//since we are now using elements to specify the order of drawing we use a differnt function to draw.
		//2nd argument specifies the number of vertices we want to draw. 3rd argument is the type of indices which is int. 4th is the offset in the EBO
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//swap the current buffer with the finsih rendered new buffer
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}