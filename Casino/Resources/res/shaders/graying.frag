#ifdef GL_ES
precision mediump float;
#endif

// v_texCoord is the texture coord position, u_texture is our original texture
varying vec2 v_texCoord;
uniform sampler2D u_texture;

void main()
{
    // get the "usual" colour - although we don't actually use it, could be useful for future dev
    vec3 normalColor = texture2D(u_texture, v_texCoord).rgb;

    // set the output pixel colour to magenta, but keep alpha value from original
    vec4 texel = texture2D(u_texture, v_texCoord);
    float gray = (texel.r + texel.g + texel.b)/3.0;
    gl_FragColor = vec4(gray, gray, gray, texel.a);
}
