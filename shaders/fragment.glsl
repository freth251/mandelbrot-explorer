#version 410 core
out vec4 FragColor;
uniform vec2 u_window_size;
uniform float u_zoom;
uniform vec2 u_center;

struct Complex {
    float r; 
    float i;
}; 


int isMandelbrotSet(Complex zn, Complex c, int n)
{
    while (n < 1000){
        Complex zn_1; 
        zn_1.r = zn.r * zn.r - zn.i * zn.i + c.r;
        zn_1.i = 2 * zn.r * zn.i + c.i; 

        float magnitude = sqrt(zn_1.r * zn_1.r + zn_1.i*zn_1.i);
        if (magnitude > 2.0){
            return n;
        }
        zn = zn_1; 
        n = n + 1;
    }
    return n; 
}


void main() {

    Complex z0; 
    z0.r = 0.0f; 
    z0.i = 0.0f;

    Complex translation; 

    translation.r = 4.0 * (u_center.x) / u_window_size.x;
    translation.i = 4.0 * (u_center.y) / u_window_size.y;

    Complex c; 

    c.r = 4.0 * (gl_FragCoord.x) / u_window_size.x - 2.0 + translation.r; 
    c.i = 4.0 * (gl_FragCoord.y) / u_window_size.y - 2.0 + translation.i;

    c.r = (c.r - translation.r)/ u_zoom + translation.r;
    c.i = (c.i - translation.i) / u_zoom + translation.i;
    int n = isMandelbrotSet(z0, c , 0);
    if (n < 1000){
        FragColor = vec4(1.0 * n / 100 , 0.5 * n / 100, 0.2 *n / 100 , 1.0);
    }else{
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
