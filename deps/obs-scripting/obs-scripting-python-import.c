/******************************************************************************
    Copyright (C) 2017 by Hugh Bailey <jim@obsproject.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <util/dstr.h>
#include <util/platform.h>

#define NO_REDEFS
#include "obs-scripting-python-import.h"
#include "obs-scripting-config.h"

#ifdef _MSC_VER
#pragma warning(disable : 4152)
#endif

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define F_OK 0
#define access _access
#define VERSION_PATTERN "%d%d"
#define FILE_PATTERN "python%s.dll"
#define PATH_MAX MAX_PATH
#elif __APPLE__
#define VERSION_PATTERN "%d.%d"
#define FILE_PATTERN "Python.framework/Versions/Current/lib/libpython%s.dylib"
#endif

#define PY_MAJOR_VERSION_MAX 3
#define PY_MINOR_VERSION_MAX 10

bool import_python(const char *python_path)
{
	struct dstr lib_path;
	bool success = false;
	void *lib = NULL;

	if (!python_path)
		python_path = "";

	dstr_init_copy(&lib_path, python_path);
	dstr_replace(&lib_path, "\\", "/");
	if (!dstr_is_empty(&lib_path)) {
		dstr_cat(&lib_path, "/");
	}

	struct dstr lib_candidate_path;
	dstr_init_copy(&lib_candidate_path, lib_path.array);

	char cur_version[5];
	char next_version[5];

	char temp[PATH_MAX];

	sprintf(cur_version, VERSION_PATTERN, PY_MAJOR_VERSION_MAX,
		PY_MINOR_VERSION_MAX);
	sprintf(temp, FILE_PATTERN, cur_version);

	dstr_cat(&lib_candidate_path, temp);

	int minor_version = PY_MINOR_VERSION_MAX;
	do {
		if (access(lib_candidate_path.array, F_OK) == 0) {
			lib = os_dlopen(lib_candidate_path.array);
		}

		if (lib) {
			break;
		}

		sprintf(cur_version, VERSION_PATTERN, PY_MAJOR_VERSION_MAX,
			minor_version);
		sprintf(next_version, VERSION_PATTERN, PY_MAJOR_VERSION_MAX,
			--minor_version);
		dstr_replace(&lib_candidate_path, cur_version, next_version);
	} while (minor_version > 5);

	dstr_free(&lib_candidate_path);

	if (!lib) {
		blog(LOG_WARNING, "[Python] Could not load library: %s",
		     lib_path.array);
		goto fail;
	}

#define IMPORT_FUNC(x)                                                     \
	do {                                                               \
		Import_##x = os_dlsym(lib, #x);                            \
		if (!Import_##x) {                                         \
			blog(LOG_WARNING, "[Python] Failed to import: %s", \
			     #x);                                          \
			goto fail;                                         \
		}                                                          \
	} while (false)

	IMPORT_FUNC(PyType_Ready);
	IMPORT_FUNC(PyObject_GenericGetAttr);
	IMPORT_FUNC(PyObject_IsTrue);
	IMPORT_FUNC(Py_DecRef);
	IMPORT_FUNC(PyObject_Malloc);
	IMPORT_FUNC(PyObject_Free);
	IMPORT_FUNC(PyObject_Init);
	IMPORT_FUNC(PyUnicode_FromFormat);
	IMPORT_FUNC(PyUnicode_Concat);
	IMPORT_FUNC(PyLong_FromVoidPtr);
	IMPORT_FUNC(PyLong_FromLong);
	IMPORT_FUNC(PyBool_FromLong);
	IMPORT_FUNC(PyGILState_Ensure);
	IMPORT_FUNC(PyGILState_GetThisThreadState);
	IMPORT_FUNC(PyErr_SetString);
	IMPORT_FUNC(PyErr_Occurred);
	IMPORT_FUNC(PyErr_Fetch);
	IMPORT_FUNC(PyErr_Restore);
	IMPORT_FUNC(PyErr_WriteUnraisable);
	IMPORT_FUNC(PyArg_UnpackTuple);
	IMPORT_FUNC(Py_BuildValue);
	IMPORT_FUNC(PyRun_SimpleStringFlags);
	IMPORT_FUNC(PyErr_Print);
	IMPORT_FUNC(Py_SetPythonHome);
	IMPORT_FUNC(Py_Initialize);
	IMPORT_FUNC(Py_Finalize);
	IMPORT_FUNC(Py_IsInitialized);
	IMPORT_FUNC(PyEval_InitThreads);
	IMPORT_FUNC(PyEval_ThreadsInitialized);
	IMPORT_FUNC(PyEval_ReleaseThread);
	IMPORT_FUNC(PySys_SetArgv);
	IMPORT_FUNC(PyImport_ImportModule);
	IMPORT_FUNC(PyObject_CallFunctionObjArgs);
	IMPORT_FUNC(_Py_NotImplementedStruct);
	IMPORT_FUNC(PyExc_TypeError);
	IMPORT_FUNC(PyExc_RuntimeError);
	IMPORT_FUNC(PyObject_GetAttr);
	IMPORT_FUNC(PyUnicode_FromString);
	IMPORT_FUNC(PyDict_New);
	IMPORT_FUNC(PyDict_GetItemString);
	IMPORT_FUNC(PyDict_SetItemString);
	IMPORT_FUNC(PyCFunction_NewEx);
#if PY_VERSION_HEX > 0x030900b0
	IMPORT_FUNC(PyCMethod_New);
#endif
	IMPORT_FUNC(PyModule_GetDict);
	IMPORT_FUNC(PyModule_GetNameObject);
	IMPORT_FUNC(PyModule_AddObject);
	IMPORT_FUNC(PyModule_AddStringConstant);
	IMPORT_FUNC(PyImport_Import);
	IMPORT_FUNC(PyObject_CallObject);
	IMPORT_FUNC(_Py_FalseStruct);
	IMPORT_FUNC(_Py_TrueStruct);
	IMPORT_FUNC(PyGILState_Release);
	IMPORT_FUNC(PyList_Append);
	IMPORT_FUNC(PySys_GetObject);
	IMPORT_FUNC(PyImport_ReloadModule);
	IMPORT_FUNC(PyObject_GetAttrString);
	IMPORT_FUNC(PyCapsule_New);
	IMPORT_FUNC(PyCapsule_GetPointer);
	IMPORT_FUNC(PyArg_ParseTuple);
	IMPORT_FUNC(PyFunction_Type);
	IMPORT_FUNC(PyObject_SetAttr);
	IMPORT_FUNC(_PyObject_New);
	IMPORT_FUNC(PyCapsule_Import);
	IMPORT_FUNC(PyErr_Clear);
	IMPORT_FUNC(PyObject_Call);
	IMPORT_FUNC(PyList_New);
	IMPORT_FUNC(PyList_Size);
	IMPORT_FUNC(PyList_GetItem);
	IMPORT_FUNC(PyUnicode_AsUTF8String);
	IMPORT_FUNC(PyLong_FromUnsignedLongLong);
	IMPORT_FUNC(PyArg_VaParse);
	IMPORT_FUNC(_Py_NoneStruct);
	IMPORT_FUNC(PyTuple_New);

#if defined(Py_DEBUG) || PY_VERSION_HEX >= 0x030900b0
	IMPORT_FUNC(_Py_Dealloc);
#endif
#if PY_VERSION_HEX >= 0x030900b0
	IMPORT_FUNC(PyType_GetFlags);
#endif

#undef IMPORT_FUNC
	success = true;

fail:
	if (!success && lib)
		os_dlclose(lib);

	dstr_free(&lib_path);

	return success;
}
