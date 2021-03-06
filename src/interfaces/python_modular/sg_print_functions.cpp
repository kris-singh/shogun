#undef _POSIX_C_SOURCE
extern "C" {
#include <Python.h>
}

#include <shogun/io/SGIO.h>
#include <stdio.h>

void sg_global_print_message(FILE* target, const char* str)
{
	fprintf(target, "%s", str);
}

void sg_global_print_warning(FILE* target, const char* str)
{
	if (target==stdout)
	{
		PyGILState_STATE gil = PyGILState_Ensure();
		PyErr_Warn(NULL, str);
		PyGILState_Release(gil);
	}
	else
		fprintf(target, "%s", str);
}

void sg_global_print_error(FILE* target, const char* str)
{
	if (target==stdout)
	{
		PyGILState_STATE gil = PyGILState_Ensure();
		PyErr_SetString(PyExc_RuntimeError, str);
		PyGILState_Release(gil);
	}
	else
		fprintf(target, "%s", str);
}

void sg_global_cancel_computations(bool &delayed, bool &immediately)
{
	using namespace shogun;

	PyGILState_STATE gil = PyGILState_Ensure();
	if (PyErr_CheckSignals())
	{
		SG_SPRINT("\nImmediately return to matlab prompt / Prematurely finish computations / Do nothing (I/P/D)? ");
		char answer=fgetc(stdin);

		if (answer == 'I')
			immediately=true;
		else if (answer == 'P')
		{
			PyErr_Clear();
			delayed=true;
		}
		else
			SG_SPRINT("\n");
	}
	PyGILState_Release(gil);
}
