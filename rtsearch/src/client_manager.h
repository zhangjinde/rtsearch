#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

int cm_start();
int cm_send2client(uint64_t peerid,void *buf,uint32_t len);
int cm_joinuser(uint64_t peerid,uint64_t uid);
int cm_bindgs(uint64_t peerid,uint64_t gspeerid);
int cm_getidbycid(uint64_t peerid,uint64_t *gspeerid,uint64_t *accountid);
void* cm_malloc(uint32_t size);
int cm_free(void *memory);
int cm_rmclient(uint64_t peerid);
int cm_stop();
int cm_destroy();

#ifdef __cplusplus
}
#endif

#endif

