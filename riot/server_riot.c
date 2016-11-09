/*
// Copyright (c) 2016 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#include "led.h"
#include "oc_api.h"
#include "port/oc_clock.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>

#define MAX_TEXT_SIZE 64

static char current_text[MAX_TEXT_SIZE];

static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
static int quit = 0;

static void
set_device_custom_property(void *data)
{
  oc_set_custom_device_property(purpose, "Smart text scroller");
}

static void
app_init(void)
{
  oc_init_platform("Intel", NULL, NULL);

  oc_add_device("/oic/d", "oic.d.text.scroller", "Text Scroller", "1.0", "1.0",
                set_device_custom_property, NULL);

#ifdef OC_SECURITY
  oc_storage_config("./creds");
#endif /* OC_SECURITY */
}

static void
post_switch(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
  PRINT("POST_text:\n");
  oc_rep_t *rep = request->request_payload;
  size_t length;
  char *chars;

  while (rep != NULL) {
    PRINT("key: %s ", oc_string(rep->name));
    switch (rep->type) {
    case STRING:
      chars = oc_string(rep->value_string);
      length = strlen(chars);
      if (length > MAX_TEXT_SIZE - 1)
        length = MAX_TEXT_SIZE - 1;
      memcpy(current_text, chars, length);
      current_text[length] = '\0';
      PRINT("value: %s\n", current_text);
      break;
    default:
      oc_send_response(request, OC_STATUS_BAD_REQUEST);
      return;
      break;
    }
    rep = rep->next;
  }
  oc_send_response(request, OC_STATUS_CHANGED);
}

static void
put_switch(oc_request_t *request, oc_interface_mask_t interface,
           void *user_data)
{
  PRINT("PUT_text:\n");
  post_switch(request, interface, user_data);
}

static void
register_resources(void)
{
  oc_resource_t *res = oc_new_resource("/TextScrollerResURI", 1, 0);
  oc_resource_bind_resource_type(res, "oic.r.text.scroller");
  oc_resource_bind_resource_interface(res, OC_IF_RW);
  oc_resource_set_default_interface(res, OC_IF_RW);

#ifdef OC_SECURITY
  oc_resource_make_secure(res);
#endif

  oc_resource_set_discoverable(res, true);
//  oc_resource_set_periodic_observable(res, 1);
  oc_resource_set_request_handler(res, OC_POST, post_switch, NULL);
  oc_resource_set_request_handler(res, OC_PUT, put_switch, NULL);

  oc_add_resource(res);
}

static void
signal_event_loop(void)
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
}

static char _oc_main_stack[THREAD_STACKSIZE_MAIN];

void *
oc_main_thread(void *arg)
{
  (void)arg;

  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .register_resources = register_resources };

  if (oc_main_init(&handler) < 0) {
    PRINT("server_riot: failed to initialize stack\n");
    return NULL;
  }

  oc_clock_time_t next_event;
  while (!quit) {
    next_event = oc_main_poll();
    mutex_lock(&mutex);
    if (next_event == 0) {
      pthread_cond_wait(&cv, &mutex);
    } else {
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    mutex_unlock(&mutex);
  }

  oc_main_shutdown();

  return NULL;
}

int
main(void)
{
  thread_create(_oc_main_stack, sizeof(_oc_main_stack), 2, 0, oc_main_thread,
                NULL, "OCF event thread");

  fgetc(stdin);

  quit = 1;
  signal_event_loop();

  return 0;
}
