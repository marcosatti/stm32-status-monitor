/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.4 */

#ifndef PB_STATUS_PB_H_INCLUDED
#define PB_STATUS_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _Status {
    uint32_t cpu;
    uint32_t cpu_power;
    uint32_t temperature;
    uint32_t network;
    uint32_t samba_users_connected;
    uint32_t samba_files_opened;
} Status;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Status_init_default                      {0, 0, 0, 0, 0, 0}
#define Status_init_zero                         {0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define Status_cpu_tag                           1
#define Status_cpu_power_tag                     2
#define Status_temperature_tag                   3
#define Status_network_tag                       4
#define Status_samba_users_connected_tag         5
#define Status_samba_files_opened_tag            6

/* Struct field encoding specification for nanopb */
#define Status_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   cpu,               1) \
X(a, STATIC,   SINGULAR, UINT32,   cpu_power,         2) \
X(a, STATIC,   SINGULAR, UINT32,   temperature,       3) \
X(a, STATIC,   SINGULAR, UINT32,   network,           4) \
X(a, STATIC,   SINGULAR, UINT32,   samba_users_connected,   5) \
X(a, STATIC,   SINGULAR, UINT32,   samba_files_opened,   6)
#define Status_CALLBACK NULL
#define Status_DEFAULT NULL

extern const pb_msgdesc_t Status_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Status_fields &Status_msg

/* Maximum encoded size of messages (where known) */
#define Status_size                              36

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
