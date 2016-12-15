//
//  ScribbleShader.frag
//
//
//  Created by renhong on 7/24/14.
//
//

const char* HigthLightShader_frag = STRINGIFY(
                                              \n#ifdef GL_ES\n
                                              precision mediump float;
                                              \n#endif\n
                                              
                                              varying vec4 v_fragmentColor;
                                              varying vec2 v_texCoord;
                                              
                                              uniform float highLight;
                                              void main() {
                                                  vec4 color1 = texture2D(CC_Texture0, v_texCoord);
                                                  color1.rgb = color1.rgb*highLight;
                                                  gl_FragColor = v_fragmentColor * color1;
                                                  
                                              }
                                              );
