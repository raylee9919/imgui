static u32
load_and_compile_shader(const char *vs_file,
                            const char *fs_file)
{
    u32 pid = 0;

    u32 vs_id = glCreateShader(GL_VERTEX_SHADER);
    u32 fs_id = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar *header = (const GLchar *)read_entire_file_and_null_terminate("shader/shader.header").contents;
    const GLchar *vs_src = (const GLchar *)read_entire_file_and_null_terminate(vs_file).contents;
    const GLchar *fs_src = (const GLchar *)read_entire_file_and_null_terminate(fs_file).contents;

    const GLchar *vs_unit[] = {
        header,
        vs_src
    };

    const GLchar *fs_unit[] = {
        header,
        fs_src
    };

    if (vs_src && fs_src)
    {
        glShaderSource(vs_id, array_count(vs_unit), (const GLchar **)vs_unit, 0);
        glCompileShader(vs_id);

#if 0
        glGetShaderiv(vs_id, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(vs_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(vs_id, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }
#endif

        glShaderSource(fs_id, array_count(fs_unit), (const GLchar **)fs_unit, 0);
        glCompileShader(fs_id);

#if 0
        glGetShaderiv(fs_id, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(fs_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
            glGetShaderInfoLog(fs_id, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }
#endif

        pid = glCreateProgram();
        glAttachShader(pid, vs_id);
        glAttachShader(pid, fs_id);
        glLinkProgram(pid);

#if 0
        glGetProgramiv(pid, GL_LINK_STATUS, &Result);
        glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            std::vector<char> ProgramErrorMessage(InfoLogLength+1);
            glGetProgramInfoLog(pid, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }
#endif

        glDetachShader(pid, vs_id);
        glDetachShader(pid, fs_id);

        glDeleteShader(vs_id);
        glDeleteShader(fs_id);
    }
    else
    {
        fprintf(stderr, "ERROR: Couldn't read shader sources.\n");
    }

    return pid;
}
