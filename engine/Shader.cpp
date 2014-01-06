
#include <windows.h>
#include <gl\gl.h>
#include <stdio.h>

#include <io.h>
#include <fcntl.h>

#include "Shader.h"
#include "OpenGLExt.h"

Shader::Shader(const char *filename)
{
	m_iUniformErrors = 0;
	Load(filename);
}

Shader::~Shader()
{
	for(int i = 0; i < m_iUniformErrors; i++)
		delete m_UniformErrors[i];
}

void Shader::enable(void) const
{
	glUseProgramObjectARB(m_Handle);
}

void Shader::disable(void) const
{
    glUseProgramObjectARB(0);
}

bool Shader::Load(const char *filename)
{
	strcpy(m_FileName, filename);

//	console->printfAndRender("Compiling Shader '%s'", filename);
   
	if( !readShaderSource() ) return false;
	if( !installShaders() ) return false;

	return true;
}

GLint Shader::GetUniformLocation(const char *name)
{
    GLint loc;

    loc = glGetUniformLocationARB(m_Handle, name);

    if(loc == -1)
	{
		for(int i = 0; i < m_iUniformErrors; i++)
			if( !strcmp(m_UniformErrors[i], name) ) return -1;;

		m_UniformErrors[m_iUniformErrors] = new char[512];

		strcpy(m_UniformErrors[m_iUniformErrors++], name);
		
//		console->printf(1.0f, 0.0f, 0.0f, "Uniform '%s' not found in the shader '%s'", name, strrchr(m_FileName, '\\') + 1);
	}

    return loc;
}

GLint Shader::GetAttribLocation(const char* name) const
{
	GLint loc;

	loc = glGetAttribLocationARB(m_Handle, name);

	if(loc == -1)
	{
		//for(int i = 0; i < m_iUniformErrors; i++)
			//if( !strcmp(m_UniformErrors[i], name) ) return -1;;

		//m_UniformErrors[m_iUniformErrors] = new char[512];

		//strcpy(m_UniformErrors[m_iUniformErrors++], name);
		
//		console->printf(1.0f, 0.0f, 0.0f, "Attribute '%s' not found in the shader '%s'", name, strrchr(m_FileName, '\\') + 1);
	}

    return loc;
}

void Shader::printInfoLog(GLhandleARB obj) const
{
    int infoLogLength = 0;
    int charsWritten  = 0;
    GLcharARB *infoLog;

    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);

    if(infoLogLength > 0)
    {
        infoLog = (GLcharARB*) malloc(infoLogLength);

		glGetInfoLogARB(obj, infoLogLength, &charsWritten, infoLog);
		
//		console->printfAndRender(1.0f, 0.0f, 0.0f, "%s", infoLog);
		MessageBox(NULL, infoLog, "Error", MB_OK | MB_ICONERROR);

        free(infoLog);
    }
}

int Shader::shaderSize(t_ShaderType shdType) const
{
    int shader;
    char name[128];
    int count;

    strcpy(name, m_FileName);

    switch (shdType)
    {
        case VERT_SHD:
            strcat(name, ".vert");
            break;
        case FRAG_SHD:
            strcat(name, ".frag");
            break;
    }

    shader = _open(name, _O_RDONLY);
    if (shader == -1)
        return -1;

    // Percorre o ficheiro até ao fim para saber o tamanho
    count = _lseek(shader, 0, SEEK_END);

    _close(shader);
    return count;
}

bool Shader::readShader(t_ShaderType shdType) const
{
    FILE *p_File;
    char name[100];
    int count;
	int size;
	char *target_str;

    strcpy(name, m_FileName);

    switch (shdType) 
    {
        case VERT_SHD:
            strcat(name, ".vert");
			target_str = m_VertShdStr;
			size = m_VertShdStrLen;
            break;
        case FRAG_SHD:
            strcat(name, ".frag");
			target_str = m_FragShdStr;
			size = m_FragShdStrLen;
            break;
    }

    p_File = fopen(name, "r");
    if (!p_File)
        return false;

    fseek(p_File, 0, SEEK_SET);
    count = int( fread( target_str, 1, size, p_File) );
    target_str[count] = '\0';

    if (ferror(p_File))
	{
		fclose(p_File);
		return false;
	}

	fclose(p_File);

    return true;
}

bool Shader::readShaderSource()
{
    // Alocar memoria para o source code dos shaders
    m_VertShdStrLen = shaderSize(VERT_SHD);
    m_FragShdStrLen = shaderSize(FRAG_SHD);

    if (m_VertShdStrLen == -1)
    {
//		console->printfAndRender(1.0f, 0.0f, 0.0f, "Vertex Shader '%s.vert' not found", m_FileName);
        return false;
    }

    if (m_FragShdStrLen == -1)
    {
//		console->printfAndRender(1.0f, 0.0f, 0.0f, "Fragment Shader '%s.frag' not found", m_FileName);
		return false;
    }

	m_VertShdStr = (GLcharARB *) malloc(m_VertShdStrLen);
    m_FragShdStr = (GLcharARB *) malloc(m_FragShdStrLen);

    // Ler o source code dos shaders
	if (!readShader(VERT_SHD))
    {
//		console->printfAndRender(1.0f, 0.0f, 0.0f, "Error reading '%s.vert'", m_FileName);
        return false;
    }

    if (!readShader(FRAG_SHD)) {
//		console->printfAndRender(1.0f, 0.0f, 0.0f, "Error reading '%s.frag'", m_FileName);
        return false;
    }

    return true;
}

bool Shader::Link()
{
	int linked;

    glLinkProgramARB(m_Handle);

    glGetObjectParameterivARB(m_Handle, GL_OBJECT_LINK_STATUS_ARB, &linked);

    if (!linked)
	{
		printInfoLog(m_Handle);
        return false;
	}

    return true;
}

int Shader::installShaders()
{
    GLhandleARB vs, fs;
    GLint       vertCompiled, fragCompiled;

	// Criar objectos
    vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    // Carregar strings
    glShaderSourceARB(vs, 1, (const GLcharARB**)&m_VertShdStr, NULL);
    glShaderSourceARB(fs, 1, (const GLcharARB**)&m_FragShdStr, NULL);

    glCompileShaderARB(vs);
    glGetObjectParameterivARB(vs, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    if (!vertCompiled)
	{
//		console->printfAndRender(1.0f, 0.0f, 0.0f, "Error compiling vertex shader '%s':", strrchr(m_FileName, '\\') + 1);
		printInfoLog(vs);
		return false;
	}

    glCompileShaderARB(fs);
    glGetObjectParameterivARB(fs, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
	if (!fragCompiled)
	{
//		console->printfAndRender(1.0f, 0.0f, 0.0f, "Error compiling fragment shader '%s':", strrchr(m_FileName, '\\') + 1);
		printInfoLog(fs);
        return false;
	}

	// Criar um objecto e ligar os dois shaders já compilados
    m_Handle = glCreateProgramObjectARB();
    glAttachObjectARB(m_Handle, vs);
    glAttachObjectARB(m_Handle, fs);

	Link();

    return true;
}

/*
===============================================================================
	UNIFORMS
===============================================================================
*/

bool Shader::sendUniform1f(char* varname, GLfloat v0)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform1fARB(loc, v0);
    
    return true;
}

bool Shader::sendUniform2f(char* varname, GLfloat v0, GLfloat v1)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
	glUniform2fARB(loc, v0, v1);
    
    return true;
}

bool Shader::sendUniform3f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2)
{
	GLint loc = GetUniformLocation(varname);
	if (loc == -1) return false;
    
    glUniform3fARB(loc, v0, v1, v2);

    return true;
}

bool Shader::sendUniform4f(char* varname, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform4fARB(loc, v0, v1, v2, v3);

    return true;
}

bool Shader::sendUniform1i(char* varname, GLint v0)
{ 
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform1iARB(loc, v0);
    
    return true;
}

bool Shader::sendUniform2i(char* varname, GLint v0, GLint v1)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform2iARB(loc, v0, v1);

    return true;
}

bool Shader::sendUniform3i(char* varname, GLint v0, GLint v1, GLint v2)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform3iARB(loc, v0, v1, v2);

    return true;
}
bool Shader::sendUniform4i(char* varname, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform4iARB(loc, v0, v1, v2, v3);

    return true;
}

bool Shader::sendUniform1fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform1fvARB(loc, count, value);

    return true;
}

bool Shader::sendUniform2fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform2fvARB(loc, count, value);

    return true;
}

bool Shader::sendUniform3fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform3fvARB(loc, count, value);

    return true;
}

bool Shader::sendUniform4fv(char* varname, GLsizei count, GLfloat *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform4fvARB(loc, count, value);

    return true;
}

bool Shader::sendUniform1iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform1ivARB(loc, count, value);

    return true;
}

bool Shader::sendUniform2iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform2ivARB(loc, count, value);

    return true;
}

bool Shader::sendUniform3iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform3ivARB(loc, count, value);

    return true;
}

bool Shader::sendUniform4iv(char* varname, GLsizei count, GLint *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniform4ivARB(loc, count, value);

    return true;
}

bool Shader::sendUniformMatrix2fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniformMatrix2fvARB(loc, count, transpose, value);

    return true;
}

bool Shader::sendUniformMatrix3fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniformMatrix3fvARB(loc, count, transpose, value);

    return true;
}

bool Shader::sendUniformMatrix4fv(char* varname, GLsizei count, GLboolean transpose, GLfloat *value)
{
    GLint loc = GetUniformLocation(varname);
    if (loc == -1) return false;
    
    glUniformMatrix4fvARB(loc, count, transpose, value);

    return true;
}


void Shader::bindAttribLocation(char *varname, GLuint index)
{
	glBindAttribLocationARB(m_Handle, index, varname);
	Link();
}

