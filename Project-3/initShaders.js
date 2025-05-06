//-----------------------------------------------------------------------------
//
//  initShaders.js
//
//  A helper function for compiling WebGL shaders.
//

function initShaders( gl, vertexShaderId, fragmentShaderId )
{
    // Begin by determining which version the OpenGL Shading Language (GLSL)
    //   is supported by the version of WebGL.
    //
    //     WebGL 1.0 supports "GLSL ES 1.0", while
    //     WebGL 2.0 supports "GLSL ES 3.0"
    //
    //  which requires starting the shader with a "#version" line.
    //
    var version = gl.getParameter(gl.SHADING_LANGUAGE_VERSION)
    var es300 = /GLSL ES 3.0/.test(version);

    // Create a local function to create and compile the shader's
    //   source.  The function also inserts the required #version
    //   string if necessary.
    var CompileShader = function( type, src ) {

        // Validate the appropriate shader type, and set up some
        //   diagnostic information.
        var typeStr = undefined;
        var id = undefined;

        switch( type ) {
            case gl.VERTEX_SHADER:
                typeStr = "Vertex";
                id = vertexShaderId;
                break;

            case gl.FRAGMENT_SHADER:
                typeStr = "Fragment";
                id = fragmentShaderId;
                break;

            default:
                alert( "Invalid shader type passed to CompileShader()" );
                return -1;
            }

        // Determine if there's a valid GLSL ES 3.0 version string
        //   already present in the shader, and if not, insert a
        //   version string at the start of the shader.
        var versionRegExp = /\s*#version\s+300\s+es/;

        if ( es300 && !versionRegExp.test(src) ) {
            src = "#version 300 es\n" + src;
        }

        // Create a WebGL shader object, load its source, and
        //   compile it.  If the compilation fails, retrieve and
        //   display the error log and shader source, and indicate
        //   the operation failed by passing back a "-1", which is
        //   an invalid shader id (they must be greater than zero)
        var shader = gl.createShader( type );
        gl.shaderSource( shader, src );
        gl.compileShader( shader );
        if ( !gl.getShaderParameter(shader, gl.COMPILE_STATUS) ) {

            var msg = typeStr + " shader '" + id + "' failed to compile." +
                "  The error log is:\n\n" + gl.getShaderInfoLog( shader ) +
                "\n-----------------------------------------\n\n" + src;
            //alert( msg );
            console.error(msg);

            return -1;
        }

        return shader;
    };

    // Attempt to retrieve the vertex shader's source, and compile it.
    var vertElem = document.getElementById( vertexShaderId );
    if ( !vertElem ) {
        alert( "Unable to load vertex shader '" + vertexShaderId + "'");
        return -1;
    }

    var vertShdr = CompileShader( gl.VERTEX_SHADER, vertElem.textContent );
    if ( vertShdr < 0 ) {
        return -1;
    }

    // Do the identical operation for the fragment shader, verifying that
    //   the fragment shader also contains an appropriate precision specification
    var fragElem = document.getElementById( fragmentShaderId );
    if ( !fragElem ) {
        alert( "Unable to load fragment shader '" + fragmentShaderId + "'" );
        return -1;
    }

    // This test is fairly rudimentary as it only checks on the precision
    //   keyword being present in the shader.  
    var src = fragElem.textContent;
    if ( !/precision/.test(src) ) { 
        src = "precision highp float;" + src;
    }

    var fragShdr = CompileShader( gl.FRAGMENT_SHADER, src );
    if ( fragShdr < 0 ) {
        return -1;
    }

    // Finally, compose the shader program pipeline by attaching shaders
    //   and linking the program.  If the link succeeds, we return the
    //   shader program, otherwise, we report an error and return a "-1"
    //   (an invalid shader program value).
    var program = gl.createProgram();
    gl.attachShader( program, vertShdr );
    gl.attachShader( program, fragShdr );
    gl.linkProgram( program );

    if ( !gl.getProgramParameter(program, gl.LINK_STATUS) ) {
        var msg = "Shader program failed to link.  The error log is:\n\n"
            + gl.getProgramInfoLog( program );
        alert( msg );
        return -1;
    }

    return program;
}
