#ifndef __USER_LM_H__
#define __USER_LM_H__

#include <julius/jfunc.h>

void init_user_lms();
LOGPROB unigram(WORD_INFO *winfo, WORD_ID w, LOGPROB ngram_prob);
LOGPROB bigram(WORD_INFO *winfo, WORD_ID context, WORD_ID w, LOGPROB ngram_prob);
LOGPROB language_model(WORD_INFO *winfo, WORD_ID *contexts, int context_len, WORD_ID w, LOGPROB ngram_prob);
boolean opt_user_lm_py_location(Jconf *jconf, char *arg[], int argnum);
void free_user_lms();

#endif // __USER_LM_H__
