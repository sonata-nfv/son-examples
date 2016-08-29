/*
 *
 * Copyright 2012 - ntop
 *
 * Authors: Luca Deri <deri@ntop.org>
 *          Alfredo Cardigliano <cardigliano@ntop.org>
 *
 */

#ifndef _PFRING_ZERO_H_
#define _PFRING_ZERO_H_

/**
 * @file pfring_zero.h
 *
 * @brief      Libzero library header file.
 * @details    This header file is automatically included in any PF_RING-based applications (when the HAVE_ZERO macro is defined).
 */

/* #define DNA_CLUSTER_MAX_NUM_SLAVES              32 */ /**< Max number of slaves (Defined in pf_ring.h). 32 is due to 32-bit mask. */
#define DNA_CLUSTER_MAX_NUM_SOCKETS                32    /**< Max number of interfaces that can be added to a cluster. 32 is due to 32-bit mask. */

/* flags for the 'flag' bitmap parameter of dna_cluster_create() */
#define DNA_CLUSTER_DIRECT_FORWARDING     1 << 0 /**< dna_cluster_create() flag: enable the "direct forwarding" for transmitting incoming packets to an egress interface according to the return values of the distribution function. */
#define DNA_CLUSTER_NO_ADDITIONAL_BUFFERS 1 << 1 /**< dna_cluster_create() flag: disable allocation of “pfring_pkt_buff” (see "The Slave API" Section), reducing memory usage. */
#define DNA_CLUSTER_HUGEPAGES             1 << 2 /**< dna_cluster_create() flag: enable memory allocation via hugepages */
#define DNA_CLUSTER_DCA                   1 << 3 /**< dna_cluster_create() flag: disable smart-prefetch algorithm */
#define DNA_CLUSTER_TIME_PULSE_THREAD     1 << 4 /**< dna_cluster_create() flag: enable an additional thread for accurate sw timestamping */

/* return values for pfring_dna_cluster_distribution_func()  */
#define DNA_CLUSTER_DROP -1 /**< pfring_dna_cluster_distribution_func return value:  */
#define DNA_CLUSTER_PASS  0 /**< pfring_dna_cluster_distribution_func return value:  */
#define DNA_CLUSTER_FRWD  1 /**< pfring_dna_cluster_distribution_func return value:  */

/* return values for pfring_dna_cluster_distribution_func()  */
#define DNA_BOUNCER_DROP -1 /**< pfring_dna_bouncer_decision_func return value:  */
#define DNA_BOUNCER_PASS  0 /**< pfring_dna_bouncer_decision_func return value:  */

typedef void pfring_dna_bouncer;

typedef enum {
  one_way_mode = 0,
  two_way_mode
} dna_bouncer_mode;

typedef int (*pfring_dna_bouncer_decision_func)(u_int32_t *pkt_len, u_char *pkt, const u_char *user_bytes, u_int8_t direction);

typedef void pfring_dna_cluster;

typedef struct {
  u_int64_t tot_rx_packets;
  u_int64_t tot_tx_packets;
  u_int64_t tot_rx_processed; 
} pfring_dna_cluster_stat;

typedef struct {
  u_int32_t rx_queued;
} pfring_dna_cluster_slave_stat;

typedef struct {
  u_int32_t num_slaves;
  pfring_dna_cluster_slave_stat approximate_stats[DNA_CLUSTER_MAX_NUM_SLAVES];
} pfring_dna_cluster_slaves_info;

typedef int (*pfring_dna_cluster_distribution_func)(const u_char *buffer, const u_int16_t buffer_len, const pfring_dna_cluster_slaves_info *slaves_info, u_int32_t *id_mask, u_int32_t *hash);

/* --------------------- Public API --------------------- */

/* DNA cluster master */

/**
 * Create a new DNA Cluster handle. The cluster just created has no ring associated. 
 * @param cluster_id The cluster identifier.
 * @param num_apps   The number of slave applications/threads.
 * @param flags      A mask for enabling additional extensions (see DNA_CLUSTER_* macros).
 * @return The cluster handle. 
 */
pfring_dna_cluster* dna_cluster_create(u_int32_t cluster_id, u_int32_t num_apps, u_int32_t flags);

/**
 * Changes the default values for the number of rx/tx queue slots and additional buffers (experts only).
 * @param handle                   The DNA Cluster handle.
 * @param slave_rx_queue_len       The number of slots in the slave rx queue.
 * @param slave_tx_queue_len       The number of slots in the slave tx queue.
 * @param slave_additional_buffers The maximum number of extra buffers a slave can allocate (see "The Slave API" Section).
 * @param 0 on success, a negative value otherwise.
 * @return 
 */
int  dna_cluster_low_level_settings(pfring_dna_cluster *handle, u_int32_t slave_rx_queue_len, u_int32_t slave_tx_queue_len, u_int32_t slave_additional_buffers);

/**
 * Set the hugetlb mountopoint, when the hugepages support is enabled.
 * @param handle     The DNA Cluster handle.
 * @param mountpoint The mountpoint.
 * @return 0 on success, a negative value otherwise.
 */
int  dna_cluster_set_hugepages_mountpoint(pfring_dna_cluster *handle, const char *mountpoint);

/**
 * Add a PF_RING socket to the DNA Cluster.
 * @param handle The DNA Cluster handle.
 * @param ring   The PF_RING handle.
 * @return 0 on success, a negative value otherwise.
 */
int  dna_cluster_register_ring(pfring_dna_cluster *handle, pfring *ring);

/**
 * Bind the RX and TX master threads to the given core ids. In clusters, threads are used to poll (RX) and send (TX) cluster packets. This function is used to specify thread affinity across cores. 
 * @param handle     The DNA Cluster handle.
 * @param rx_core_id The core id for the RX thread.
 * @param tx_core_id The core id for the TX thread.
 */
void dna_cluster_set_cpu_affinity(pfring_dna_cluster *handle, u_int32_t rx_core_id, u_int32_t tx_core_id);

/**
 * Sets the core affinity and the timestamp resolution for the time pulse thread.
 * @param handle          The DNA Cluster handle.
 * @param core_id         The core id for the time pulse thread. 
 * @param resolution_nsec The timestamp resolution (the higher value, the better cpu utilisation). 
 */
void dna_cluster_time_pulse_settings(pfring_dna_cluster *handle, u_int32_t core_id, u_int32_t resolution_nsec);

/**
 * Sets the name for the rx and tx threads.
 * @param handle     The DNA Cluster handle.
 * @param rx_core_id The name for the rx thread. 
 * @param tx_core_id The name id for the tx thread. 
 */
void dna_cluster_set_thread_name(pfring_dna_cluster *handle, const char *rx_thread_name, const char *tx_thread_name);

/**
 * Set the cluster mode: receive (TX only), send (RX and TX), or both receive and send (RX and TX). 
 * @param handle The DNA Cluster handle.
 * @param mode   The cluster mode.
 * @return 0 on success, a negative value otherwise.
 */
int  dna_cluster_set_mode(pfring_dna_cluster *handle, socket_mode mode);

/**
 * Set the packet distribution function (the default function is a bidirectional IP-based hash function). This call allows developers to specify their own custom function. 
 * @param handle The DNA Cluster handle.
 * @param func   The distribution function.
 */
void dna_cluster_set_distribution_function(pfring_dna_cluster *handle, pfring_dna_cluster_distribution_func func);

/**
 * Set the ingress packet wait mode: passive (poll) or active wait. 
 * @param handle      The DNA Cluster handle.
 * @param active_wait A boolean value: 0 for passive mode, 1 for active mode.
 */
void dna_cluster_set_wait_mode(pfring_dna_cluster *handle, u_int32_t active_wait);

/**
 * Enable the cluster. 
 * @param handle The DNA Cluster handle.
 * @return 0 on success, a negative value otherwise.
 */
int  dna_cluster_enable(pfring_dna_cluster *handle);

/**
 * Disable the cluster. 
 * @param handle The DNA Cluster handle.
 * @return 0 on success, a negative value otherwise.
 */
int  dna_cluster_disable(pfring_dna_cluster *handle);

/**
 * Return cluster statistics.
 * @param handle The DNA Cluster handle.
 * @param stats  The memory area where stats will be copied.
 * @return 0 on success, a negative value otherwise.
 */
int  dna_cluster_stats(pfring_dna_cluster *handle, pfring_dna_cluster_stat *stats); 

/**
 * Destroy the cluster, and close the bound PF_RING sockets. 
 * @param handle The DNA Cluster handle.
 */
void dna_cluster_destroy(pfring_dna_cluster *handle);

/* DNA bouncer */

/**
 * Create a new DNA Bouncer handle. 
 * @param ingress_ring The socket where packets will be read.
 * @param egress_ring  The socket where packets will be sent.
 * @return The DNA Bouncer handle.
 */
pfring_dna_bouncer *pfring_dna_bouncer_create(pfring *ingress_ring, pfring *egress_ring);

/**
 * Set the DNA Bouncer mode to one-way (default) or two-way. 
 * @param handle The DNA Bouncer handle.
 * @param mode   The mode: one_way_mode or two_way_mode.
 * @return 0 on success, a negative value otherwise.
 */
int  pfring_dna_bouncer_set_mode(pfring_dna_bouncer *handle, dna_bouncer_mode mode);

/**
 * Enable the DNA Bouncer. 
 * @param handle     The DNA Bouncer handle.
 * @param func       The packet processing function.
 * @param user_bytes A pointer to user data.
 * @param wait_for_packet If 0 active wait is used to check the packet availability.
 * @return 0 on success, a negative value otherwise.
 */
int  pfring_dna_bouncer_loop(pfring_dna_bouncer *handle, pfring_dna_bouncer_decision_func func, const u_char *user_bytes, u_int8_t wait_for_packet);

/**
 * Stop the bouncer. 
 * @param handle The DNA Bouncer handle.
 */
void pfring_dna_bouncer_breakloop(pfring_dna_bouncer *handle);

/**
 * Destroy the bouncer and close the bound PF_RING sockets. 
 * @param handle The DNA Bouncer handle.
 */
void pfring_dna_bouncer_destroy(pfring_dna_bouncer *handle);

/* -------------------- Internal API -------------------- */

/* DNA cluster slave */

int   pfring_dna_cluster_open(pfring *ring);

#endif /* _PFRING_ZERO_H_ */

