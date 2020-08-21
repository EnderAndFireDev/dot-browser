/*
Copyright (c) 2019 The Khronos Group Inc.
Use of this source code is governed by an MIT-style license that can be
found in the LICENSE.txt file.
*/

function generateTest(internalFormat, pixelFormat, pixelType, prologue, resourcePath, defaultContextVersion) {
    var wtu = WebGLTestUtils;
    var tiu = TexImageUtils;
    var gl = null;
    var successfullyParsed = false;

    function init()
    {
        description('Verify texImage2D and texSubImage2D code paths taking ImageBitmap created from an HTMLImageElement (' + internalFormat + '/' + pixelFormat + '/' + pixelType + ')');

        if(!window.createImageBitmap || !window.ImageBitmap) {
            finishTest();
            return;
        }

        // Set the default context version while still allowing the webglVersion URL query string to override it.
        wtu.setDefault3DContextVersion(defaultContextVersion);
        gl = wtu.create3DContext("example");

        if (!prologue(gl)) {
            finishTest();
            return;
        }

        gl.clearColor(0,0,0,1);
        gl.clearDepth(1);

        var image = new Image();
        image.onload = function() {
            bufferedLogToConsole("Source image has been loaded");
            runImageBitmapTest(image, 0.5, internalFormat, pixelFormat, pixelType, gl, tiu, wtu, false)
            .then(() => {
                finishTest();
            });
        }
        image.src = resourcePath + "red-green-semi-transparent.png";
    }

    return init;
}