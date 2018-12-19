#include "user_lm.h"
#include <Python.h>
#include <stdlib.h>

PyObject *main_module, *global_dict, *my_uni_py, *my_bi_py, *my_lm_py, *lm_init_py;
void init_user_lms()
{
    // https://www.linuxjournal.com/article/8497
    Py_Initialize();
    PyRun_SimpleString("import sys; print('Python interpreter up, version: {}'.format(sys.version_info))");
    // Py_Finalize();

    FILE *lm_funs_file = fopen("user_lm.py", "r");
    PyRun_SimpleFile(lm_funs_file, exp);
    fclose(lm_funs_file);

    // Get a reference to the main module
    // and global dictionary
    main_module = PyImport_AddModule("__main__");
    global_dict = PyModule_GetDict(main_module);

    // Extract a reference to the function "func_name"
    // from the global dictionary
    my_uni_py = PyDict_GetItemString(global_dict, "my_uni");
    my_bi_py = PyDict_GetItemString(global_dict, "my_bi");
    my_lm_py = PyDict_GetItemString(global_dict, "my_lm");
    lm_init_py = PyDict_GetItemString(global_dict, "lm_init");

    PyObject *init_status_py = PyObject_CallObject(lm_init_py, NULL);
    if(!PyBool_Check(init_status_py))
    {
        printf("Did not get bool from lm_init, exiting\n");
        exit(1);
    }
    int init_status = PyLong_AsLong(init_status_py);
    printf("Py LM init status: %d", init_status);
}

LOGPROB my_uni(WORD_INFO *winfo, WORD_ID w, LOGPROB ngram_prob){
  // fprintf(stderr, "my_uni call, wordid: %d, logprob: %f\n", w, ngram_prob);
  PyObject * ret_proba_py = PyObject_CallFunction(my_uni_py, "yif", winfo->woutput[w], w, ngram_prob);
  if (!PyFloat_CheckExact(ret_proba_py))
  {
    printf("Did not get float from my_uni_py, exiting\n");
    exit(1);
  }
  float ret_proba = PyFloat_AS_DOUBLE(ret_proba_py);

  // fprintf(stderr, "got %f\n", ret_proba);
  return ret_proba;
}

LOGPROB my_bi(WORD_INFO *winfo, WORD_ID context, WORD_ID w, LOGPROB ngram_prob){
  // fprintf(stderr, "my_bi call, wordid: %d, ncalls: %d, logprob: %f\n", w, my_bi_calls, ngram_prob);

  PyObject * ret_proba_py = PyObject_CallFunction(my_bi_py, "yyiif", winfo->woutput[context], winfo->woutput[w], context, w, ngram_prob);
  if (!PyFloat_CheckExact(ret_proba_py))
  {
    printf("Did not get float from my_bi_py, exiting\n");
    exit(1);
  }
  float ret_proba = PyFloat_AS_DOUBLE(ret_proba_py);

  // fprintf(stderr, "got %f\n", ret_proba);
  return ret_proba;
}

LOGPROB my_lm(WORD_INFO *winfo, WORD_ID *contexts, int context_len, WORD_ID w, LOGPROB ngram_prob){
  
  PyObject *context_ids_py = PyList_New(context_len);
  for(int i=0; i < context_len; ++i)
  {
      PyList_SetItem(context_ids_py, i, Py_BuildValue("i", contexts[i]));
  }

  PyObject *context_words_py = PyList_New(context_len);
  for(int i=0; i < context_len; ++i)
  {
      PyList_SetItem(context_words_py, i, Py_BuildValue("y", winfo->woutput[contexts[i]]));
  }

  PyObject * ret_proba_py = PyObject_CallFunction(my_lm_py, "OOyif", context_ids_py, context_words_py, winfo->woutput[w], w, ngram_prob);
  if (!PyFloat_CheckExact(ret_proba_py))
  {
    printf("Did not get float from my_lm_py, exiting\n");
    exit(1);
  }
  float ret_proba = PyFloat_AS_DOUBLE(ret_proba_py);

  return ret_proba;
}
