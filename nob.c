#include <stdio.h>
#define NOB_IMPLEMENTATION
#include "nob.h"

#define BIN_FOLDER "bin/"
#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define RSOURCE "raylib/src/"
#define GLFWIN "-I./raylib/src/external/glfw/include"

const char *rayfiles[] = {
	"raudio",  "rcore", "rglfw",	 "rmodels",
	"rshapes", "rtext", "rtextures", "utils",
};

bool build_raylib(Nob_Cmd *cmd)
{
	Nob_File_Paths object_files = { 0 };
	Nob_Procs procs = { 0 };
	char *build_path = BUILD_FOLDER "raylib/";
	if (!nob_mkdir_if_not_exists("./build/raylib")) {
		return false;
	}

	for (size_t i = 0; i < NOB_ARRAY_LEN(rayfiles); ++i) {
		const char *input_path =
			nob_temp_sprintf(RSOURCE "%s.c", rayfiles[i]);
		const char *output_path =
			nob_temp_sprintf("%s%s.o", build_path, rayfiles[i]);

		nob_da_append(&object_files, output_path);

		if (nob_needs_rebuild(output_path, &input_path, 1)) {
			nob_cmd_append(cmd, "cc", "-ggdb", "-DPLATFORM_DESKTOP",
				       "-D_GLFW_X11", "-I./raylib/src/", GLFWIN,
				       "-c", input_path, "-o", output_path);
			if (!nob_cmd_run(cmd, .async = &procs))
				return false;
		}
	}

	nob_temp_reset();
	nob_procs_wait(procs);
	return true;
}

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);

	if (!nob_mkdir_if_not_exists(BIN_FOLDER))
		return 1;
	if (!nob_mkdir_if_not_exists(BUILD_FOLDER))
		return 1;

	Nob_Cmd cmd = { 0 };
	if (!build_raylib(&cmd)) {
		return 1;
	}

	nob_cc(&cmd);
	nob_cc_output(&cmd, BIN_FOLDER "2to11");
	nob_cc_inputs(&cmd, SRC_FOLDER "main.c");
	nob_cc_inputs(&cmd, SRC_FOLDER "funny_math.c");
	nob_cc_inputs(&cmd, SRC_FOLDER "save.c");
	nob_cc_inputs(&cmd, SRC_FOLDER "platform/linux/getAppStoragePath.c");
	nob_cmd_append(&cmd, "-DPLATFORM_DESKTOP", "-D_GLFW_X11");
	nob_cmd_append(&cmd, GLFWIN);
	nob_cmd_append(&cmd, "-I./" RSOURCE);

	for (int i = 0; i < NOB_ARRAY_LEN(rayfiles); i++) {
		const char *objfile = nob_temp_sprintf(
			"%s%s.o", BUILD_FOLDER "raylib/", rayfiles[i]);
		nob_cmd_append(&cmd, objfile);
	}
	nob_temp_reset();
	nob_cmd_append(&cmd, "-lm");

	if (!nob_cmd_run(&cmd))
		return 1;

	return 0;
}
