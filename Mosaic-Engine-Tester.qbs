import qbs
import qbs.Process
import qbs.File
import qbs.FileInfo
import qbs.TextFile
import "../../../libs/openFrameworksCompiled/project/qtcreator/ofApp.qbs" as ofApp

Project{
    property string of_root: '../../..'

    ofApp {
        name: { return FileInfo.baseName(sourceDirectory) }

        files: [
            'src/main.cpp',
            'src/mosaicNode.cpp',
            'src/mosaicNode.h',
            'src/ofApp.cpp',
            'src/ofApp.h',
            'src/ofxVPHasUid.cpp',
            'src/ofxVPHasUid.h',
            'src/mosaicParameters.cpp',
            'src/mosaicParameters.h',
            'src/imgui/imgui_controls.cpp',
            'src/imgui/imgui_controls.h',
            'src/imgui/imgui_helpers.h',
            'src/imgui/imgui_node_canvas.cpp',
            'src/imgui/imgui_node_canvas.h',
            'src/imgui/imgui_plot.cpp',
            'src/imgui/imgui_plot.h',
            'src/imgui/imgui_profiler.h',
            'src/imgui/imgui_stdlib.cpp',
            'src/imgui/imgui_stdlib.h',
            //
            //'tracy/*.hpp',
            //'tracy/*.cpp',
            //'tracy/client/*',
            //'tracy/*.cpp',
            'tracy/Tracy.hpp',
            'tracy/TracyClient.cpp',
        ]

        of.addons: [
            'ofxImGui',
            'ofxXmlSettings',
            'ofxInfiniteCanvas',
        ]

        // additional flags for the project. the of module sets some
        // flags by default to add the core libraries, search paths...
        // this flags can be augmented through the following properties:
        of.pkgConfigs: []       // list of additional system pkgs to include
        of.includePaths: ['src/imgui', 'tracy']     // include search paths
        of.cFlags: []           // flags passed to the c compiler
        of.cxxFlags: []         // flags passed to the c++ compiler
        of.linkerFlags: []      // flags passed to the linker
        of.defines: [ // defines are passed as -D to the compiler
            //"TRACY_ENABLE", // Tracy runs better in Release context, but it's only for debugging
            // Getting started with tracy
            // - Pull the submodule
            // - Install the profiler dependencies : (osx) brew install capstone freetype2 glfw3 gtk+3
            // - Build the profiler : cd /Mosaic-Engine-Tester/tracy/profiler/build/unix && make release
            // More information on Tracy can be found in it's manual, available on their releases github page.
            "OFXIMGUI_DEBUG",
        ]
        // and can be checked with #ifdef or #if in the code
        of.frameworks: []       // osx only, additional frameworks to link with the project
        of.staticLibraries: []  // static libraries
        of.dynamicLibraries: [] // dynamic libraries

        // other flags can be set through the cpp module: http://doc.qt.io/qbs/cpp-module.html
        // eg: this will enable ccache when compiling
        //
        // cpp.compilerWrapper: 'ccache'

        Depends{
            name: "cpp"
        }

        // common rules that parse the include search paths, core libraries...
        Depends{
            name: "of"
        }

        // dependency with the OF library
        Depends{
            name: "openFrameworks"
        }
    }

    property bool makeOF: true  // use makfiles to compile the OF library
    // will compile OF only once for all your projects
    // otherwise compiled per project with qbs


    property bool precompileOfMain: false  // precompile ofMain.h
    // faster to recompile when including ofMain.h
    // but might use a lot of space per project

    references: [FileInfo.joinPaths(of_root, "/libs/openFrameworksCompiled/project/qtcreator/openFrameworks.qbs")]
}
