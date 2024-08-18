internal void
render_Init(platform_engine* engine, platform_render* render)
{
	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &engine->vsSource, NULL);
    glCompileShader(vertexShader);
    i32 success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        printf("VS: %s\n", infoLog);
		return;
    }

    u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &engine->psSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        printf("PS: %s\n", infoLog);
		return;
    }
    
    render->program = glCreateProgram();
    glAttachShader(render->program, vertexShader);
    glAttachShader(render->program, fragmentShader);
    glLinkProgram(render->program);
    glGetProgramiv(render->program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(render->program, sizeof(infoLog), NULL, infoLog);
		engine->log(infoLog);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Test: vertices
    vertex vertices[] =
    {
        { 0.f, 0.f, 500.f, 500.f, 0.f, 0.f, 2.f, 2.f },
    };
    
    glGenBuffers(1, &render->buffer);
    
    glGenVertexArrays(1, &render->vao);
    glBindVertexArray(render->vao);
    glBindBuffer(GL_ARRAY_BUFFER, render->buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
	glBindVertexArray(render->vao);
	glBindBuffer(GL_ARRAY_BUFFER, render->buffer);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(offsetof(vertex, p0)));
	glVertexAttribDivisor(0, 1);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(offsetof(vertex, p1)));
	glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(offsetof(vertex, t0)));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(offsetof(vertex, t1)));
	glVertexAttribDivisor(3, 1);
	
	u32 textureData[] =
	{
		0x00000000, 0xffffffff,
		0xffffffff, 0x00000000,
	};
	
	glGenTextures(1, &render->texture);
	glBindTexture(GL_TEXTURE_2D, render->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindVertexArray(0);
}

internal void
render_Resize(platform_engine* engine, u32 width, u32 height)
{
    engine->windowWidth = width;
	engine->windowHeight = height;
}

internal void
render_DrawScene(platform_engine* engine, platform_render* render, float dt)
{
    glClearColor(0.f, 0.38f, 0.85f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
	u32 vpUniformLocation = glGetUniformLocation(render->program, "viewProjection");
	matrix viewProjection = game_OrthographicProjection(V2(0, 0), engine->windowWidth, engine->windowHeight);
	glUniformMatrix4fv(vpUniformLocation, 1, GL_FALSE, (f32*)&viewProjection);
	
    glBindVertexArray(render->vao);
    glUseProgram(render->program);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
}

internal void
render_Cleanup(platform_engine* engine, platform_render* render)
{
    
}
