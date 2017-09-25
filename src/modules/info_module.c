#include <string.h>
#include <sys/utsname.h>

#include "monikor.h"

#define AWS_INSTANCE_ID_URL "http://169.254.169.254/latest/meta-data/instance-id"
#define GCE_INSTANCE_ID_URL "http://metadata.google.internal/computeMetadata/v1/instance/id"
#define CLOUD_HTTPGET_TIMEOUT 15

static int poll_uname(monikor_t *mon, struct timeval *clock) {
  int n = 0;
  struct utsname name;

  if (uname(&name) == -1)
    return 0;
  n += monikor_metric_push(mon, monikor_metric_string(
    "sysinfo.uname.kernel", clock, name.sysname));
  n += monikor_metric_push(mon, monikor_metric_string(
    "sysinfo.uname.kernel_version", clock, name.release));
  n += monikor_metric_push(mon, monikor_metric_string(
    "sysinfo.uname.arch", clock, name.machine));
  return n;
}


/*
** Since there seems to be no reliable easy way to determine the OS name and version (each
** distribution has its own file), it seems to be reasonable to rely on /etc/os-release wich
** aims at standardizing this. It won't work for Centos6 and Debian Squeeze though
*/
static const struct {
  char *osr_name;
  char *name;
} infos[] = {
  {"ID=", "sysinfo.os.distribution"},
  {"VERSION_ID=", "sysinfo.os.version"},
  {NULL, NULL}
};

static int poll_os_info(char *value, monikor_t *mon, struct timeval *clock) {
  char *end;

  for (size_t i = 0; infos[i].name; i++) {
    size_t len = strlen(infos[i].osr_name);
    if (strncmp(value, infos[i].osr_name, len))
      continue;
    value += len;
    if ((*value == '"') && (end = strrchr(value, '"'))) {
      value++;
      *end = 0;
    }
    return monikor_metric_push(mon, monikor_metric_string(
      infos[i].name, clock, value
    ));
  }
  return 0;
}

static int poll_os(monikor_t *mon, struct timeval *clock) {
  char *osrelease;
  char *value;
  char *lasts = NULL;
  int n = 0;

  if (!(osrelease = monikor_read_file("/etc/os-release")))
    return 0;
  for (value = strtok_r(osrelease, "\n", &lasts);
       value;
       value = strtok_r(NULL, "\n", &lasts)) {
    n += poll_os_info(value, mon, clock);
  }
  free(osrelease);
  return n;
}

static int poll_uptime(monikor_t *mon, struct timeval *clock) {
  char *uptime;
  int n = 0;

  if (!(uptime = monikor_read_file("/proc/uptime")))
    return 0;
  n += monikor_metric_push(mon, monikor_metric_integer(
    "sysinfo.uptime", clock, strtoull(uptime, NULL, 10), 0));
  free(uptime);
  return n;
}

static void set_instance_id_as_hostid(monikor_t *mon, char *id) {
  static char instance_id[256] = "";

  if (strlen(id) && strcmp(id, instance_id)) {
    strncpy(instance_id, id, 255);
    instance_id[255] = 0;
    if (!mon->config->hostid)
      mon->config->hostid = instance_id;
  }
}

static void handle_cloud_instance_id(http_response_t *status, CURLcode result) {
  monikor_t *mon = (monikor_t *)status->userdata;
  struct timeval clock;

  if (result != CURLE_OK || status->code != 200) {
    monikor_log(LOG_ERR, "cannot get cloud instance ID");
    free(status->data);
    free(status);
    return;
  }
  gettimeofday(&clock, NULL);
  set_instance_id_as_hostid(mon, status->data);
  monikor_metric_push(mon, monikor_metric_string(
    "sysinfo.cloud.instance_id", &clock, status->data));
  free(status->data);
  free(status);
}

static struct curl_slist gce_curl_header = {
  .data = "Metadata-Flavor: Google",
  .next = NULL,
};

static int poll_cloud(monikor_t *mon, struct timeval *clock) {
  char *hyp_uuid;
  int n = 0;

  if (!(hyp_uuid = monikor_read_file("/sys/devices/virtual/dmi/id/bios_version")))
    return 0;
  if (strstr(hyp_uuid, "amazon")) {
    monikor_http_get(mon,
      AWS_INSTANCE_ID_URL, CLOUD_HTTPGET_TIMEOUT,
      &handle_cloud_instance_id, mon);
    n += monikor_metric_push(mon, monikor_metric_string(
      "sysinfo.cloud.provider", clock, "ec2"));
  } else if (strstr(hyp_uuid, "Google")) {
    monikor_http_get_with_headers(mon,
      GCE_INSTANCE_ID_URL, CLOUD_HTTPGET_TIMEOUT,
      &handle_cloud_instance_id, mon, &gce_curl_header);
    n += monikor_metric_push(mon, monikor_metric_string(
      "sysinfo.cloud.provider", clock, "gce"));
  }
  free(hyp_uuid);
  return n;
}

static int poll_tags(monikor_t *mon, struct timeval *clock) {
  (void)mon;
  (void)clock;
  return 0;
}

int monikor_info_module_poll(monikor_t *mon, void *data) {
  struct timeval clock;

  (void)data;
  gettimeofday(&clock, NULL);
  return poll_uptime(mon, &clock)
    + poll_uname(mon, &clock)
    + poll_os(mon, &clock)
    + poll_cloud(mon, &clock)
    + poll_tags(mon, &clock);
}
