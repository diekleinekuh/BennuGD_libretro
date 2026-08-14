/*
  libco / scefiber (Vita)
  Based on libretro-common libco/scefiber.c (public domain).

  Upstream bug: co_switch() assigned co_active_ before testing it, so the
  first switch always took sceFiberSwitch instead of sceFiberRun. Fibers that
  were never Run never execute — RetroArch loads the core, then shows a
  permanent black screen with no freeze.

  Also drop thread_local: Vita TLS is unreliable for this single-threaded
  libco path; a plain static matches how the host thread uses the fiber API.
*/

#define LIBCO_C
#include <libco.h>
#include <stdlib.h>
#include <psp2/sysmodule.h>

#ifdef __cplusplus
extern "C" {
#endif

static cothread_t co_active_ = 0;

typedef struct SceFiber
{
   char reserved[128];
} SceFiber __attribute__((aligned(8)));

/* Forward declarations */
int32_t _sceFiberInitializeImpl(SceFiber *fiber, char *name, void *entry,
      uint32_t argOnInitialize, void *addrContext, int32_t sizeContext,
      void *params);
int32_t sceFiberFinalize(SceFiber *fiber);
int32_t sceFiberRun(SceFiber *fiber, uint32_t argOnRunTo, uint32_t *argOnRun);
int32_t sceFiberSwitch(SceFiber *fiber, uint32_t argOnRunTo, uint32_t *argOnRun);
int32_t sceFiberReturnToThread(uint32_t argOnReturn, uint32_t *argOnRun);

static void co_thunk(uint32_t argOnInitialize, uint32_t argOnRun)
{
   ((void (*)(void))argOnInitialize)();
}

cothread_t co_active(void)
{
   if (!co_active_)
   {
      sceSysmoduleLoadModule(SCE_SYSMODULE_FIBER);
      co_active_ = (cothread_t)1;
   }
   return co_active_;
}

cothread_t co_create(unsigned int heapsize, void (*coentry)(void))
{
   int ret;
   SceFiber *tail_fiber = (SceFiber *)malloc(sizeof(SceFiber));
   char *m_ctxbuf       = (char *)malloc(heapsize * sizeof(char));

   if (!tail_fiber || !m_ctxbuf)
   {
      free(tail_fiber);
      free(m_ctxbuf);
      return (cothread_t)0;
   }

   if (!co_active_)
   {
      sceSysmoduleLoadModule(SCE_SYSMODULE_FIBER);
      co_active_ = (cothread_t)1;
   }

   if ((ret = _sceFiberInitializeImpl(
            tail_fiber, "tailFiber", co_thunk,
            (uint32_t)coentry, (void *)m_ctxbuf, heapsize, NULL)) == 0)
      return (cothread_t)tail_fiber;

   free(m_ctxbuf);
   free(tail_fiber);
   return (cothread_t)0;
}

void co_delete(cothread_t cothread)
{
   if (cothread && cothread != (cothread_t)1)
      sceFiberFinalize((SceFiber *)cothread);
}

void co_switch(cothread_t cothread)
{
   uint32_t argOnReturn = 0;

   if (cothread == (cothread_t)1)
   {
      co_active_ = cothread;
      sceFiberReturnToThread(0, NULL);
      return;
   }

   {
      SceFiber *theFiber = (SceFiber *)cothread;

      /* Must test the *current* context before updating co_active_.
       * From the host thread (1) the fiber must be started with Run;
       * subsequent resumes use Switch. */
      if (co_active_ == (cothread_t)1)
      {
         co_active_ = cothread;
         sceFiberRun(theFiber, 0, &argOnReturn);
      }
      else
      {
         co_active_ = cothread;
         sceFiberSwitch(theFiber, 0, &argOnReturn);
      }
   }
}

#ifdef __cplusplus
}
#endif
