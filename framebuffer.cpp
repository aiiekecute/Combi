#include "stdafx.h"

#include "framebuffer.h"

//colorFormat=(GL_RGBA8)
//depthFormat=GL_DEPTH_COMPONENT16_ARB,	GL_DEPTH_COMPONENT24_ARB, GL_DEPTH_COMPONENT32_ARB (GL_DEPTH_COMPONENT24)
//stencilFormat=GL_STENCIL_INDEX(1,4,8,16)_EXT
bool FRAMEBUFFER::Create(int theWidth, int theHeight, int colorFormat, int depthFormat, int stencilFormat)
{
	id=0;
	renderbuffer_color_id=0;
	renderbuffer_depth_id=0;
	renderbuffer_stencil_id=0;

	width=theWidth;
	height=theHeight;

    glGenFramebuffersEXT(1, &id);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);

	if(colorFormat!=NULL)
	{
		glGenRenderbuffersEXT(1, &renderbuffer_color_id);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer_color_id);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, colorFormat, width, height);
	}

	if(depthFormat!=NULL)
	{
		glGenRenderbuffersEXT(1, &renderbuffer_depth_id);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer_depth_id);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, depthFormat, width, height);
	}

	if(stencilFormat!=NULL)
	{
		glGenRenderbuffersEXT(1, &renderbuffer_stencil_id);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer_stencil_id);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, stencilFormat, width, height);
	}

    if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)!=GL_FRAMEBUFFER_COMPLETE_EXT) return false;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    return true;
}

void FRAMEBUFFER::Delete()
{
	if(renderbuffer_color_id!=0)   glDeleteRenderbuffersEXT(1, &renderbuffer_color_id);
    if(renderbuffer_depth_id!=0)   glDeleteRenderbuffersEXT(1, &renderbuffer_depth_id);
	if(renderbuffer_stencil_id!=0) glDeleteRenderbuffersEXT(1, &renderbuffer_stencil_id);

	if(id!=0) glDeleteFramebuffersEXT(1, &id);
}

int FRAMEBUFFER::GetStatus()
{
	int status;
    status=glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	return status;
}

//attachment=GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHEMNT1_EXT, ..., GL_DEPTH_ATTACHMENT_EXT, GL_STENCIL_ATTACHMENT_EXT
void FRAMEBUFFER::AttachRenderbuffer(GLenum attachment, GLuint rbId)
{
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment, GL_RENDERBUFFER_EXT, rbId);
}
void FRAMEBUFFER::DetachRenderbuffer(GLenum attachment)
{
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment, GL_RENDERBUFFER_EXT, 0);
}

//attachment=GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHEMNT1_EXT, ..., GL_DEPTH_ATTACHMENT_EXT, GL_STENCIL_ATTACHMENT_EXT
//texTarget=GL_TEXTURE_nD, GL_TEXTURE_CUBE_MAP_POSITIVE_X,...,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
void FRAMEBUFFER::AttachTexture(GLenum attachment, GLenum texTarget, GLuint texId)
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, texTarget, texId, 0);
}
void FRAMEBUFFER::DetachTexture(GLenum attachment, GLenum texTarget)
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, texTarget, 0, 0);
}
