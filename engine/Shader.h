
#ifndef __SHADER_H__
#define __SHADER_H__

#include <windows.h>
#include <gl\gl.h>
#include "gl\glext.h"
#include "gl\wglext.h"
#include <stdio.h>

#include "OpenGLExt.h"

class Shader {
private:

	typedef enum {
		VERT_SHD,
		FRAG_SHD
	} t_ShaderType;

public:

	Shader(const char *filename);
	~Shader();

	bool Load(const char *filename);
	void enable() const;
	void disable() const;

	GLint GetUniformLocation(const char *name);
	GLint GetAttribLocation(const char *name) const;


	void printInfoLog(GLhandleARB obj) const;
	int shaderSize(t_ShaderType shdType) const;
	bool readShader(t_ShaderType shdType) const;
	bool readShaderSource();
	bool Link();
	int installShaders();

	bool sendUniform1f(char* varname, GLfloat v0);
	bool sendUniform2f(char* varname, GLfloat v0, GLfloat v1);
	bool sendUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2);
	bool sendUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

	bool sendUniform1i(char* varname, GLint v0);
	bool sendUniform2i(char* varname, GLint v0, GLint v1);
	bool sendUniform3i(char* varname, GLint v0, GLint v1, GLint v2);
	bool sendUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3);

	bool sendUniform1fv(char* varname, GLsizei count, GLfloat *value);
	bool sendUniform2fv(char* varname, GLsizei count, GLfloat *value);
	bool sendUniform3fv(char* varname, GLsizei count, GLfloat *value);
	bool sendUniform4fv(char* varname, GLsizei count, GLfloat *value);

	bool sendUniform1iv(char* varname, GLsizei count, GLint *value);
	bool sendUniform2iv(char* varname, GLsizei count, GLint *value);
	bool sendUniform3iv(char* varname, GLsizei count, GLint *value);
	bool sendUniform4iv(char* varname, GLsizei count, GLint *value);

	bool sendUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
	bool sendUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
	bool sendUniformMatrix4fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value);
	
	void bindAttribLocation(char *varname, GLuint index);


	GLhandleARB m_Handle;
private:

	char m_FileName[512];

	GLcharARB *m_VertShdStr;
	GLcharARB *m_FragShdStr;
	int m_VertShdStrLen;
	int m_FragShdStrLen;

	char *m_UniformErrors[512];
	int m_iUniformErrors;



};

#endif /*__SHADER_H__*/