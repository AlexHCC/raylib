
// Vertex shader definition to embed, no external file required
static const char vStandardShaderStr[] = 
#if defined(GRAPHICS_API_OPENGL_21)
"#version 120                       \n"
#elif defined(GRAPHICS_API_OPENGL_ES2)
"#version 100                       \n"
#endif
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
"attribute vec3 vertexPosition;     \n"
"attribute vec3 vertexNormal;       \n"
"attribute vec2 vertexTexCoord;     \n"
"attribute vec4 vertexColor;        \n"
"varying vec3 fragPosition;         \n"
"varying vec3 fragNormal;           \n"
"varying vec2 fragTexCoord;         \n"
"varying vec4 fragColor;            \n"
#elif defined(GRAPHICS_API_OPENGL_33)
"#version 330                       \n"
"in vec3 vertexPosition;            \n"
"in vec3 vertexNormal;              \n"
"in vec2 vertexTexCoord;            \n"
"in vec4 vertexColor;               \n"
"out vec3 fragPosition;             \n"
"out vec3 fragNormal;               \n"
"out vec2 fragTexCoord;             \n"
"out vec4 fragColor;                \n"
#endif
"uniform mat4 mvpMatrix;            \n"
"void main()                        \n"
"{                                  \n"
"    fragPosition = vertexPosition; \n"
"    fragNormal = vertexNormal;     \n"
"    fragTexCoord = vertexTexCoord; \n"
"    fragColor = vertexColor;       \n"
"    gl_Position = mvpMatrix*vec4(vertexPosition, 1.0); \n"
"}                                  \n";

// Fragment shader definition to embed, no external file required
static const char fStandardShaderStr[] = 
#if defined(GRAPHICS_API_OPENGL_21)
"#version 120                       \n"
#elif defined(GRAPHICS_API_OPENGL_ES2)
"#version 100                       \n"
"precision mediump float;           \n"     // precision required for OpenGL ES2 (WebGL)
#endif
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
"varying vec3 fragPosition;         \n"
"varying vec3 fragNormal;           \n"
"varying vec2 fragTexCoord;         \n"
"varying vec4 fragColor;            \n"
#elif defined(GRAPHICS_API_OPENGL_33)
"#version 330                       \n"
"in vec3 fragPosition;              \n"
"in vec3 fragNormal;                \n"
"in vec2 fragTexCoord;              \n"
"in vec4 fragColor;                 \n"
"out vec4 finalColor;               \n"
#endif
"uniform sampler2D texture0;        \n"
"uniform sampler2D texture1;        \n"
"uniform sampler2D texture2;        \n"
"uniform vec4 colAmbient;           \n"
"uniform vec4 colDiffuse;           \n"
"uniform vec4 colSpecular;          \n"
"uniform float glossiness;          \n"
"uniform int useNormal;             \n"
"uniform int useSpecular;           \n"
"uniform mat4 modelMatrix;          \n"
"uniform vec3 viewDir;              \n"
"struct Light {                     \n"
"    int enabled;                   \n"
"    int type;                      \n"
"    vec3 position;                 \n"
"    vec3 direction;                \n"
"    vec4 diffuse;                  \n"
"    float intensity;               \n"
"    float radius;                  \n"
"    float coneAngle; };            \n"
"const int maxLights = 8;           \n"
"uniform int lightsCount;           \n"
"uniform Light lights[maxLights];   \n"
"\n"  
"vec3 CalcPointLight(Light l, vec3 n, vec3 v, float s)   \n"
"{\n"
"    vec3 surfacePos = vec3(modelMatrix*vec4(fragPosition, 1));\n"
"    vec3 surfaceToLight = l.position - surfacePos;\n"
"    float brightness = clamp(float(dot(n, surfaceToLight)/(length(surfaceToLight)*length(n))), 0.0, 1.0);\n"
"    float diff = 1.0/dot(surfaceToLight/l.radius, surfaceToLight/l.radius)*brightness*l.intensity;\n"
"    float spec = 0.0;\n"
"    if (diff > 0.0)\n"
"    {\n"
"        vec3 h = normalize(-l.direction + v);\n"
"        spec = pow(dot(n, h), 3.0 + glossiness)*s;\n"
"    }\n"
"    return (diff*l.diffuse.rgb + spec*colSpecular.rgb);\n"
"}\n"
"\n"
"vec3 CalcDirectionalLight(Light l, vec3 n, vec3 v, float s)\n"
"{\n"
"    vec3 lightDir = normalize(-l.direction);\n"
"    float diff = clamp(float(dot(n, lightDir)), 0.0, 1.0)*l.intensity;\n"
"    float spec = 0.0;\n"
"    if (diff > 0.0)\n"
"    {\n"
"        vec3 h = normalize(lightDir + v);\n"
"        spec = pow(dot(n, h), 3.0 + glossiness)*s;\n"
"    }\n"
"    return (diff*l.intensity*l.diffuse.rgb + spec*colSpecular.rgb);\n"
"}\n"
"\n"
"vec3 CalcSpotLight(Light l, vec3 n, vec3 v, float s)\n"
"{\n"
"    vec3 surfacePos = vec3(modelMatrix*vec4(fragPosition, 1.0));\n"
"    vec3 lightToSurface = normalize(surfacePos - l.position);\n"
"    vec3 lightDir = normalize(-l.direction);\n"
"    float diff = clamp(float(dot(n, lightDir)), 0.0, 1.0)*l.intensity;\n"
"    float attenuation = clamp(float(dot(n, lightToSurface)), 0.0, 1.0);\n"
"    attenuation = dot(lightToSurface, -lightDir);\n"
"    float lightToSurfaceAngle = degrees(acos(attenuation));\n"
"    if (lightToSurfaceAngle > l.coneAngle) attenuation = 0.0;\n"
"    float falloff = (l.coneAngle - lightToSurfaceAngle)/l.coneAngle;\n"
"    float diffAttenuation = diff*attenuation;\n"
"    float spec = 0.0;\n"
"    if (diffAttenuation > 0.0)\n"
"    {\n"
"        vec3 h = normalize(lightDir + v);\n"
"        spec = pow(dot(n, h), 3.0 + glossiness)*s;\n"
"    }\n"
"    return (falloff*(diffAttenuation*l.diffuse.rgb + spec*colSpecular.rgb));\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    mat3 normalMatrix = mat3(modelMatrix);\n"
"    vec3 normal = normalize(normalMatrix*fragNormal);\n"
"    vec3 n = normalize(normal);\n"
"    vec3 v = normalize(viewDir);\n"
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
"    vec4 texelColor = texture2D(texture0, fragTexCoord);\n"
#elif defined(GRAPHICS_API_OPENGL_33)
"    vec4 texelColor = texture(texture0, fragTexCoord);\n"
#endif
"    vec3 lighting = colAmbient.rgb;\n"
"    if (useNormal == 1)\n"
"    {\n"
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
"        n *= texture2D(texture1, fragTexCoord).rgb;\n"
#elif defined(GRAPHICS_API_OPENGL_33)
"        n *= texture(texture1, fragTexCoord).rgb;\n"
#endif
"        n = normalize(n);\n"
"    }\n"
"    float spec = 1.0;\n"
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
"    if (useSpecular == 1) spec *= normalize(texture2D(texture2, fragTexCoord).r);\n"
#elif defined(GRAPHICS_API_OPENGL_33)
"    if (useSpecular == 1) spec *= normalize(texture(texture2, fragTexCoord).r);\n"
#endif
"    for (int i = 0; i < lightsCount; i++)\n"
"    {\n"
"        if (lights[i].enabled == 1)\n"
"        {\n"
"            if(lights[i].type == 0) lighting += CalcPointLight(lights[i], n, v, spec);\n"
"            else if(lights[i].type == 1) lighting += CalcDirectionalLight(lights[i], n, v, spec);\n"
"            else if(lights[i].type == 2) lighting += CalcSpotLight(lights[i], n, v, spec);\n"
"        }\n"
"    }\n"
#if defined(GRAPHICS_API_OPENGL_33)
"   finalColor = vec4(texelColor.rgb*lighting*colDiffuse.rgb, texelColor.a*colDiffuse.a); \n"
#elif defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_21)
"   gl_FragColor = vec4(texelColor.rgb*lighting*colDiffuse.rgb, texelColor.a*colDiffuse.a); \n"
#endif
"}\n";
