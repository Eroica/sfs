#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#ifdef _WIN32
	#include "Shlwapi.h"
#else
	#include <fcntl.h>
	#include <unistd.h>
	#include "mkpath.h"
#endif


static int l_isdir (lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	#ifdef _WIN32
		lua_pushboolean(L, PathIsDirectory(path) == FILE_ATTRIBUTE_DIRECTORY);
	#else
		int file = 0;
		if((file = open(path, O_RDONLY)) == -1)
			lua_pushboolean(L, false);

		struct stat f_stat;
		fstat(file, &f_stat);
		lua_pushboolean(L, S_ISDIR(f_stat.st_mode));
		close(file);
	#endif

	return 1;
}

static int l_exists (lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	#ifdef _WIN32
		lua_pushboolean(L, PathFileExists(path));
	#else
		int file = 0;
		if((file = open(path, O_RDONLY)) == -1)
			lua_pushboolean(L, false);

		struct stat f_stat;
		lua_pushboolean(L, fstat(file, &f_stat) == 0);
		close(file);
	#endif

	return 1;
}

static int l_mkpath (lua_State *L) {
	const char *dir_path = luaL_checkstring(L, 1);

	#ifdef _WIN32
		int status = SHCreateDirectoryEx(NULL, dir_path);
		lua_pushboolean(L, 1);
		return 1;
	#else
		int status = mkpath((char *) dir_path, 0755, 0755);
		if (status == 0) {
			lua_pushboolean(L, 1);
			return 1;
		} else {
			lua_pushnil(L); lua_pushstring(L, "failure");
			return 2;
		}
	#endif
}

static int l_mklink(lua_State *L) {
	const char *oldpath = luaL_checkstring(L, 1);
	const char *newpath = luaL_checkstring(L, 2);

	#ifdef _WIN32
		int symlink = lua_toboolean(L, 3);
		int is_dir = PathIsDirectory(oldpath) == FILE_ATTRIBUTE_DIRECTORY;
		if (is_dir && !symlink) {
				lua_pushnil(L); lua_pushstring(L, "hard links to directories are not supported on Windows");
				return 2;
		}

		int result = symlink ? CreateSymbolicLink(newpath, oldpath, is_dir)
							 : CreateHardLink(newpath, oldpath, NULL);
		if (result) {
				return pushresult(L, result, NULL);
		} else {
				lua_pushnil(L); lua_pushstring(L, symlink ? "make_link CreateSymbolicLink() failed"
														  : "make_link CreateHardLink() failed");
				return 2;
		}
	#else
		int result = (lua_toboolean(L, 3) ? symlink : link)(oldpath, newpath);
		if (result == 0) {
			lua_pushboolean(L, 1);
			return 1;
		} else {
			lua_pushboolean(L, 0); lua_pushstring(L, "link creation failed");
			return 2;
		}
	#endif
}


static const struct luaL_Reg sfs_lib [] = {
	{"is_dir", l_isdir},
	{"exists", l_exists},
	{"mkpath", l_mkpath},
	{"mklink", l_mklink},
	{NULL, NULL}
};


int __declspec(dllexport) luaopen_sfs (lua_State *L) {
	luaL_newlib(L, sfs_lib);
	return 1;
}
