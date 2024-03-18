#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


GLuint loadBMP_custom(const char * imagepath){

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	// Everything is in memory now, the file can be closed.
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}


GLuint loadBMP(const char* imagepath) {
	int         req_channels = 3; // 3 color channels of BMP-file   

	int width = 0, height = 0, channels = 0;

	stbi_set_flip_vertically_on_load(true);
	stbi_uc *image = stbi_load( imagepath, &width, &height, &channels, 3 ); 

	GLuint textureId = 0;
	if ( image != nullptr )
	{
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // default

		stbi_image_free( image );
	}

	return textureId;

	// std::cout << "OK O" << std::endl;

    // unsigned char header[54];
    // unsigned char* data;
    // unsigned int dataPos, imageSize, width, height;

    // FILE* file = fopen(imagepath, "rb");
    // if (!file) {
    //     printf("%s not found\n", imagepath);
    //     getchar();
    //     return 0;
    // } if (fread(header, 1, 54, file) != 54) {
    //     printf("Invalid BMP file\n");
    //     fclose(file);
    //     return 0;
    // } if ( header[0]!='B' || header[1]!='M' ) {
	// 	printf("Not a correct BMP file\n");
	// 	fclose(file);
	// 	return 0;
	// } if (*(int*) &(header[0x1E]) != 0) {
    //     printf("Not a correct BMP file\n");
	// 	fclose(file);
	// 	return 0;
    // } if (*(int*) &(header[0x1C]) != 24) {
    //     printf("Not a correct BMP file\n");
	// 	fclose(file);
	// 	return 0;
    // }

	// std::cout << "OK Opened" << std::endl;


    // dataPos     = *(int*) &(header[0x0A]);
    // imageSize   = *(int*) &(header[0x22]);
	// width       = *(int*) &(header[0x12]);
	// height      = *(int*) &(header[0x16]);

    // if (imageSize==0) imageSize = width * height * 3;
    // if (dataPos==0) dataPos = 54;

    // data = new unsigned char[imageSize];
    // fread(data, 1, imageSize, file);
    // fclose(file);
	// std::cout << "OK Read" << std::endl;


    // GLuint textureID;
    // glGenTextures(1, &textureID);
	// std::cout << "OK Clean -1" << std::endl;

    // glBindTexture(GL_TEXTURE_2D, textureID);
	// std::cout << "OK Clean 0" << std::endl;
	
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	// std::cout << "OK Clean 1" << std::endl;

    // delete [] data;
	// std::cout << "OK Clean" << std::endl;


    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glGenerateMipmap(GL_TEXTURE_2D);

	// std::cout << "OK end" << std::endl;

    // return textureID;
}