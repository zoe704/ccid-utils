/*
 * This file is part of ccid-utils
 * Copyright (c) 2008 Gianni Tedesco <gianni@scaramanga.co.uk>
 * Released under the terms of the GNU GPL version 3
*/
#ifndef _EMV_H
#define _EMV_H

typedef struct _emv *emv_t;
typedef struct _emv_app *emv_app_t;

_public emv_t emv_init(chipcard_t cc);
_public void emv_fini(emv_t emv);

#endif /* _EMV_H */