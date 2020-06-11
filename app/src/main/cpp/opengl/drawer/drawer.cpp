//
// Created by cxp on 2019-08-06.
//

#include <malloc.h>
#include "drawer.h"
#include "../../utils/logger.h"

Drawer::Drawer(int origin_width, int origin_height):
m_origin_width(origin_width),
m_origin_height(origin_height) {
}

Drawer::~Drawer() {
}

void Drawer::SetSize(int width, int height) {
    this->m_origin_width = width;
    this->m_origin_height = height;
}

void Drawer::Draw() {
    if (IsReadyToDraw()) {
        CreateTextureId();
        CreateProgram();
        BindTexture();
        PrepareDraw();
        DoDraw();
        DoneDraw();
    }
}

bool Drawer::IsReadyToDraw() {
    return m_origin_width > 0 && m_origin_height > 0;
}

void Drawer::DoDraw() {
    //启用顶点的句柄
    glEnableVertexAttribArray(m_vertex_pos_handler);
    glEnableVertexAttribArray(m_texture_pos_handler);
    //设置着色器参数
//    glUniformMatrix4fv(m_vertex_matrix_handler, 1, false, m_matrix, 0);
    glVertexAttribPointer(m_vertex_pos_handler, 2, GL_FLOAT, GL_FALSE, 0, m_vertex_coors);
    glVertexAttribPointer(m_texture_pos_handler, 2, GL_FLOAT, GL_FALSE, 0, m_texture_coors);
    //开始绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Drawer::CreateTextureId() {
    if (m_texture_id == 0) {
        glGenTextures(1, &m_texture_id);
        LOGI(TAG, "Create texture id : %d, %x", m_texture_id, glGetError())
    }
}

void Drawer::CreateProgram() {
    if (m_program_id == 0) {
        //创建一个空的OpenGLES程序，注意：需要在OpenGL渲染线程中创建，否则无法渲染
        m_program_id = glCreateProgram();
        LOGI(TAG, "create gl program : %d, %x", m_program_id, glGetError())
        if (glGetError() != GL_NO_ERROR) {
            return;
        }

        GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, GetVertexShader());
        GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, GetFragmentShader());

        //将顶点着色器加入到程序
        glAttachShader(m_program_id, vertexShader);
        //将片元着色器加入到程序中
        glAttachShader(m_program_id, fragmentShader);
        //连接到着色器程序
        glLinkProgram(m_program_id);

        m_vertex_matrix_handler = glGetUniformLocation(m_program_id, "uMatrix");
        m_vertex_pos_handler = glGetAttribLocation(m_program_id, "aPosition");
        m_texture_handler = glGetUniformLocation(m_program_id, "uTexture");
        m_texture_pos_handler = glGetAttribLocation(m_program_id, "aCoordinate");

        InitCstShaderHandler();

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    //使用OpenGL程序
    if (m_program_id != 0) {
        glUseProgram(m_program_id);
    }
}

GLuint Drawer::LoadShader(GLenum type, const GLchar *shader_code) {
    LOGI(TAG, "Load shader:\n %s", shader_code)
    //根据type创建顶点着色器或者片元着色器
    GLuint shader = glCreateShader(type);
    //将资源加入到着色器中，并编译
    glShaderSource(shader, 1, &shader_code, NULL);
    glCompileShader(shader);

    GLint compiled;
    // 检查编译状态
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            GLchar* infoLog = (GLchar*) malloc(sizeof(GLchar) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            LOGI(TAG, "Error compiling shader:\n%s\n", infoLog);

            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

void Drawer::ActivateTexture(GLenum type, GLuint texture, GLenum index, int texture_handler) {
    if (texture == -1) texture = m_texture_id;
    if (texture_handler == -1) texture_handler = m_texture_handler;
    //激活指定纹理单元
    glActiveTexture(GL_TEXTURE0 + index);
    //绑定纹理ID到纹理单元
    glBindTexture(type, texture);
    //将活动的纹理单元传递到着色器里面
    glUniform1i(texture_handler, index);
    //配置边缘过渡参数
    glTexParameterf(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Drawer::Release() {
    glDisableVertexAttribArray(m_vertex_pos_handler);
    glDisableVertexAttribArray(m_texture_pos_handler);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &m_texture_id);
    glDeleteProgram(m_program_id);
}