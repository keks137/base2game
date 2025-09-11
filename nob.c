// This is your build script. You only need to "bootstrap" it once with `cc -o nob nob.c`
// (you can call the executable whatever actually) or `cl nob.c` on MSVC. After that every
// time you run the `nob` executable if it detects that you modifed nob.c it will rebuild
// itself automatically thanks to NOB_GO_REBUILD_URSELF (see below)

// nob.h is an stb-style library https://github.com/nothings/stb/blob/master/docs/stb_howto.txt
// What that means is that it's a single file that acts both like .c and .h files, but by default
// when you include it, it acts only as .h. To make it include implementations of the functions
// you must define NOB_IMPLEMENTATION macro. This is done to give you full control over where
// the implementations go.
#define NOB_IMPLEMENTATION

// Always keep a copy of nob.h in your repo. One of my main pet peeves with build systems like CMake
// and Autotools is that the codebases that use them naturally rot. That is if you do not actively update
// your build scripts, they may not work with the latest version of the build tools. Here we basically
// include the entirety of the source code of the tool along with the code base. It will never get
// outdated (unless you got no standard compliant C compiler lying around, but at that point why are
// you trying to build a C project?)
//
// (In these examples we actually symlinking nob.h, but this is to keep nob.h-s synced among all the
// examples)
#include "nob.h"

// Some folder paths that we use throughout the build process.
#define BIN_FOLDER "bin/"
#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define RAYLIB_V "raylib-5.5_linux_amd64"

int main(int argc, char **argv)
{
	// This line enables the self-rebuilding. It detects when nob.c is updated and auto rebuilds it then
	// runs it again.
	NOB_GO_REBUILD_URSELF(argc, argv);

	// It's better to keep all the building artifacts in a separate build folder. Let's create it if it
	// does not exist yet.
	//
	// Majority of the nob command return bool which indicates whether operation has failed or not (true -
	// success, false - failure). If the operation returned false you don't need to log anything, the
	// convention is usually that the function logs what happened to itself. Just do
	// `if (!nob_function()) return;`
	if (!nob_mkdir_if_not_exists(BIN_FOLDER))
		return 1;
	if (!nob_mkdir_if_not_exists(BUILD_FOLDER))
		return 1;

	// The working horse of nob is the Nob_Cmd structure. It's a Dynamic Array of strings which represent
	// command line that you want to execute.
	Nob_Cmd cmd = { 0 };

	if (access(BUILD_FOLDER RAYLIB_V "/README.md", F_OK) != 0) {
		nob_cmd_append(&cmd, "wget");
		nob_cmd_append(&cmd, "-P");
		nob_cmd_append(&cmd, "build");
		nob_cmd_append(
			&cmd,
			"https://github.com/raysan5/raylib/releases/download/5.5/" RAYLIB_V
			".tar.gz");
		nob_cmd_append(&cmd, "-O");
		nob_cmd_append(&cmd, BUILD_FOLDER RAYLIB_V ".tar.gz");
		if (!nob_cmd_run(&cmd))
			return 2;

		nob_cmd_append(&cmd, "tar");
		nob_cmd_append(&cmd, "-xzf");
		nob_cmd_append(&cmd, BUILD_FOLDER RAYLIB_V ".tar.gz");
		nob_cmd_append(&cmd, "-C");
		nob_cmd_append(&cmd, BUILD_FOLDER);

		if (!nob_cmd_run(&cmd))
			return 2;
	}
	// nob.h ships with a bunch of nob_cc_* macros that try abstract away the specific compiler.
	// They are verify basic and not particularly flexible, but you can redefine them if you need to
	// or not use them at all and create your own abstraction on top of Nob_Cmd.
	nob_cc(&cmd);
	nob_cc_flags(&cmd);
	nob_cc_output(&cmd, BIN_FOLDER "2to11");
	nob_cc_inputs(&cmd, SRC_FOLDER "main.c");
	nob_cc_inputs(&cmd, SRC_FOLDER "funny_math.c");
	nob_cc_inputs(&cmd, SRC_FOLDER "save.c");
	nob_cc_inputs(&cmd, SRC_FOLDER "platform/linux/getAppStoragePath.c");
	nob_cmd_append(&cmd, "-Ibuild/" RAYLIB_V "/include");
	//nob_cmd_append(&cmd, "-Lbuild/" RAYLIB_V "/lib");
	//nob_cmd_append(&cmd, "-lraylib");
	nob_cmd_append(&cmd,  BUILD_FOLDER RAYLIB_V "/lib/libraylib.a");
	nob_cc_inputs(&cmd,"-lGL");
	nob_cc_inputs(&cmd,"-lm");
	nob_cc_inputs(&cmd,"-lpthread");
	nob_cc_inputs(&cmd,"-ldl");
	nob_cc_inputs(&cmd,"-lrt" );
	if (!nob_cmd_run(&cmd))
		return 1;

	return 0;
}
