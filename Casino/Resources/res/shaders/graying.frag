#ifdef GL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;
uniform sampler2D u_texture;

void main()
{
    vec4 texel = texture2D(u_texture, v_texCoord);
    float gray = (texel.r + texel.g + texel.b)/3.0;
    gl_FragColor = vec4(gray, gray, gray, texel.a);
}
