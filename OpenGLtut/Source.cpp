//GLEW manages function pointers to OpenGL.
#define GLEW_STATIC
#include<GL\glew.h>
//Adding SOIL for loading textures in OpenGL
#include<SOIL.h>
//GLFW provides windowing and user input functions.
#include<GLFW\glfw3.h>
//need to add this otherwise cout is not found in std namespace
#include<iostream>
#include<string>

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL\glew.h>

class Shader
{
public:
	//The program id
	GLuint Program;
	//constructor reads and builds the shader. needs file paths of the source code that we can store on disk as simple text files.
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		//ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			//open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			//read files's buffer content into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//close file handlers
			vShaderFile.close();
			fShaderFile.close();
			//convert stream into GLchar array
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		//vertex shader can also be stored in a string and later compiled at run time
		//shaders always begin with a version declaration
		//vertex shader input is called as vertex attribute
		//HW limits the number of VAs we can declare. GLSL guarantees 16 4-component wise
		//below is 1 way to write a shader but there is a better way after that
		//const GLchar *vertexShaderSource = "#version 330 core\n layout(location = 0) in vec3 position;\n void main()\n { gl_Position = vec4(position.x, position.y, position.z, 1.0);	}";
		//Now since we have more data to pass to vertex shader, we have to update our shader code.
		//const GLchar *vertexShaderSource = "#version 330 core\n layout(location = 0) in vec3 position;\n layout(location = 1) in vec3 color;\n out vec3 ourColor;\n void main()\n { gl_Position = vec4(position, 1.0);	\n ourColor = color;}";
		//const GLchar* fragmentShaderSource = "#version 330 core\n in vec3 ourColor;\n out vec4 color;\n void main()\n { color = vec4(ourColor, 1.0f); }";

		//2. Compile shaders
		GLuint vertexShader, fragmentShader;
		//create a variable to store the result of compilation
		GLint success;
		//create a storage container for error message (if any)
		GLchar infoLog[512];
		//create vertex and fragment shaders object referenced by an ID
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		//bind the shader source to the vertex shader object. 2nd parameter is the number of strings we want to pass.
		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);
		//if we want to check the result of compilation, we can do it this way		
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
		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//Shader program object (SPO) is the final linked version of multiple shaders combined.
		//to use the previously compiled shaders we have to link them to a SPO and activate the SPO
		this->Program = glCreateProgram();
		//next we need to attach both the compiled shaders to the SPO
		glAttachShader(this->Program, vertexShader);
		glAttachShader(this->Program, fragmentShader);
		//finally link the shaders together. The output of one shader will the input of next
		glLinkProgram(this->Program);
		//get the status of linking. notice the differences
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		//we can now delete the individual VS and FS because they are linked into the SPO
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	};
	//use the program
	void Use() {
		glUseProgram(this->Program);
	};
};

#endif // SHADER_H

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
	//these are vertices/colors and texture co-ordinates for 4 corners of a rectangle or 2 triangles.
	//consolidating all data in a single array
	GLfloat vertices[] = {
	//position			//color				//texture co-ords
	0.5f, 0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,	//top right
	0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,	//bottom right
	-0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	//bottom left
	-0.5f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f,	0.0f, 1.0f	//top left
	};
	//creating a new array which also holds the color value for each vertex along with its position
	//adding color data as well. 
	//GLfloat vertices[] = {
	//	//Positions		  //color
	//	0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 0.0f,	//top right
	//   -0.5f,-0.5f, 0.0f, 0.0f, 1.0f, 0.0f,	//bottom right
	//	0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f	//bottom left
	//};
	//In this array we specify the order in which we want to draw the 2 triangles. This way we need not store the 2 common vertices for our 2 triangles
	GLuint indices[] = {
		0, 1, 3,	//First triangle
		1, 2, 3		//second triangle
	};
	//texture coordinates for the 3 vertices of the triangle. example: bottom left vertex maps to the bottom left corner of the texture. these co-ordinates are also called s and t coord
	//a 2D texture image has co-ordinates in the range (0,0) to (1,1)
	//fragment interpolation then interpolates the coordinates of all the generated fragments.
	//retreiving the texture value from the texture using the texture coordinates is called sampling
	//GLfloat texCoords[] = {
	//0.0f, 0.0f, //bottom left of the triangle
	//1.0f, 0.0f, //bottom right of the triangle
	//0.5f, 1.0f  //top center of the triangle
	//};
	//Texture wrapping
	//There are many to specify what to do when s and t values specified to sample the texture are outside the range.
	//default is to repeat the texture. there are other modes like clamp to edge, clamp to border and mirrored repeat.
	//below is an example of clamp to border where for values outside the range, a default border color is used.
	//this function is used to specify which type of texture wrap we want to use and for which axis. we can specify different wrap for different axis.
	//the 1st argument is the texture target. since we are using 2D texture, it is 2D. 2nd is the type of texture option for a axis. this is wrap. 3rd option is the type of wrap.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f,1.0f,0.0f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//texture filtering
	//texture coordinates are always between (0,0) and (1,1) but a texture image can be of any resolution.
	//if our object has the same resolution as the texture then OpenGL will map a unique value (texel) for each s and t coordinate
	//however if the object has a different resolution than the texture image then OpenGL has to somehow figure out the best texel for a given s and t value
	//if object has a lower resolution (texture has higher res) then the texture needs to be scaled down to object's resolution. This is also called minification
	//if object has higher resolution than texture then texture needs to be scaled up. This is also called as magnification.
	//for minification we can for example simply select a texel whose center is closest to (s,t). This is called nearest neighbor filtering
	//for magnification we can for example take an interpolated color of the (s,t) coordinates neighbors. closer the distance, higher the color contribution. This technique is called bilinear filtering.
	//nearest neighbor results in block patterns and liner filtering gives smoother results for magnification cases.
	//we have to specify filtering operations for both magnification and minification
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//mipmaps
	//imagine this. we have a room full of objects using the same texture.
	//some objects are closer to the viewer and some are far away. All the objects use a single high resolution texture.
	//assume that for objects which are closer to the viewer, the fragments s and t value have a 1:1 mapping to the texel on texture so opengl can easily sample the color.
	//however for objects that are far away from the viewer, a single fragment will span many texels and opengl will have a hard time figuring out which color to sample.
	//for the above case, minification will solve the problem to some extent but if the texture is scaled down significantly then there will be some artifacts
	//to solve this problem, openGL uses mipmaps. mipmaps are collection of texture images where each subsequent image is half the size of the previous image.
	// the idea is that when a certain distance threshold is reached from the viewer to the object, opengl uses a different scaled down texture image which is half the size of the previous image.
	//mipmaps are good for improving performance as well. Even in case of mipmaps there can be certain artifacts between 2 levels of mipmaps like sharp edges. Just like texture filtering used previously we can use
	//texture filtering BETWEEN mipmap levels to smooth out the edges.
	//so instead of using the above function to set the mag and min filter we can use below function to specify filter for both textture sampling and mipmap level sampling
	//we are setting the minification filter as linear filtering for texture sampling and linear interpolation between two closest mipmap levels.
	//keep in mind that mipmaps are only used for minification filter because we need it only when we downscale textures. 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//loading the wooden container texture. We can write our own image loader functions but we are going to use a library for loading textures in OpenGL called SOIL
	int width, height;
	//1st argument is the location of the image. 2nd and 3rd will be initialized by SOIL from the image's data. 4th is the numbers of channels image has. we'll leave it at 0.
	//5th arg tells how to load the image. since we just want to RGB values we set to RGB.
	unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	//next we are going to generate the texture. store the texture id in a varibale. function takes as input the number of textures we want to generate
	GLuint texture;
	glGenTextures(1, &texture);
	//Now we need to bind this texture so any texture commands will configure this texture. we are binding it to the 2D texture target
	glBindTexture(GL_TEXTURE_2D, texture);
	//now that the texture is bound we can start generating the texture
	//1st arg specifies the texture target. 1D and 3D texture targets are not affected. 2nd arg specifies the mipmap level we want the texture generated for. we set it to 0 or base level.
	//3rd ar specifies the format we want to store the texture in. 4th and 5th tells the width and height we want to use for our texture. next arg should always be 0.
	//7th and 8th ar specifies the format and data type of the source image. last is actual image data.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//now our container texture is attached to the currently bound 2D texture object but it only has the base level of the image loaded.
	//if we want to use mipmaps we have to manually specify the image for each mipmap level by incrementing the level in above function or we can use...
	glGenerateMipmap(GL_TEXTURE_2D);
	//now it is a good practice to free the image memory and unbind the texture object.
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	//5th argument is the stride between consecutive VA sets. After we added the color data, this changes. Each new vertex start after 8th byte in consolidated array.
	//6th is the offset of where the position data begins in the buffer. it is 0 in this case.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	//we also have to enable the VA giving the location of the VA as argument
	glEnableVertexAttribArray(0);
	//color attributes
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	//texture attributes
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	//unbind the VAO. all statements between bind and unbind are now stored in the VAO.
	glBindVertexArray(0);
	//below 3 lines are optional and used to check how many VAs are supported by HW
	GLint nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum number of vertex attributes supported: " << nrAttributes << std::endl;
	
	Shader ourShader("shader.vs","shader.frag");
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
		ourShader.Use();
		//Here we will update the uniform color in fragment shader to gradually change color over time
		//first we retrieve the running time in seconds
		GLfloat timeValue = glfwGetTime();
		//then we vary the color in the range from 0.0 - 1.0 by using the sin function
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
		//next we query for the location of uniform variable ourColor in fragment shader
		GLint vertexColorLocation = glGetUniformLocation(ourShader.Program, "ourColor");
		//lastly we set the uniform value to the varying green color. before updating the uniform the SPO should be in use
		//notice that we are using a single color for all fragments in 1 frame because all fragments in the FS use the same color.
		//what if we want different colors for different fragments? calculate new color value in fragment shader instead of gameloop 
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		//now we also need to bind our texture and it will be automatically assigned to the sampler in the fragment shader
		glBindTexture(GL_TEXTURE_2D, texture);
		//bind the vertex array we want to use
		glBindVertexArray(VAO);
		//draw primitives using currently active shader. 2nd argument specifies the starting index of the vertex array. last argument tells how many vertices we want to draw.
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//since we are now using elements to specify the order of drawing we use a differnt function to draw.
		//2nd argument specifies the number of vertices we want to draw. 3rd argument is the type of indices which is int. 4th is the offset in the EBO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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