/*
 * (C) 2003-15 - ntop 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#define _GNU_SOURCE
#include <signal.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/ethernet.h>

#include <json-c/json.h>

#include <signal.h>
#include <pthread.h>



#include "pfring.h"
#include "pfutils.c"


#include "config.h"
#include "ndpi_api.h"

#define MAX_PKT_LEN 1536
#define MAX_SIZE 50
#define MY_PORT		9999
#define MAXBUF		1024
#define GTP_U_V1_PORT        2152
#define MAX_NDPI_FLOWS  200000000
#define MAX_IDLE_TIME           30000
#define IDLE_SCAN_PERIOD           10 
#define MAX_NUM_READER_THREADS     16

u_int32_t num_sent = 0;

static u_int32_t detection_tick_resolution = 1000;
static u_int8_t live_capture = 0, full_http_dissection = 1;
static u_int32_t size_id_struct = 0;
static u_int32_t size_flow_struct = 0;
static u_int8_t enable_protocol_guess = 1, verbose = 0, nDPI_traceLevel = 0, json_flag = 0;
static u_int32_t current_ndpi_memory = 0, max_ndpi_memory = 0;
static char *_protoFilePath   = NULL;
char *proto_app;

static u_int16_t decode_tunnels = 0;
static u_int16_t num_loops = 1;
static u_int8_t shutdown_app = 0, quiet_mode = 0;
static u_int8_t num_threads = 1;
#ifdef linux
static int core_affinity[MAX_NUM_READER_THREADS];
#endif
/* ****************************************************** */

static void setupDetection(u_int16_t thread_id);

#define IDLE_SCAN_BUDGET         1024

#define NUM_ROOTS                 512

struct ip_header {
#if BYTE_ORDER == LITTLE_ENDIAN
  u_int32_t	ihl:4,	/* header length */
    version:4;	/* version */
#else
  u_int32_t	version:4,	/* version */
    ihl:4;	/* header length */
#endif
  u_int8_t	tos;	/* type of service */
  u_int16_t	tot_len;	/* total length */
  u_int16_t	id;	/* identification */
  u_int16_t	frag_off;	/* fragment offset field */
  u_int8_t	ttl;	/* time to live */
  u_int8_t	protocol;	/* protocol */
  u_int16_t	check;	/* checksum */
  u_int32_t saddr, daddr;	/* source and dest address */
};

struct class_rule *class_rules;
size_t rules_size;
char *proto_name;
char *dev;
static u_int8_t undetected_flows_deleted = 0;

struct class_rule {
	u_int32_t ip;
	int ruleid;
	int tos;
};

static void debug_printf(u_int32_t protocol, void *id_struct,
			 ndpi_log_level_t log_level,
			 const char *format, ...) {
  va_list va_ap;
#ifndef WIN32
  struct tm result;
#endif

  if(log_level <= nDPI_traceLevel) {
    char buf[8192], out_buf[8192];
    char theDate[32];
    const char *extra_msg = "";
    time_t theTime = time(NULL);

    va_start (va_ap, format);

    if(log_level == NDPI_LOG_ERROR)
      extra_msg = "ERROR: ";
    else if(log_level == NDPI_LOG_TRACE)
      extra_msg = "TRACE: ";
    else
      extra_msg = "DEBUG: ";

    memset(buf, 0, sizeof(buf));
    strftime(theDate, 32, "%d/%b/%Y %H:%M:%S", localtime_r(&theTime,&result) );
    vsnprintf(buf, sizeof(buf)-1, format, va_ap);

    snprintf(out_buf, sizeof(out_buf), "%s %s%s", theDate, extra_msg, buf);
    printf("%s", out_buf);
    fflush(stdout);
  }

  va_end(va_ap);
}

/* ***************************************************** */

static void *malloc_wrapper(unsigned long size) {
  current_ndpi_memory += size;

  if(current_ndpi_memory > max_ndpi_memory)
    max_ndpi_memory = current_ndpi_memory;

  return malloc(size);
}

/* ***************************************************** */

static void free_wrapper(void *freeable) {
  free(freeable);
}

typedef struct ndpi_flow {
  u_int32_t lower_ip;
  u_int32_t upper_ip;
  u_int16_t lower_port;
  u_int16_t upper_port;
  u_int8_t detection_completed, protocol;
  u_int16_t vlan_id;
  struct ndpi_flow_struct *ndpi_flow;
  char lower_name[32], upper_name[32];

  u_int64_t last_seen;

  u_int64_t bytes;
  u_int32_t packets;

  // result only, not used for flow identification
  u_int32_t detected_protocol;

  char host_server_name[256];

  struct {
    char client_certificate[48], server_certificate[48];
  } ssl;

  void *src_id, *dst_id;
} ndpi_flow_t;

void resizeArray(size_t n)
{
    /* TODO: Handle reallocations errors. */
    class_rules = realloc(class_rules, n * sizeof *class_rules);
}

/**********************NDPI**********************/
struct thread_stats {
  u_int32_t guessed_flow_protocols;
  u_int64_t raw_packet_count;
  u_int64_t ip_packet_count;
  u_int64_t total_wire_bytes, total_ip_bytes, total_discarded_bytes;
  u_int64_t protocol_counter[NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS + 1];
  u_int64_t protocol_counter_bytes[NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS + 1];
  u_int32_t protocol_flows[NDPI_MAX_SUPPORTED_PROTOCOLS + NDPI_MAX_NUM_CUSTOM_PROTOCOLS + 1];
  u_int32_t ndpi_flow_count;
  u_int64_t tcp_count, udp_count;
  u_int64_t mpls_count, pppoe_count, vlan_count, fragmented_count;
  u_int64_t packet_len[6];
  u_int16_t max_packet_len;
};


struct reader_thread {
  struct ndpi_detection_module_struct *ndpi_struct;
  void *ndpi_flows_root[NUM_ROOTS];
  char _pcap_error_buffer[PCAP_ERRBUF_SIZE];
  pcap_t *_pcap_handle;
  u_int64_t last_time;
  u_int64_t last_idle_scan_time;
  u_int32_t idle_scan_idx;
  u_int32_t num_idle_flows;
  pthread_t pthread;
  int _pcap_datalink_type;

  /* TODO Add barrier */
  struct thread_stats stats;

  struct ndpi_flow *idle_flows[IDLE_SCAN_BUDGET];
};

static struct reader_thread ndpi_thread_info[10];
/* ***************************************************** */

static void free_ndpi_flow(struct ndpi_flow *flow) {
  if(flow->ndpi_flow) { ndpi_free_flow(flow->ndpi_flow); flow->ndpi_flow = NULL; }
  if(flow->src_id)    { ndpi_free(flow->src_id); flow->src_id = NULL;       }
  if(flow->dst_id)    { ndpi_free(flow->dst_id); flow->dst_id = NULL;       }
}

/* ***************************************************** */

static void ndpi_flow_freer(void *node) {
  struct ndpi_flow *flow = (struct ndpi_flow*)node;

  free_ndpi_flow(flow);
  ndpi_free(flow);
}

/* ***************************************************** */

static void node_print_unknown_proto_walker(const void *node, ndpi_VISIT which, int depth, void *user_data) {
  struct ndpi_flow *flow = *(struct ndpi_flow**)node;
  u_int16_t thread_id = *((u_int16_t*)user_data);

  if(flow->detected_protocol != 0 /* UNKNOWN */) return;

  if((which == ndpi_preorder) || (which == ndpi_leaf)) /* Avoid walking the same node multiple times */
    printFlow(thread_id, flow);
}

/* ***************************************************** */

static void node_print_known_proto_walker(const void *node, ndpi_VISIT which, int depth, void *user_data) {
  struct ndpi_flow *flow = *(struct ndpi_flow**)node;
  u_int16_t thread_id = *((u_int16_t*)user_data);

  if(flow->detected_protocol == 0 /* UNKNOWN */) return;

  if((which == ndpi_preorder) || (which == ndpi_leaf)) /* Avoid walking the same node multiple times */
    printFlow(thread_id, flow);
}

/* ***************************************************** */

static unsigned int node_guess_undetected_protocol(u_int16_t thread_id,
						   struct ndpi_flow *flow) {
  flow->detected_protocol = ndpi_guess_undetected_protocol(ndpi_thread_info[thread_id].ndpi_struct,
							   flow->protocol,
							   ntohl(flow->lower_ip),
							   ntohs(flow->lower_port),
							   ntohl(flow->upper_ip),
							   ntohs(flow->upper_port));
  // printf("Guess state: %u\n", flow->detected_protocol);
  if(flow->detected_protocol != 0)
    ndpi_thread_info[thread_id].stats.guessed_flow_protocols++;

  return flow->detected_protocol;
}

/* ***************************************************** */

static void node_proto_guess_walker(const void *node, ndpi_VISIT which, int depth, void *user_data) {
  struct ndpi_flow *flow = *(struct ndpi_flow **) node;
  u_int16_t thread_id = *((u_int16_t *) user_data);

#if 0
  printf("<%d>Walk on node %s (%p)\n",
	 depth,
	 which == preorder?"preorder":
	 which == postorder?"postorder":
	 which == endorder?"endorder":
	 which == leaf?"leaf": "unknown",
	 flow);
#endif

  if((which == ndpi_preorder) || (which == ndpi_leaf)) { /* Avoid walking the same node multiple times */
    if(enable_protocol_guess) {
      if(flow->detected_protocol == 0 /* UNKNOWN */) {
	node_guess_undetected_protocol(thread_id, flow);
	// printFlow(thread_id, flow);
      }
    }

    ndpi_thread_info[thread_id].stats.protocol_counter[flow->detected_protocol]       += flow->packets;
    ndpi_thread_info[thread_id].stats.protocol_counter_bytes[flow->detected_protocol] += flow->bytes;
    ndpi_thread_info[thread_id].stats.protocol_flows[flow->detected_protocol]++;
  }
}

/* ***************************************************** */

static void node_idle_scan_walker(const void *node, ndpi_VISIT which, int depth, void *user_data) {
  struct ndpi_flow *flow = *(struct ndpi_flow **) node;
  u_int16_t thread_id = *((u_int16_t *) user_data);

  if(ndpi_thread_info[thread_id].num_idle_flows == IDLE_SCAN_BUDGET) /* TODO optimise with a budget-based walk */
    return;

  if((which == ndpi_preorder) || (which == ndpi_leaf)) { /* Avoid walking the same node multiple times */
    if(flow->last_seen + MAX_IDLE_TIME < ndpi_thread_info[thread_id].last_time) {

      /* update stats */
      node_proto_guess_walker(node, which, depth, user_data);

      if (flow->detected_protocol == 0 /* UNKNOWN */ && !undetected_flows_deleted)
        undetected_flows_deleted = 1;

      free_ndpi_flow(flow);
      ndpi_thread_info[thread_id].stats.ndpi_flow_count--;

      /* adding to a queue (we can't delete it from the tree inline ) */
      ndpi_thread_info[thread_id].idle_flows[ndpi_thread_info[thread_id].num_idle_flows++] = flow;
    }
  }
}

/* ***************************************************** */

static int node_cmp(const void *a, const void *b) {
  struct ndpi_flow *fa = (struct ndpi_flow*)a;
  struct ndpi_flow *fb = (struct ndpi_flow*)b;

  if(fa->vlan_id   < fb->vlan_id  )   return(-1); else { if(fa->vlan_id   > fb->vlan_id  )   return(1); }
  if(fa->lower_ip   < fb->lower_ip  ) return(-1); else { if(fa->lower_ip   > fb->lower_ip  ) return(1); }
  if(fa->lower_port < fb->lower_port) return(-1); else { if(fa->lower_port > fb->lower_port) return(1); }
  if(fa->upper_ip   < fb->upper_ip  ) return(-1); else { if(fa->upper_ip   > fb->upper_ip  ) return(1); }
  if(fa->upper_port < fb->upper_port) return(-1); else { if(fa->upper_port > fb->upper_port) return(1); }
  if(fa->protocol   < fb->protocol  ) return(-1); else { if(fa->protocol   > fb->protocol  ) return(1); }

  return(0);
}

/* ***************************************************** */

static struct ndpi_flow *get_ndpi_flow(u_int16_t thread_id,
				       const u_int8_t version,
				       u_int16_t vlan_id,
				       const struct ndpi_iphdr *iph,
				       u_int16_t ip_offset,
				       u_int16_t ipsize,
				       u_int16_t l4_packet_len,
				       struct ndpi_id_struct **src,
				       struct ndpi_id_struct **dst,
				       u_int8_t *proto,
				       const struct ndpi_ip6_hdr *iph6) {
  u_int32_t idx, l4_offset;
  struct ndpi_tcphdr *tcph = NULL;
  struct ndpi_udphdr *udph = NULL;
  u_int32_t lower_ip;
  u_int32_t upper_ip;
  u_int16_t lower_port;
  u_int16_t upper_port;
  struct ndpi_flow flow;
  void *ret;
  u_int8_t *l3;

  /*
    Note: to keep things simple (ndpiReader is just a demo app)
    we handle IPv6 a-la-IPv4.
  */
  if(version == 4) {
    if(ipsize < 20)
      return NULL;

    if((iph->ihl * 4) > ipsize || ipsize < ntohs(iph->tot_len)
       || (iph->frag_off & htons(0x1FFF)) != 0)
      return NULL;

    l4_offset = iph->ihl * 4;
    l3 = (u_int8_t*)iph;
  } else {
    l4_offset = sizeof(struct ndpi_ip6_hdr);
    l3 = (u_int8_t*)iph6;
  }

  if(l4_packet_len < 64)
    ndpi_thread_info[thread_id].stats.packet_len[0]++;
  else if(l4_packet_len >= 64 && l4_packet_len < 128)
    ndpi_thread_info[thread_id].stats.packet_len[1]++;
  else if(l4_packet_len >= 128 && l4_packet_len < 256)
    ndpi_thread_info[thread_id].stats.packet_len[2]++;
  else if(l4_packet_len >= 256 && l4_packet_len < 1024)
    ndpi_thread_info[thread_id].stats.packet_len[3]++;
  else if(l4_packet_len >= 1024 && l4_packet_len < 1500)
    ndpi_thread_info[thread_id].stats.packet_len[4]++;
  else if(l4_packet_len >= 1500)
    ndpi_thread_info[thread_id].stats.packet_len[5]++;

  if(l4_packet_len > ndpi_thread_info[thread_id].stats.max_packet_len)
    ndpi_thread_info[thread_id].stats.max_packet_len = l4_packet_len;

  if(iph->saddr < iph->daddr) {
    lower_ip = iph->saddr;
    upper_ip = iph->daddr;
  } else {
    lower_ip = iph->daddr;
    upper_ip = iph->saddr;
  }

  *proto = iph->protocol;

  if(iph->protocol == 6 && l4_packet_len >= 20) {
    ndpi_thread_info[thread_id].stats.tcp_count++;

    // tcp
    tcph = (struct ndpi_tcphdr *) ((u_int8_t *) l3 + l4_offset);
    if(iph->saddr < iph->daddr) {
      lower_port = tcph->source;
      upper_port = tcph->dest;
    } else {
      lower_port = tcph->dest;
      upper_port = tcph->source;

      if(iph->saddr == iph->daddr) {
	if(lower_port > upper_port) {
	  u_int16_t p = lower_port;

	  lower_port = upper_port;
	  upper_port = p;
	}
      }
    }
  } else if(iph->protocol == 17 && l4_packet_len >= 8) {
    // udp
    ndpi_thread_info[thread_id].stats.udp_count++;

    udph = (struct ndpi_udphdr *) ((u_int8_t *) l3 + l4_offset);
    if(iph->saddr < iph->daddr) {
      lower_port = udph->source;
      upper_port = udph->dest;
    } else {
      lower_port = udph->dest;
      upper_port = udph->source;
    }
  } else {
    // non tcp/udp protocols
    lower_port = 0;
    upper_port = 0;
  }

  flow.protocol = iph->protocol, flow.vlan_id = vlan_id;
  flow.lower_ip = lower_ip, flow.upper_ip = upper_ip;
  flow.lower_port = lower_port, flow.upper_port = upper_port;

  if(0)
    printf("[NDPI] [%u][%u:%u <-> %u:%u]\n",
	   iph->protocol, lower_ip, ntohs(lower_port), upper_ip, ntohs(upper_port));

  idx = (vlan_id + lower_ip + upper_ip + iph->protocol + lower_port + upper_port) % NUM_ROOTS;
  ret = ndpi_tfind(&flow, &ndpi_thread_info[thread_id].ndpi_flows_root[idx], node_cmp);

  if(ret == NULL) {
    if(ndpi_thread_info[thread_id].stats.ndpi_flow_count == MAX_NDPI_FLOWS) {
      printf("ERROR: maximum flow count (%u) has been exceeded\n", MAX_NDPI_FLOWS);
      exit(-1);
    } else {
      struct ndpi_flow *newflow = (struct ndpi_flow*)malloc(sizeof(struct ndpi_flow));

      if(newflow == NULL) {
	printf("[NDPI] %s(1): not enough memory\n", __FUNCTION__);
	return(NULL);
      }

      memset(newflow, 0, sizeof(struct ndpi_flow));
      newflow->protocol = iph->protocol, newflow->vlan_id = vlan_id;
      newflow->lower_ip = lower_ip, newflow->upper_ip = upper_ip;
      newflow->lower_port = lower_port, newflow->upper_port = upper_port;

      if(version == 4) {
	inet_ntop(AF_INET, &lower_ip, newflow->lower_name, sizeof(newflow->lower_name));
	inet_ntop(AF_INET, &upper_ip, newflow->upper_name, sizeof(newflow->upper_name));
      } else {
	inet_ntop(AF_INET6, &iph6->ip6_src, newflow->lower_name, sizeof(newflow->lower_name));
	inet_ntop(AF_INET6, &iph6->ip6_dst, newflow->upper_name, sizeof(newflow->upper_name));
      }

      if((newflow->ndpi_flow = malloc_wrapper(size_flow_struct)) == NULL) {
	printf("[NDPI] %s(2): not enough memory\n", __FUNCTION__);
	return(NULL);
      } else
	memset(newflow->ndpi_flow, 0, size_flow_struct);

      if((newflow->src_id = malloc_wrapper(size_id_struct)) == NULL) {
	printf("[NDPI] %s(3): not enough memory\n", __FUNCTION__);
	return(NULL);
      } else
	memset(newflow->src_id, 0, size_id_struct);

      if((newflow->dst_id = malloc_wrapper(size_id_struct)) == NULL) {
	printf("[NDPI] %s(4): not enough memory\n", __FUNCTION__);
	return(NULL);
      } else
	memset(newflow->dst_id, 0, size_id_struct);

      ndpi_tsearch(newflow, &ndpi_thread_info[thread_id].ndpi_flows_root[idx], node_cmp); /* Add */
      ndpi_thread_info[thread_id].stats.ndpi_flow_count++;

      *src = newflow->src_id, *dst = newflow->dst_id;

      // printFlow(thread_id, newflow);

      return(newflow);
    }
  } else {
    struct ndpi_flow *flow = *(struct ndpi_flow**)ret;

    if(flow->lower_ip == lower_ip && flow->upper_ip == upper_ip
       && flow->lower_port == lower_port && flow->upper_port == upper_port)
      *src = flow->src_id, *dst = flow->dst_id;
    else
      *src = flow->dst_id, *dst = flow->src_id;

    return flow;
  }
}

/* ***************************************************** */

static struct ndpi_flow *get_ndpi_flow6(u_int16_t thread_id,
					u_int16_t vlan_id,
					const struct ndpi_ip6_hdr *iph6,
					u_int16_t ip_offset,
					struct ndpi_id_struct **src,
					struct ndpi_id_struct **dst,
					u_int8_t *proto) {
  struct ndpi_iphdr iph;

  memset(&iph, 0, sizeof(iph));
  iph.version = 4;
  iph.saddr = iph6->ip6_src.__u6_addr.__u6_addr32[2] + iph6->ip6_src.__u6_addr.__u6_addr32[3];
  iph.daddr = iph6->ip6_dst.__u6_addr.__u6_addr32[2] + iph6->ip6_dst.__u6_addr.__u6_addr32[3];
  iph.protocol = iph6->ip6_ctlun.ip6_un1.ip6_un1_nxt;

  if(iph.protocol == 0x3C /* IPv6 destination option */) {
    u_int8_t *options = (u_int8_t*)iph6 + sizeof(const struct ndpi_ip6_hdr);

    iph.protocol = options[0];
  }

  return(get_ndpi_flow(thread_id, 6, vlan_id, &iph, ip_offset,
		       sizeof(struct ndpi_ip6_hdr),
		       ntohs(iph6->ip6_ctlun.ip6_un1.ip6_un1_plen),
		       src, dst, proto, iph6));
}

/* ***************************************************** */


/* ***************************************************** */

static void terminateDetection(u_int16_t thread_id) {
  int i;

  for(i=0; i<NUM_ROOTS; i++) {
    ndpi_tdestroy(ndpi_thread_info[thread_id].ndpi_flows_root[i], ndpi_flow_freer);
    ndpi_thread_info[thread_id].ndpi_flows_root[i] = NULL;
  }

  ndpi_exit_detection_module(ndpi_thread_info[thread_id].ndpi_struct, free_wrapper);
}

/***********************************************/



uint32_t parseIPV4string(char* ipAddress) {
  char ipbytes[4];
  sscanf(ipAddress, "%d.%d.%d.%d", &ipbytes[3], &ipbytes[2], &ipbytes[1], &ipbytes[0]);
  //unsigned long ip = (unsigned long)ipbytes[3]+ipbytes[2]*256+ipbytes[1]*256*256+ipbytes[0]*256*256*256;
  //printf("The unsigned long integer is %lu\n",ip);
  uint32_t ip = ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24;
  printf("ip %" PRIu32 "\n",ip);
  return ip;
}

void concatenate_string(char *original, char *add)
{
   while(*original)
      original++;
 
   while(*add)
   {
      *original = *add;
      add++;
      original++;
   }
   *original = '\0';
}

/*printing the value corresponding to boolean, double, integer and strings*/
void print_json_value(json_object *jobj){
  enum json_type type;
  printf("type: ",type);
  type = json_object_get_type(jobj); /*Getting the type of the json object*/
  switch (type) {
    case json_type_boolean: printf("json_type_booleann");
                         printf("value: %sn", json_object_get_boolean(jobj)? "true": "false");
                         break;
    case json_type_double: printf("json_type_doublen");
                        printf("          value: %lfn", json_object_get_double(jobj));
                         break;
    case json_type_int: printf("json_type_intn");
                        printf("          value: %dn", json_object_get_int(jobj));
                         break;
    case json_type_string: printf("json_type_stringn");
                         printf("          value: %sn", json_object_get_string(jobj));
                         break;
  }

}

void json_parse_array( json_object *jobj, char *key) {
  void json_parse(json_object * jobj); /*Forward Declaration*/
  enum json_type type;

  json_object *jarray = jobj; /*Simply get the array*/
  if(key) {
    jarray = json_object_object_get(jobj, key); /*Getting the array if it is a key value pair*/
  }

  int arraylen = json_object_array_length(jarray); /*Getting the length of the array*/
  printf("Array Length: %dn",arraylen);
  int i;
  json_object * jvalue;

  for (i=0; i< arraylen; i++){
    jvalue = json_object_array_get_idx(jarray, i); /*Getting the array element at position i*/
    type = json_object_get_type(jvalue);
    if (type == json_type_array) {
      json_parse_array(jvalue, NULL);
    }
    else if (type != json_type_object) {
      printf("value[%d]: ",i);
      print_json_value(jvalue);
    }
    else {
      json_parse(jvalue);
    }
  }
}

/*Parsing the json object*/
void json_parse(json_object * jobj) {
  enum json_type type;
  json_object_object_foreach(jobj, key, val) { /*Passing through every array element*/
    printf("type: ",type);
    type = json_object_get_type(val);
    switch (type) {
      case json_type_boolean: 
      case json_type_double: 
      case json_type_int: 
      case json_type_string: print_json_value(val);
                           break; 
      case json_type_object: printf("json_type_objectn");
                           jobj = json_object_object_get(jobj, key);
                           json_parse(jobj); 
                           break;
      case json_type_array: printf("type: json_type_array, ");
                          json_parse_array(jobj, key);
                          break;
    }
  }
} 

unsigned int ip_to_int (const char * ip)
{
    /* The return value. */
    unsigned v = 0;
    /* The count of the number of bytes processed. */
    int i;
    /* A pointer to the next digit to process. */
    const char * start;

    start = ip;
    for (i = 0; i < 4; i++) {
        /* The digit being processed. */
        char c;
        /* The value of this byte. */
        int n = 0;
        while (1) {
            c = * start;
            start++;
            if (c >= '0' && c <= '9') {
                n *= 10;
                n += c - '0';
            }
            /* We insist on stopping at "." if we are still parsing
               the first, second, or third numbers. If we have reached
               the end of the numbers, we will allow any character. */
            else if ((i < 3 && c == '.') || i == 3) {
                break;
            }
            else {
                return 0;
            }
        }
        if (n >= 256) {
            return 0;
        }
        v *= 256;
        v += n;
    }
    return v;
}


void *connection_handler(void *threadid)
{
    
    int sockfd;
	struct sockaddr_in self;
	char buffer[MAXBUF];

	/*---Create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Socket");
		exit(errno);
	}

	/*---Initialize address/port structure---*/
	bzero(&self, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(MY_PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---Assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
	{
		perror("socket--bind");
		exit(errno);
	}
	
	int true = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int));
	
	/*---Make it a "listening socket"---*/
	if ( listen(sockfd, 20) != 0 )
	{
		perror("socket--listen");
		exit(errno);
	}

	/*---Forever... ---*/
	int line = 0;
	
	int con = 0, valid = 0;
	
	while (1)
	{	
		
		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);
		char *fl;
		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
		printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
	
		char *json[200];
		int first = 1;
		int index = 0;
		
		if(recv(clientfd, buffer, MAXBUF, 0)==0){
			int first = 0;
		}
		else{
			//printf("rcvd %s\n",buffer);
			char *token = NULL;
			
			token = strtok(buffer, "\n");
			while (token) {
				if(first){
					first = 0;
					fl = token;
				}
				
				if(strstr(token, "{\"classification_rul") != NULL || con == 1){
					//printf("Current token: %s\n", token);
					if(!con) json[0] = 0;
					strcat(json, token);
					valid = 1;
					con = 1;
					
				}
				line++;
				//printf("Current token: %s\n", token);
				token = strtok(NULL, "\n");
			}
		}
		
		first = 1;
		con = 0;
		printf("\n----JSON String %s-----\n\n",json);
		
		
		char *ip = strtok(fl," /");
		char *ipR;
		while(ip != NULL){
		//printf("backslash %s\n",fl);
		if(index == 3){
			ipR = strtok(ip, " ");
			//printf("ip is %s\n",ipR);
			 break; 
		}
		ip = strtok(NULL,"/");
		index++;
		}
		
		int a,b,c,d;
		char ip2[16];
		sscanf(ipR,"%d.%d.%d.%d",&a,&b,&c,&d);
		sprintf(ip2, "%d.%d.%d.%d", d, c, b, a);
		//printf("ip str___%s___\n", ip2);
		int ipAddress = ip_to_int(ip2);//parseIPV4string(ipR);
		
		first = 1;
		json_object * jobj;
		jobj = json_tokener_parse(json);
		
		//json_parse(jobj);
		
		
		struct json_object *item_obj, *item_obj_name, *ruleid_obj_name, *tos_obj_name, *class_json;
		
		class_json = json_object_object_get(jobj, "classification_rules");
		
		if(rules_size > 0)
		{
			int ci;
			for(ci = 0; ci <= rules_size; ci++)
			{
			
				
				if(class_rules[ci].ip == ipAddress)
				{
					class_rules[ci].ip = 0;
					class_rules[ci].ruleid = 0;
					class_rules[ci].tos = 0;
					rules_size--;
					resizeArray(rules_size);
				}
				
				
			}
		}
		
		
		
		
		if(class_json != NULL){
		
		int arraylen = json_object_array_length(class_json);
		//printf("json array length %u\n",arraylen);
		int i;
		for (i = 0; i < arraylen; i++) {
		  // get the i-th object in medi_array
		  item_obj = json_object_array_get_idx(class_json, i);
		  // get the name attribute in the i-th object
		  ruleid_obj_name = json_object_object_get(item_obj, "RuleId");
		  tos_obj_name = json_object_object_get(item_obj, "ToS");
		  
		  struct class_rule rule;
		  rule.ip = ipAddress;
		  rule.ruleid = atoi(json_object_get_string(ruleid_obj_name));
		  rule.tos = atoi(json_object_get_string(tos_obj_name))*4;
		  
		  //printf("ip %u id %u tos %u\n",rule.ip, rule.ruleid, rule.tos);
		  rules_size++;
		  resizeArray(rules_size);
		  class_rules[rules_size-1] = rule;
		  
		}
		
		}
		
		close(clientfd);
	}

	/*---Clean up (should never get here!)---*/
	close(sockfd);
	return 0;
}



static void setupDetection(u_int16_t thread_id) {
  NDPI_PROTOCOL_BITMASK all;

  memset(&ndpi_thread_info[thread_id], 0, sizeof(ndpi_thread_info[thread_id]));

  // init global detection structure
  ndpi_thread_info[thread_id].ndpi_struct = ndpi_init_detection_module(detection_tick_resolution,
								       malloc_wrapper, free_wrapper, debug_printf);
  if(ndpi_thread_info[thread_id].ndpi_struct == NULL) {
    printf("ERROR: global structure initialization failed\n");
    exit(-1);
  }

  //if(!full_http_dissection)
  //  ndpi_thread_info[thread_id].ndpi_struct->http_dont_dissect_response = 1;

  // enable all protocols
  NDPI_BITMASK_SET_ALL(all);
  ndpi_set_protocol_detection_bitmask2(ndpi_thread_info[thread_id].ndpi_struct, &all);

  // allocate memory for id and flow tracking
  size_id_struct = ndpi_detection_get_sizeof_ndpi_id_struct();
  size_flow_struct = ndpi_detection_get_sizeof_ndpi_flow_struct();

  // clear memory for results
  memset(ndpi_thread_info[thread_id].stats.protocol_counter, 0, sizeof(ndpi_thread_info[thread_id].stats.protocol_counter));
  memset(ndpi_thread_info[thread_id].stats.protocol_counter_bytes, 0, sizeof(ndpi_thread_info[thread_id].stats.protocol_counter_bytes));
  memset(ndpi_thread_info[thread_id].stats.protocol_flows, 0, sizeof(ndpi_thread_info[thread_id].stats.protocol_flows));

  if(_protoFilePath != NULL)
    ndpi_load_protocols_file(ndpi_thread_info[thread_id].ndpi_struct, _protoFilePath);
}


void printHelp(void) {
  printf("pfbridge - Forwards traffic from -a -> -b device using vanilla PF_RING (no DNA)\n\n");
  printf("-h              [Print help]\n");
  printf("-v              [Verbose]\n");
  printf("-p              [Use pfring_send() instead of bridge]\n");
  printf("-a <device>     [First device name]\n");
  printf("-b <device>     [Second device name]\n");
  printf("-g <core_id>    Bind this app to a core\n");
  printf("-w <watermark>  Watermark\n");
}

/* ******************************** */

void my_sigalarm(int sig) {
  char buf[32];

  pfring_format_numbers((double)num_sent, buf, sizeof(buf), 0),
  printf("%s pps\n", buf);
  num_sent = 0;
  alarm(1);
  signal(SIGALRM, my_sigalarm);
}

void *processing_thread(void *_thread_id) {
  long thread_id = (long) _thread_id;

#ifdef linux
  if(core_affinity[thread_id] >= 0) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_affinity[thread_id], &cpuset);

    if(pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
      fprintf(stderr, "Error while binding thread %ld to core %d\n", thread_id, core_affinity[thread_id]);
    else {
      if((!json_flag) && (!quiet_mode)) printf("Running thread %ld on core %d...\n", thread_id, core_affinity[thread_id]);
    }
  } else
#endif
    if((!json_flag) && (!quiet_mode)) printf("Running thread %ld...\n", thread_id);



  return NULL;
}

void test_lib() {
  struct timeval begin, end;
  u_int64_t tot_usec;
  long thread_id = 0;

#ifdef HAVE_JSON_C
  json_init();
#endif

  
    setupDetection(thread_id);
  

  //gettimeofday(&begin, NULL);

  /* Running processing threads */
  //for(thread_id = 0; thread_id < num_threads; thread_id++)
    pthread_create(&ndpi_thread_info[thread_id].pthread, NULL, processing_thread, (void *) thread_id);

  /* Waiting for completion */
  
    pthread_join(ndpi_thread_info[thread_id].pthread, NULL);

  //gettimeofday(&end, NULL);
  //tot_usec = end.tv_sec*1000000 + end.tv_usec - (begin.tv_sec*1000000 + begin.tv_usec);

  /* Printing cumulative results */
  //printResults(tot_usec);

    //terminateDetection(thread_id);
}

static unsigned int packet_processing(u_int16_t thread_id,
				      const u_int64_t time,
				      u_int16_t vlan_id,
				      const struct ndpi_iphdr *iph,
				      struct ndpi_ip6_hdr *iph6,
				      u_int16_t ip_offset,
				      u_int16_t ipsize, u_int16_t rawsize) {
  struct ndpi_id_struct *src, *dst;
  struct ndpi_flow *flow;
  struct ndpi_flow_struct *ndpi_flow = NULL;
  u_int32_t protocol = 0;
  u_int8_t proto;

  
  if(iph)
    flow = get_ndpi_flow(thread_id, 4, vlan_id, iph, ip_offset, ipsize,
			 ntohs(iph->tot_len) - (iph->ihl * 4),
			 &src, &dst, &proto, NULL);
  else
    flow = get_ndpi_flow6(thread_id, vlan_id, iph6, ip_offset, &src, &dst, &proto);

	
	
  if(flow != NULL) {
    ndpi_thread_info[thread_id].stats.ip_packet_count++;
    ndpi_thread_info[thread_id].stats.total_wire_bytes += rawsize + 24 /* CRC etc */, ndpi_thread_info[thread_id].stats.total_ip_bytes += rawsize;
    ndpi_flow = flow->ndpi_flow;
    flow->packets++, flow->bytes += rawsize;
    flow->last_seen = time;
  } else {
    return(0);
  }

  
  
  if(flow->detection_completed) {
	proto_app = ndpi_get_proto_name(ndpi_thread_info[0].ndpi_struct, flow->detected_protocol);
	return(0);
	}


  protocol = (const u_int32_t)ndpi_detection_process_packet(ndpi_thread_info[thread_id].ndpi_struct, ndpi_flow,
							    iph ? (uint8_t *)iph : (uint8_t *)iph6,
							    ipsize, time, src, dst);

  flow->detected_protocol = protocol;
  
  if((flow->detected_protocol != NDPI_PROTOCOL_UNKNOWN)
     || ((proto == IPPROTO_UDP) && (flow->packets > 8))
     || ((proto == IPPROTO_TCP) && (flow->packets > 10))) {
    flow->detection_completed = 1;
	


    if((flow->detected_protocol == NDPI_PROTOCOL_UNKNOWN) )
      ndpi_int_add_connection(ndpi_thread_info[thread_id].ndpi_struct, ndpi_flow, NDPI_PROTOCOL_STUN, NDPI_REAL_PROTOCOL);
 
	
	
    snprintf(flow->host_server_name, sizeof(flow->host_server_name), "%s", flow->ndpi_flow->host_server_name);

    if((proto == IPPROTO_TCP) && (flow->detected_protocol != NDPI_PROTOCOL_DNS)) {
      snprintf(flow->ssl.client_certificate, sizeof(flow->ssl.client_certificate), "%s", flow->ndpi_flow->protos.ssl.client_certificate);
      snprintf(flow->ssl.server_certificate, sizeof(flow->ssl.server_certificate), "%s", flow->ndpi_flow->protos.ssl.server_certificate);
    }

//#if 0
    if((
	(flow->detected_protocol == NDPI_PROTOCOL_HTTP)
	|| (flow->detected_protocol == NDPI_SERVICE_FACEBOOK)
	)
       && full_http_dissection) {
	   
      char *method;
      printf("[URL] %s\n", ndpi_get_http_url(ndpi_thread_info[thread_id].ndpi_struct, ndpi_flow));
      printf("[Content-Type] %s\n", ndpi_get_http_content_type(ndpi_thread_info[thread_id].ndpi_struct, ndpi_flow));
      switch(ndpi_get_http_method(ndpi_thread_info[thread_id].ndpi_struct, ndpi_flow)) {
      case HTTP_METHOD_OPTIONS: method = "HTTP_METHOD_OPTIONS"; break;
      case HTTP_METHOD_GET: method = "HTTP_METHOD_GET"; break;
      case HTTP_METHOD_HEAD: method = "HTTP_METHOD_HEAD"; break;
      case HTTP_METHOD_POST: method = "HTTP_METHOD_POST"; break;
      case HTTP_METHOD_PUT: method = "HTTP_METHOD_PUT"; break;
      case HTTP_METHOD_DELETE: method = "HTTP_METHOD_DELETE"; break;
      case HTTP_METHOD_TRACE: method = "HTTP_METHOD_TRACE"; break;
      case HTTP_METHOD_CONNECT: method = "HTTP_METHOD_CONNECT"; break;
      default: method = "HTTP_METHOD_UNKNOWN"; break;
      }
      printf("[Method] %s\n", method);
    }
//#endif


    free_ndpi_flow(flow);

    if(verbose > 1) {
      if(enable_protocol_guess) {
	if(flow->detected_protocol == 0 /* UNKNOWN */) {
	  protocol = node_guess_undetected_protocol(thread_id, flow);
	}
      }

      printFlow(thread_id, flow);
    }
  }

#if 0
  if(ndpi_flow->l4.tcp.host_server_name[0] != '\0')
    printf("%s\n", ndpi_flow->l4.tcp.host_server_name);
#endif

  if(live_capture) {
  
    if(ndpi_thread_info[thread_id].last_idle_scan_time + IDLE_SCAN_PERIOD < ndpi_thread_info[thread_id].last_time) {
      
      ndpi_twalk(ndpi_thread_info[thread_id].ndpi_flows_root[ndpi_thread_info[thread_id].idle_scan_idx], node_idle_scan_walker, &thread_id);

      
      while (ndpi_thread_info[thread_id].num_idle_flows > 0)
	ndpi_tdelete(ndpi_thread_info[thread_id].idle_flows[--ndpi_thread_info[thread_id].num_idle_flows],
		     &ndpi_thread_info[thread_id].ndpi_flows_root[ndpi_thread_info[thread_id].idle_scan_idx], node_cmp);

      if(++ndpi_thread_info[thread_id].idle_scan_idx == NUM_ROOTS) ndpi_thread_info[thread_id].idle_scan_idx = 0;
      ndpi_thread_info[thread_id].last_idle_scan_time = ndpi_thread_info[thread_id].last_time;
    }
  }
  return 0;
}
/* ****************************************************** */

int main(int argc, char* argv[]) {
  pfring *a_ring, *b_ring;
  char *a_dev = NULL, *b_dev = NULL, c;
  u_int8_t verbose = 0, use_pfring_send = 1;
  int a_ifindex, b_ifindex;
  int bind_core = -1;
  u_int16_t watermark = 1;
  char *bpfFilter = NULL;
  long thread_id = 0;

  while((c = getopt(argc,argv, "ha:b:c:f:vpg:w:")) != -1) {
    switch(c) {
      case 'h':
	printHelp();
	return 0;
	break;
      case 'a':
	a_dev = strdup(optarg);
	break;
      case 'b':
	b_dev = strdup(optarg);
	break;
      case 'f':
        bpfFilter = strdup(optarg);
	break;
      case 'p':
	use_pfring_send = 1;
	break;
      case 'v':
	verbose = 1;
	break;
      case 'g':
        bind_core = atoi(optarg);
        break;
      case 'w':
        watermark = atoi(optarg);
        break;
    }
  }  

  if ((!a_dev) || (!b_dev)) {
    printf("You must specify two devices!\n");
    return -1;
  }

  if(strcmp(a_dev, b_dev) == 0) {
    printf("Bridge devices must be different!\n");
    //return -1;
  }
  
  memset(ndpi_thread_info, 0, sizeof(ndpi_thread_info));
   
  int socket_desc , new_socket , *new_sock, s;
  pthread_t sniffer_thread;
  
  class_rules = malloc(3 * sizeof(int));
  
  if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) s) < 0)
  {
    perror("could not create thread");
    return 1;
  }
   
   test_lib();
  
  
  
  /* Device A */
  if((a_ring = pfring_open(a_dev, MAX_PKT_LEN, PF_RING_PROMISC | PF_RING_LONG_HEADER |
                           (use_pfring_send ? 0 : PF_RING_RX_PACKET_BOUNCE))
    ) == NULL) {
    printf("pfring_open error for %s [%s]\n", a_dev, strerror(errno));
    return(-1);
  }

  pfring_set_application_name(a_ring, "pfbridge-a");
  pfring_set_direction(a_ring, rx_only_direction);
  pfring_set_socket_mode(a_ring, recv_only_mode);
  pfring_set_poll_watermark(a_ring, watermark);
  pfring_get_bound_device_ifindex(a_ring, &a_ifindex);

  /* Adding BPF filter */
  if(bpfFilter != NULL) {
    int rc = pfring_set_bpf_filter(a_ring, bpfFilter);
    if(rc != 0)
      printf("pfring_set_bpf_filter(%s) returned %d\n", bpfFilter, rc);
    else
      printf("Successfully set BPF filter '%s'\n", bpfFilter);
  }

  /* Device B */

  if((b_ring = pfring_open(b_dev, MAX_PKT_LEN, PF_RING_PROMISC|PF_RING_LONG_HEADER)) == NULL) {
    printf("pfring_open error for %s [%s]\n", b_dev, strerror(errno));
    pfring_close(a_ring);
    return(-1);
  }

  pfring_set_application_name(b_ring, "pfbridge-b");
  pfring_set_socket_mode(b_ring, send_only_mode);
  pfring_get_bound_device_ifindex(b_ring, &b_ifindex);
  
  /* Enable Sockets */

  if (pfring_enable_ring(a_ring) != 0) {
    printf("Unable enabling ring 'a' :-(\n");
    pfring_close(a_ring);
    pfring_close(b_ring);
    return(-1);
  }

  if(use_pfring_send) {
    if (pfring_enable_ring(b_ring)) {
      printf("Unable enabling ring 'b' :-(\n");
      pfring_close(a_ring);
      pfring_close(b_ring);
      return(-1);
    }
  } else {
    pfring_close(b_ring);
  }

  signal(SIGALRM, my_sigalarm);
  alarm(1);

  if(bind_core >= 0)
    bind2core(bind_core);

	char *unk = "Unknown";
	
  while(1) {
    u_char *buffer;
    struct pfring_pkthdr hdr;
    
    if(pfring_recv(a_ring, &buffer, 0, &hdr, 1) > 0) {
      int rc;
	  
	  struct ip_header *ip_header=malloc(sizeof(ip_header));
	  //struct ether_header *eh = (struct ether_header*)buffer;
	  ip_header = (struct ip_header*)(buffer + sizeof(struct ether_header));
	  pfring_parse_pkt((u_char*)buffer, (struct pfring_pkthdr*)&hdr, 5, 0, 0);
	  
	  
	  
	  packet_processing(thread_id, 1, 0, ip_header, NULL,
		 ip_header->frag_off, ip_header->tot_len, hdr.caplen);
		
		
		//if(proto_app != NULL)
		//	if(strcmp(proto_app, unk) != 0) printf("proto_app %s\n", proto_app);
			
		//ip_header->tos = 5;
		
		int i;
		int appid = 0;
		
		if(proto_app != NULL){
		if(strcmp(proto_app, "HTTP") == 0) appid = 1;
		  else if(strcmp(proto_app, "Bittorrent")==0 ) appid = 2;
		  else if(strcmp(proto_app,"Facebook")==0) appid = 3;
		  else if(strcmp(proto_app,"YouTube")==0) appid = 4;
		  else if(strcmp(proto_app,"Quake")==0) appid = 5;
		  else if(strcmp(proto_app,"MEOGO")==0) appid = 6;
		}
		
	  if(rules_size > 0){
	  
	  if(appid > 0){
		//printf("AppID %u\n", appid);
		  for (i = 0; i < rules_size; i++){
			if(ip_header->daddr == class_rules[i].ip && class_rules[i].ruleid == appid){
				ip_header->tos = class_rules[i].tos;
				printf("rule applied!!\n");
			}
		  }
	  }
  }
		
		
		
		
      if(use_pfring_send) {

	rc = pfring_send(b_ring, (char *) buffer, hdr.caplen, 1);
	
	if(rc < 0){
	int p =0;
	}
	  //printf("pfring_send(caplen=%u <= mtu=%u?) error %d\n", hdr.caplen, b_ring->mtu_len, rc);
	else if(verbose)
	  printf("Forwarded %d bytes packet\n", hdr.len);	
      } else {
	rc = pfring_send_last_rx_packet(a_ring, b_ifindex);
	
	if(rc < 0)
	  printf("pfring_send_last_rx_packet() error %d\n", rc);
	else if(verbose)
	  printf("Forwarded %d bytes packet\n", hdr.len);
      }

      if(rc >= 0) num_sent++;
	
    }
  }

  pfring_close(a_ring);
  if(use_pfring_send) pfring_close(b_ring);
  
  return(0);
}
