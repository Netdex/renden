#pragma once

// System Headers
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard Headers
#include <memory>
#include <string>
#include <gl/varray.hpp>
#include <fstream>

// Define Namespace

namespace gl {

    class shader {
        // Disable Copying and Assignment
        shader(shader const &) = delete;

        shader &operator=(shader const &) = delete;

        // Private Member Variables
        GLuint mProgram;
        GLint mStatus;
        GLint mLength;

    public:

        // Implement Custom Constructor and Destructor
        shader() { mProgram = glCreateProgram(); }

        ~shader() {
            glDeleteProgram(mProgram);
        }

        // Public Member Functions
        shader &activate(){
            glUseProgram(mProgram);
            return *this;
        }

        shader &attach(std::string const &filename){
            // Load GLSL Shader Source from File
            std::string path = PROJECT_SOURCE_DIR "/renden/shaders/";
//    std::string path = "../../renden/shaders/";
            std::ifstream fd(path + filename);
            auto src = std::string(std::istreambuf_iterator<char>(fd),
                                   (std::istreambuf_iterator<char>()));

            // Create a Shader Object
            const char *source = src.c_str();
            auto shader = create(filename);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);
            glGetShaderiv(shader, GL_COMPILE_STATUS, &mStatus);

            // Display the Build Log on Error
            if (mStatus == false) {
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &mLength);
                std::unique_ptr<char[]> buffer(new char[mLength]);
                glGetShaderInfoLog(shader, mLength, nullptr, buffer.get());
                fprintf(stderr, "%s\n%s", filename.c_str(), buffer.get());
            }

            // Attach the Shader and Free Allocated Memory
            glAttachShader(mProgram, shader);
            glDeleteShader(shader);
            return *this;
        }

        GLuint create(std::string const &filename){
            auto index = filename.rfind(".");
            auto ext = filename.substr(index + 1);
            if (ext == "comp") return glCreateShader(GL_COMPUTE_SHADER);
            else if (ext == "frag") return glCreateShader(GL_FRAGMENT_SHADER);
            else if (ext == "geom") return glCreateShader(GL_GEOMETRY_SHADER);
            else if (ext == "vert") return glCreateShader(GL_VERTEX_SHADER);
            else return false;
        }


        GLuint get() { return mProgram; }

        shader &link(){
            glLinkProgram(mProgram);
            glGetProgramiv(mProgram, GL_LINK_STATUS, &mStatus);
            if (mStatus == false) {
                glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &mLength);
                std::unique_ptr<char[]> buffer(new char[mLength]);
                glGetProgramInfoLog(mProgram, mLength, nullptr, buffer.get());
                fprintf(stderr, "%s", buffer.get());
            }
            assert(mStatus == true);
            return *this;
        }

        GLint get_uniform(std::string const &name){
            return glGetUniformLocation(mProgram, name.c_str());
        }

        GLuint get_attribute(std::string const &name) const{
            GLint attribute_location = glGetAttribLocation(mProgram, name.c_str());
            assert(attribute_location >= 0);
            return static_cast<GLuint>(attribute_location);
        }

        // Wrap Calls to glUniform
        void bind(GLint location, float value) const{ glUniform1f(location, value); }

        void bind(GLint location, glm::vec2 value) const{ glUniform2f(location, value.x, value.y); }

        void bind(GLint location, glm::vec3 value) const{ glUniform3f(location, value.x, value.y, value.z); }

        void bind(GLint location, glm::vec4 value) const{ glUniform4f(location, value.x, value.y, value.z, value.w); }

        void bind(GLint location, glm::mat4 const &matrix) const{
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        }

        template<typename T>
        void bind(std::string const &name, T &&value) const {
            GLint location = glGetUniformLocation(mProgram, name.c_str());
            if (location == -1) fprintf(stderr, "Missing Uniform: %s\n", name.c_str());
            else bind(location, std::forward<T>(value));
        }
    };
}
