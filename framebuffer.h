#include <GL/glew.h>

class FRAMEBUFFER
{
public:
	GLuint id;
	GLuint renderbuffer_color_id;
	GLuint renderbuffer_depth_id;
	GLuint renderbuffer_stencil_id;

    int width;
    int height;

    bool Create(int theWidth, int theHeight, int colorFormat, int depthFormat, int stencilFormat);
    void Delete();
    int GetStatus();

	void AttachRenderbuffer(GLenum attachment, GLuint rbId);
	void DetachRenderbuffer(GLenum attachment);

    void AttachTexture(GLenum attachment, GLenum texTarget, GLuint texId);
	void DetachTexture(GLenum attachment, GLenum texTarget);
};