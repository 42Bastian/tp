#ifndef _LOADSAVE_H_
#define _LOADSAVE_H_
long LoadFile(char *fn,uint8_t **addr);
long SaveFile(char *fn,uint8_t * addr,uint32_t len,uint8_t *head,uint32_t headlen);

#endif
