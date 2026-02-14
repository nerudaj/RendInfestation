uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    float wave = sin(uv.y * 20.0 + time * 4.0) * 0.01;
    uv.x += wave;

    gl_FragColor = gl_Color * texture2D(texture, uv);
}
