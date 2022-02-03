/**
 * @file ble.h
 * @brief Top-level header file for support of the Bluetooth Low Energy Stack
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#ifndef BLE_H
#define BLE_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
 extern "C" {
#endif

/* ----------------------------------------------------------------------------
 * BLE support header files
 * ------------------------------------------------------------------------- */
#include <ble/build_config.h>
#include <ble/rwble_config.h>
#include <ble/rwble.h>
#include <ble/rwip.h>
#include <ble/rwip_config.h>
#include <ble/_reg_em_ble_cs.h>
#include <ble/_reg_em_ble_ral.h>
#include <ble/_reg_em_ble_rx_cte_desc.h>
#include <ble/_reg_em_ble_rx_desc.h>
#include <ble/_reg_em_ble_tx_desc.h>
#include <ble/_reg_em_ble_wpal.h>
#include <ble/_reg_em_et.h>
#include <ble/_reg_ipcore.h>
#include <ble/_reg_iqgen.h>
#include <ble/_reg_blecore.h>
#include <ble/reg_blecore.h>
#include <ble/reg_access.h>
#include <ble/reg_em_ble_cs.h>
#include <ble/reg_em_ble_ral.h>
#include <ble/reg_em_ble_rx_cte_desc.h>
#include <ble/reg_em_ble_rx_desc.h>
#include <ble/reg_em_ble_tx_desc.h>
#include <ble/reg_em_ble_wpal.h>
#include <ble/reg_em_et.h>
#include <ble/reg_ipcore.h>
#include <ble/reg_iqgen.h>
#include <ble/compiler.h>
#include <ble/arch.h>
#include <ble/co_bt.h>
#include <ble/co_bt_defines.h>
#include <ble/co_endian.h>
#include <ble/co_error.h>
#include <ble/co_hci.h>
#include <ble/co_list.h>
#include <ble/co_llcp.h>
#include <ble/co_lmp.h>
#include <ble/co_version.h>
#include <ble/co_math.h>
#include <ble/co_utils.h>
#include <ble/dbg.h>
#include <ble/dbg_iqgen.h>
#include <ble/dbg_swdiag.h>
#include <ble/dbg_mwsgen.h>
#include <ble/dbg_trc.h>
#include <ble/dbg_trc_config.h>
#include <ble/em_map.h>
#include <ble/em_map_ble.h>
#include <ble/ll.h>
#include <ble/llm.h>
#include <ble/rf.h>
#include <ble/rwip_task.h>
#include <ble/rwble_hl_config.h>
#include <ble/rwble_hl_error.h>
#include <ble/rwble_hl.h>
#include <ble/rwprf_config.h>
#include <ble/ke.h>
#include <ble/ke_event.h>
#include <ble/ke_mem.h>
#include <ble/ke_msg.h>
#include <ble/ke_task.h>
#include <ble/ke_timer.h>
#include <ble/led.h>
#include <ble/nvds.h>
#include <ble/mesh_api.h>
#include <ble/mesh_api_msg.h>
#include <ble/mesh_config.h>
#include <ble/mesh_defines.h>
#include <ble/mesh_version.h>
#include <ble/sch_alarm.h>
#include <ble/sch_arb.h>
#include <ble/sch_plan.h>
#include <ble/sch_prog.h>
#include <ble/sch_slice.h>
#include <ble/aes.h>
#include <ble/prf.h>
#include <ble/ahi.h>
#include <ble/ahi_task.h>
#include <ble/att.h>
#include <ble/attm.h>
#include <ble/ecc_p256.h>
#include <ble/gap.h>
#include <ble/gapc.h>
#include <ble/gapc_task.h>
#include <ble/gapm.h>
#include <ble/gapm_task.h>
#include <ble/gattc.h>
#include <ble/gattc_task.h>
#include <ble/gattm.h>
#include <ble/gattm_task.h>
#include <ble/h4tl.h>
#include <ble/hci.h>
#include <ble/l2cc.h>
#include <ble/l2cc_pdu.h>
#include <ble/l2cc_task.h>
#include <ble/l2cm.h>
#include <ble/prf_types.h>
#include <ble/prf_utils.h>
#include <ble/profiles/anp_common.h>
#include <ble/profiles/blp_common.h>
#include <ble/profiles/cgmp_common.h>
#include <ble/profiles/cpp_common.h>
#include <ble/profiles/cscp_common.h>
#include <ble/profiles/lan_common.h>
#include <ble/profiles/pasp_common.h>
#include <ble/profiles/find_common.h>
#include <ble/profiles/glp_common.h>
#include <ble/profiles/hogp_common.h>
#include <ble/profiles/hrp_common.h>
#include <ble/profiles/htp_common.h>
#include <ble/profiles/rscp_common.h>
#include <ble/profiles/scpp_common.h>
#include <ble/profiles/tip_common.h>
#include <ble/profiles/wpt_common.h>
#include <ble/profiles/anpc_task.h>
#include <ble/profiles/anps_task.h>
#include <ble/profiles/basc_task.h>
#include <ble/profiles/bass_task.h>
#include <ble/profiles/blpc_task.h>
#include <ble/profiles/blps_task.h>
#include <ble/profiles/cgmc_task.h>
#include <ble/profiles/cgms_task.h>
#include <ble/profiles/cppc_task.h>
#include <ble/profiles/cpps_task.h>
#include <ble/profiles/cscpc_task.h>
#include <ble/profiles/cscps_task.h>
#include <ble/profiles/disc_task.h>
#include <ble/profiles/diss_task.h>
#include <ble/profiles/findl_task.h>
#include <ble/profiles/findt_task.h>
#include <ble/profiles/glpc_task.h>
#include <ble/profiles/glps.h>
#include <ble/profiles/glps_task.h>
#include <ble/profiles/hogpbh_task.h>
#include <ble/profiles/hogpd_task.h>
#include <ble/profiles/hogprh_task.h>
#include <ble/profiles/hrpc_task.h>
#include <ble/profiles/hrps_task.h>
#include <ble/profiles/htpc_task.h>
#include <ble/profiles/htpt_task.h>
#include <ble/profiles/lanc_task.h>
#include <ble/profiles/lans_task.h>
#include <ble/profiles/paspc_task.h>
#include <ble/profiles/pasps_task.h>
#include <ble/profiles/proxm_task.h>
#include <ble/profiles/proxr_task.h>
#include <ble/profiles/rscpc_task.h>
#include <ble/profiles/rscps_task.h>
#include <ble/profiles/scppc_task.h>
#include <ble/profiles/scpps_task.h>
#include <ble/profiles/tipc_task.h>
#include <ble/profiles/tips_task.h>
#include <ble/profiles/wptc_task.h>
#include <ble/profiles/wpts_task.h>

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif

#endif /* BLE_H */
